#include "params_recipe_manager.h"

#include <utility>
#include <regex>
#include <iostream>

#include "fmt/format.h"

#include "PAC_err.h"

auto_smart_ptr<ParamsRecipeManager> ParamsRecipeManager::sInstance = nullptr;

ParamsRecipeManager* G_PARAMS_RECIPE_MANAGER()
    {
    return ParamsRecipeManager::getInstance();
    }

ParamsRecipeManager* ParamsRecipeManager::getInstance()
    {
    if ( sInstance.is_null() )
        {
        sInstance = new ParamsRecipeManager();
        }
    return sInstance;
    }

int ParamsRecipeManager::save_device( char* buff ) const
    {
    int size =
        (int)fmt::format_to_n( buff, MAX_COPY_SIZE, "t.RECMAN = \n\t{{" ).size;
    for ( auto rm : recAdapters )
        {
        size += (int)fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t{{" )
                    .size;
        size += (int)fmt::format_to_n(
            buff + size, MAX_COPY_SIZE, "\n\t\t\tCMD=0," )
                    .size;
        auto activeRecipe = rm->getRecStorage()->getActiveRecipeRef();
        size += (int)fmt::format_to_n( buff + size, MAX_COPY_SIZE,
            "\n\t\t\tACT={},", rm->getActiveState() )
                    .size;
        size += (int)fmt::format_to_n( buff + size, MAX_COPY_SIZE,
            "\n\t\t\tNMR={},", rm->getRecStorage()->getActiveRecipe() )
                    .size;
        size += (int)fmt::format_to_n( buff + size, MAX_COPY_SIZE,
            "\n\t\t\tNAME='{}',", activeRecipe.name )
                    .size;

        size += (int)fmt::format_to_n(
            buff + size, MAX_COPY_SIZE, "\n\t\t\tLIST='{}',", rm->recipeList )
                    .size;

        for ( unsigned int recListIdx = 0; recListIdx < rm->recipeLists.size();
            recListIdx++ )
            {
            size += (int)fmt::format_to_n( buff + size, MAX_COPY_SIZE,
                "\n\t\t\tLIST{}='{}',", recListIdx + 1,
                rm->recipeLists[ recListIdx ] )
                        .size;
            }

        size += (int)fmt::format_to_n(
            buff + size, MAX_COPY_SIZE, "\n\t\t\tPAR=\n\t\t\t{{\n\t\t\t" )
                    .size;
        for ( auto par : activeRecipe.params )
            {
            size +=
                (int)fmt::format_to_n( buff + size, MAX_COPY_SIZE, "{},", par )
                    .size;
            }

        size +=
            (int)fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\t}}," )
                .size;

        size += (int)fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t}}," )
                    .size;
        }

    size +=
        (int)fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t}}\n" ).size;

    buff[ size ] = '\0';

    return size;
    }

const char* ParamsRecipeManager::get_name_in_Lua() const
    {
    return "ParamsRecipeManager";
    }

ParamsRecipeStorage* ParamsRecipeManager::createRecipes(
    int size, int quantity )
    {
    auto recStorage = new ParamsRecipeStorage( mId++, size, quantity );
    recPacks.push_back( recStorage );
    return recStorage;
    }

ParamsRecipeManager::ParamsRecipeManager()
    {
    mLastAdapterSaveTimer = get_millisec();
    mLastRecipeSaveTimer = get_millisec();
    mLastTechObjectSaveTimer = get_millisec();
    loadTechObjects();
    }

ParamsRecipeManager::~ParamsRecipeManager()
    {
    for ( auto rs : recPacks )
        {
        delete rs;
        }
    recPacks.clear();
    for ( auto rs : recAdapters )
        {
        delete rs;
        }
    recAdapters.clear();
    }

int ParamsRecipeManager::parseDriverCmd( const char* buff )
    {
    std::string cmd = buff;
    G_LOG->debug( cmd.c_str() );
    std::regex rmCommand(
        R"lit(__RECMAN\[(\d+)\]:set_cmd\( "(\w+)", (\d+), ((?:"(.*)")|([\d\.]+)) \))lit" );
    if ( std::smatch rmMatch; std::regex_match( cmd, rmMatch, rmCommand ) )
        {
        auto recMgrIdx = std::stoi( rmMatch[ 1 ].str() );
        auto varName = rmMatch[ 2 ].str();
        auto idx = std::stoi( rmMatch[ 3 ].str() );
        auto strval = rmMatch[ 5 ].str();
        auto floatval = ( !rmMatch[ 6 ].str().empty() )
                            ? std::stof( rmMatch[ 6 ].str() )
                            : 0;

        if ( recMgrIdx - 1 < (int)recAdapters.size() )
            {
            recAdapters[ recMgrIdx - 1 ]->set_cmd(
                varName, idx, floatval, strval );
            }
        return 1;
        }
    return 0;
    }

