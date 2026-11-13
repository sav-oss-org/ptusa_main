#ifndef PTUSA_MAIN_PARAMRECIPESMANAGER_H
#define PTUSA_MAIN_PARAMRECIPESMANAGER_H

#include "smart_ptr.h"
#include "g_device.h"
#include "tech_def.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class ParamsRecipe
    {
    public:
        std::string name; // the name of the recipe
        std::vector<float> params; // the array of float parameters

        // constructor with name and params
        ParamsRecipe( std::string name, std::vector<float> params );

        // default constructor with empty name and params
        ParamsRecipe();
    };

class ParamsRecipeStorage
    {
    private:
        int mRecipeCount;
        int mRecipeParamsCount;
        int mId;
        int mActiveRecipe;

    public:
        ParamsRecipeStorage( int id, int recipeCount, int recipeParamsCount );
        int getId() const;
        int getCount() const;
        int getParamsCount() const;
        bool isChanged;
        void setRecPar( int recNo, int parNo, float newValue );
        float getRecPar( uInt recNo, uInt parNo );
        void serialize( const std::string& filename );
        void serialize();
        void deserialize();
        void deserialize( const std::string& filename );
        int getActiveRecipe() const;
        int setActiveRecipe( int recipe );
        ParamsRecipe& getActiveRecipeRef();
        std::vector<ParamsRecipe> recipes;
    };

struct ParamsRecipeMapRecord
    {
        uInt startRecPar;
        uInt startObjPar;
        uInt quantity;
        ParamsRecipeMapRecord(
            uInt startRecPar, uInt startObjPar, uInt quantity );
    };

class ParamsRecipeAdapter
    {
    private:
        std::vector<ParamsRecipeMapRecord> mParamsMap;
        std::vector<int> mActiveRecipes;
        ParamsRecipeStorage* mRecStorage;
        int mId;
        void serialize( const std::string& filename );
        void deserialize( const std::string& filename );

    public:
        explicit ParamsRecipeAdapter( int id, ParamsRecipeStorage* recStorage );
        void addMap( unsigned int startRecPar, unsigned int startObjPar,
            unsigned int quantity );
        void loadParams( int techObject, unsigned int recNo );
        void serialize();
        void deserialize();
        int getActiveState() const;
        ParamsRecipeStorage* getRecStorage() const;
        void setActiveState( int state );
        bool isChanged = false;
        bool isLoaded = false;
        bool recipeListChanged = false;
        void refreshRecipeList();
        std::string recipeList;
        std::vector<std::string> recipeLists;
        bool useSeparateRecipeList = false;
        void setUseSeparateRecipeList( bool state );
        int set_cmd( const std::string_view& varName, int index, float value,
            const std::string_view& strValue );
    };

class ParamsRecipeManager : public i_Lua_save_device
    {
    public:
        void evaluate();
        ParamsRecipeStorage* createRecipes( int size, int quantity );
        ParamsRecipeAdapter* createAdapter( ParamsRecipeStorage* recStorage );
        ~ParamsRecipeManager();
        static ParamsRecipeManager* getInstance();

        int save_device( char* buff ) const override;
        int parseDriverCmd( const char* buff );

        const char* get_name_in_Lua() const override;

        std::vector<ParamsRecipeStorage*> recPacks;
        std::vector<ParamsRecipeAdapter*> recAdapters;

        static void saveTechObjects();
        static void loadTechObjects();

        ParamsRecipeManager( ParamsRecipeManager& outer ) = delete;
        void operator=( const ParamsRecipeManager& ) = delete;

        const unsigned long MIN_SAVE_INTERVAL_MS = 10000;

    protected:
        ParamsRecipeManager();

    private:
        int mId = 0;
        int mAdaptersId = 0;
        uint32_t mLastAdapterSaveTimer;
        uint32_t mLastRecipeSaveTimer;
        uint32_t mLastTechObjectSaveTimer;
        static auto_smart_ptr<ParamsRecipeManager> sInstance;
    };

ParamsRecipeManager* G_PARAMS_RECIPE_MANAGER();

#endif //PTUSA_MAIN_PARAMRECIPESMANAGER_H