ParamsRecipeAdapter* ParamsRecipeManager::createAdapter(
    ParamsRecipeStorage* recStorage )
    {
    auto adapter = new ParamsRecipeAdapter( mAdaptersId++, recStorage );
    recAdapters.push_back( adapter );
    return adapter;
    }

void ParamsRecipeManager::evaluate()
    {
    auto wasLoaded = false;
    for ( auto adapter : recAdapters )
        {
        if ( adapter->isChanged )
            {
            if ( get_delta_millisec( mLastAdapterSaveTimer ) >
                 MIN_SAVE_INTERVAL_MS )
                {
                adapter->isChanged = false;
                adapter->serialize();
                mLastAdapterSaveTimer = get_millisec();
                break;
                }
            }
        if ( adapter->recipeListChanged )
            {
            adapter->recipeListChanged = false;
            adapter->refreshRecipeList();
            }
        if ( adapter->isLoaded )
            {
            if ( get_delta_millisec( mLastTechObjectSaveTimer ) >
                 MIN_SAVE_INTERVAL_MS )
                {
                adapter->isLoaded = false;
                wasLoaded = true;
                }
            }
        }
    if ( wasLoaded )
        {
        mLastTechObjectSaveTimer = get_millisec();
        saveTechObjects();
        }
    for ( auto recipeStorage : recPacks )
        {
        if ( recipeStorage->isChanged )
            {
            if ( get_delta_millisec( mLastRecipeSaveTimer ) >
                 MIN_SAVE_INTERVAL_MS )
                {
                recipeStorage->isChanged = false;
                recipeStorage->serialize();
                mLastRecipeSaveTimer = get_millisec();
                break;
                }
            }
        }
    }

void ParamsRecipeManager::saveTechObjects()
    {
    std::ofstream outfile( "paramstech.serialized" );
    if ( outfile.is_open() )
        {
        auto count = G_TECH_OBJECT_MNGR()->get_count();
        outfile << count << std::endl;
        for ( int i = 0; i < (int)count; i++ )
            {
            outfile << G_TECH_OBJECTS( i )->lastLoadedRecipeName << std::endl;
            outfile << G_TECH_OBJECTS( i )->lastLoadedRecipeNmr << std::endl;
            }
        }
    else
        {
        G_LOG->debug( "Failed to open paramstech.serialized" );
        }
    }

void ParamsRecipeManager::loadTechObjects()
    {
    std::ifstream infile( "paramstech.serialized" );
    if ( infile.is_open() )
        {
        auto count = 0;
        auto techObjectsCount = G_TECH_OBJECT_MNGR()->get_count();
        infile >> count;
        for ( int i = 0; i < count; i++ )
            {
            std::string recipeName;
            int recipeNmr = 0;
            infile.ignore();
            std::getline( infile, recipeName );
            infile >> recipeNmr;
            if ( i < (int)techObjectsCount )
                {
                G_TECH_OBJECTS( i )->lastLoadedRecipeName = recipeName;
                G_TECH_OBJECTS( i )->lastLoadedRecipeNmr = recipeNmr;
                }
            }
        }
    }

ParamsRecipe::ParamsRecipe( std::string name, std::vector<float> params )
    {
    this->name = std::move( name );
    this->params = std::move( params );
    }

ParamsRecipe::ParamsRecipe()
    {
    this->name = "";
    this->params = {};
    }

ParamsRecipeStorage::ParamsRecipeStorage(
    int id, int recipeCount, int recipeParamsCount )
    {
    this->mId = id;
    this->mRecipeCount = recipeCount;
    this->mRecipeParamsCount = recipeParamsCount;
    this->isChanged = false;
    this->mActiveRecipe = 0;
    for ( int i = 0; i < recipeCount; i++ )
        {
        std::vector<float> params( recipeParamsCount, 0 );
        recipes.emplace_back( "none", params );
        }
    deserialize();
    }

int ParamsRecipeStorage::getId() const
    {
    return mId;
    }

int ParamsRecipeStorage::getCount() const
    {
    return this->mRecipeCount;
    }

int ParamsRecipeStorage::getParamsCount() const
    {
    return this->mRecipeParamsCount;
    }

void ParamsRecipeStorage::setRecPar( int recNo, int parNo, float newValue )
    {
    if ( recNo <= mRecipeCount && parNo <= mRecipeParamsCount && recNo > 0 &&
         parNo > 0 )
        {
        if ( recipes[ recNo - 1 ].params[ parNo - 1 ] != newValue )
            {
            isChanged = true;
            recipes[ recNo - 1 ].params[ parNo - 1 ] = newValue;
            }
        }
    }

float ParamsRecipeStorage::getRecPar( uInt recNo, uInt parNo )
    {
    if ( (int)recNo <= mRecipeCount && (int)parNo <= mRecipeParamsCount &&
         (int)recNo > 0 && (int)parNo > 0 )
        {
        return recipes[ recNo - 1 ].params[ parNo - 1 ];
        }
    else
        {
        return 0.0f;
        }
    }

void ParamsRecipeStorage::serialize( const std::string& filename )
    {
    std::ofstream outfile(
        filename ); // create an output file stream with the given filename
    if ( outfile.is_open() )
        { // check if the file is opened successfully
        outfile << mRecipeCount << " " << mRecipeParamsCount
                << "\n";  // write the recipe_count and param_count to the file
        for ( int i = 0; i < mRecipeCount; i++ )
            {  // loop through the recipes vector
            outfile << recipes[ i ].name
                    << "\n";  // write the name of the recipe to the file
            for ( int j = 0; j < mRecipeParamsCount; j++ )
                {  // loop through the params of the recipe
                outfile << recipes[ i ].params[ j ]
                        << ' ';  // write each param to the file
                }
            outfile << "\n";  // write a new line after each recipe
            }
        outfile.close();  // close the file stream
        G_LOG->debug(
            "Successfully serialized the recipes to %s\n", filename.c_str() );
        }
    else
        {
        G_LOG->debug(
            "Failed to open %s for serialization.\n", filename.c_str() );
        }
    }

void ParamsRecipeStorage::deserialize( const std::string& filename )
    {
    std::ifstream infile(
        filename );  // create an input file stream with the given filename
    if ( infile.is_open() )
        {  // check if the file is opened successfully
        int rc,
            pc;  // variables to store the recipe_count and param_count from the file
        infile >> rc >>
            pc;  // read the recipe_count and param_count from the file

        if ( rc != mRecipeCount || pc != mRecipeParamsCount )
            {  // check if they match with the current values
            G_LOG->debug(
                "The file has different values for recipe_count and param_count.\n" );
            G_LOG->debug(
                "Current values: %d, %d.\n", mRecipeCount, mRecipeParamsCount );
            G_LOG->debug( "File values: %d, %d.\n", rc, pc );
            G_LOG->debug( "Proceeding with deserialization anyway.\n" );
            }

        int min_rc = std::min<int>( rc,
            mRecipeCount );  // find the minimum of the two recipe_count values
        int min_pc = std::min<int>( pc,
            mRecipeParamsCount );  // find the minimum of the two param_count values

        for ( int i = 0; i < rc; i++ )
            {  // loop through the recipes in the file
            std::string name;  // variable to store the name of the recipe
            std::vector<float> params( mRecipeParamsCount,
                0 );  // vector to store the params of the recipe
            infile.ignore();  // ignore the newline character
            std::getline(
                infile, name );  // read the name of the recipe from the file
            for ( int j = 0; j < pc; j++ )
                {  // loop through the params in the file
                float param;  // variable to store each param
                infile >> param;  // read each param from the file
                if ( j < min_pc )
                    {
                    params[ j ] = param;  // add the param to the params vector
                    }
                }
            infile.ignore();  // ignore the last space character
            if ( i < min_rc )
                {
                recipes[ i ].name =
                    name;  // replace the recipe at the index i with the deserialized recipe
                recipes[ i ].params = std::move(
                    params );  // replace the recipe at the index i with the deserialized recipe
                }
            }

        infile.close();  // close the file stream
        G_LOG->debug( "Successfully deserialized the recipes from %s.\n",
            filename.c_str() );
        }
    else
        {
        G_LOG->debug(
            "Failed to open %s for deserialization.\n", filename.c_str() );
        }
    }

int ParamsRecipeStorage::getActiveRecipe() const
    {
    return mActiveRecipe + 1;
    }

int ParamsRecipeStorage::setActiveRecipe( int recipe )
    {
    if ( recipe - 1 < mRecipeCount && recipe > 0 )
        {
        if ( mActiveRecipe != recipe - 1 )
            {
            mActiveRecipe = recipe - 1;
            return recipe - 1;
            }
        }
    return -1;
    }

ParamsRecipe& ParamsRecipeStorage::getActiveRecipeRef()
    {
    if ( mActiveRecipe < mRecipeCount )
        {
        return recipes[ mActiveRecipe ];
        }
    return recipes[ 0 ];
    }

void ParamsRecipeStorage::serialize()
    {
    // Use fmt::format for efficient string construction without temporary objects
    std::string filename = fmt::format( "recipes_{}.serialized", mId );
    serialize( filename );
    }

void ParamsRecipeStorage::deserialize()
    {
    // Use fmt::format for efficient string construction without temporary objects
    std::string filename = fmt::format( "recipes_{}.serialized", mId );
    deserialize( filename );
    }

void ParamsRecipeAdapter::addMap(
    unsigned int startRecPar, unsigned int startObjPar, unsigned int quantity )
    {
    if ( startRecPar > 0 && startObjPar > 0 && quantity > 0 )
        {
        mParamsMap.emplace_back( startRecPar, startObjPar, quantity );
        }
    else
        {
        G_LOG->error(
            "Values can't be zero. ParamsRecipeAdapter::addMap: startRecPar=%d, startObjPar=%d, quantity=%d\n",
            startRecPar, startObjPar, quantity );
        }
    }

void ParamsRecipeAdapter::loadParams( int techObject, unsigned int recNo )
    {
    auto recStorage = mRecStorage;
    if ( (int)G_TECH_OBJECT_MNGR()->get_count() < techObject || techObject < 1 )
        return;
    auto techObj = G_TECH_OBJECTS( techObject - 1 );
    if ( recStorage == nullptr || techObj == nullptr ) return;
    if ( (int)recNo > recStorage->getCount() || recNo == 0 ) return;
    auto recSize = recStorage->getParamsCount();
    auto parSize = techObj->par_float.get_count();
    for ( auto map : mParamsMap )
        {
        if ( (int)map.startRecPar + (int)map.quantity > recSize + 1 ||
             map.startObjPar + map.quantity > (int)parSize )
            return;
        for ( int i = 0; i < (int)map.quantity; i++ )
            {
            techObj->par_float[ map.startObjPar + i ] =
                recStorage->getRecPar( recNo, map.startRecPar + i );
            }
        }
    techObj->par_float.save_all();
    techObj->lastLoadedRecipeNmr = (int)recNo;
    techObj->lastLoadedRecipeName = recStorage->recipes[ recNo - 1 ].name;
    isLoaded = true;
    }

ParamsRecipeAdapter::ParamsRecipeAdapter(
    int id, ParamsRecipeStorage* recStorage )
    {
    this->mId = id;
    this->mRecStorage = recStorage;
    auto recipesCount = recStorage->getCount();
    mActiveRecipes.assign( recipesCount, 1 );
    auto recipeListsCount =
        recipesCount / 10 + ( ( recipesCount % 10 > 0 ) ? 1 : 0 );
    recipeLists.assign( recipeListsCount, "" );
    deserialize();
    refreshRecipeList();
    }

void ParamsRecipeAdapter::serialize( const std::string& filename )
    {
    std::ofstream outfile(
        filename );  // create an output file stream with the given filename
    if ( outfile.is_open() )
        {
        outfile << mActiveRecipes.size() << "\n";  // write the recipe_count
        for ( int mActiveRecipe : mActiveRecipes )
            {
            outfile
                << mActiveRecipe
                << " ";  // write the active state of the recipe to the file separated by space
            }
        outfile.close();  // close the file stream
        }
    else
        {
        G_LOG->debug(
            "Failed to open %s for serialization.\n", filename.c_str() );
        }
    }

void ParamsRecipeAdapter::serialize()
    {
    // Use fmt::format for efficient string construction without temporary objects
    serialize( fmt::format( "paramsadapter_{}.serialized", mId ) );
    }

void ParamsRecipeAdapter::deserialize( const std::string& filename )
    {
    std::ifstream infile(
        filename );  // create an input file stream with the given filename
    if ( infile.is_open() )
        {
        int recipeCount;
        int actualRecipeCount = (int)mActiveRecipes.size();
        infile >> recipeCount;
        if ( recipeCount > (int)mActiveRecipes.size() )
            {  // check if they match with the current values
            G_LOG->debug( "The file has different values for recipe_count.\n" );
            }
        for ( int i = 0; i < recipeCount; i++ )
            {
            int active;
            infile >> active;
            if ( i < actualRecipeCount )
                {
                mActiveRecipes[ i ] = active;
                }
            }
        }
    else
        {
        G_LOG->debug(
            "Failed to open %s for deserialization.\n", filename.c_str() );
        }
    }

void ParamsRecipeAdapter::deserialize()
    {
    // Use fmt::format for efficient string construction without temporary objects
    deserialize( fmt::format( "paramsadapter_{}.serialized", mId ) );
    }

int ParamsRecipeAdapter::getActiveState() const
    {
    return mActiveRecipes[ mRecStorage->getActiveRecipe() - 1 ];
    }

void ParamsRecipeAdapter::setActiveState( int state )
    {
    if ( state )
        {
        mActiveRecipes[ mRecStorage->getActiveRecipe() - 1 ] = 1;
        }
    else
        {
        mActiveRecipes[ mRecStorage->getActiveRecipe() - 1 ] = 0;
        }
    isChanged = true;
    }

ParamsRecipeStorage* ParamsRecipeAdapter::getRecStorage() const
    {
    return mRecStorage;
    }

int ParamsRecipeAdapter::set_cmd( const std::string_view& varName, int index,
    float value, const std::string_view& strValue )
    {
    if ( varName == "ACT" )
        {
        setActiveState( (int)value );
        recipeListChanged = true;
        }
    if ( varName == "CMD" )
        {
        auto command = (int)value;
        if ( command == 789 )
            {
            mRecStorage->deserialize();
            }
        if ( command > 1000 )
            {
            auto objNmr = command / 1000;
            auto recNmr = command % 1000;
            loadParams( objNmr, recNmr );
            }
        }
    if ( varName == "NAME" )
        {
        mRecStorage->getActiveRecipeRef().name = strValue;
        mRecStorage->isChanged = true;
        recipeListChanged = true;
        return 1;
        }
    if ( varName == "NMR" )
        {
        mRecStorage->setActiveRecipe( (int)value );
        }
    if ( varName == "PAR" )
        {
        if ( index <= mRecStorage->getParamsCount() && index > 0 )
            {
            if ( mRecStorage->getActiveRecipeRef().params[ index - 1 ] !=
                 value )
                {
                mRecStorage->getActiveRecipeRef().params[ index - 1 ] = value;
                mRecStorage->isChanged = true;
                }
            }
        }
    return 0;
    }

void ParamsRecipeAdapter::refreshRecipeList()
    {
    recipeList.clear();
    for ( auto recList : recipeLists )
        {
        recList.clear();
        }
    for ( int i = 0; i < mRecStorage->getCount(); i++ )
        {
        if ( mActiveRecipes[ i ] )
            {
            auto recString = fmt::format(
                "{}##{}||", i + 1, mRecStorage->recipes[ i ].name );
            if ( !useSeparateRecipeList )
                {
                recipeList.append( recString );
                }
            else
                {
                auto recListIndex = (unsigned int)i / 10;
                if ( recListIndex < recipeLists.size() )
                    {
                    recipeLists[ recListIndex ].append( recString );
                    }
                }
            }
        }
    }

void ParamsRecipeAdapter::setUseSeparateRecipeList( bool state )
    {
    useSeparateRecipeList = state;
    refreshRecipeList();
    }


ParamsRecipeMapRecord::ParamsRecipeMapRecord(
    uInt startRecPar, uInt startObjPar, uInt quantity ) :
    startRecPar( startRecPar ), startObjPar( startObjPar ), quantity( quantity )
    {
    }
