#include "params_recipe_manager_tests.h"

using namespace ::testing;

class ParamsRecipeStorageTest : public testing::Test
    {
    protected:
        // define a pointer to the RecipeManager object
        ParamsRecipeStorage* storage{};

        // define a method to set up the test fixture
        void SetUp() override
            {
            // create a RecipeManager object with 3 recipes and 4 params per recipe
            storage = new ParamsRecipeStorage( 1, 3, 5 );
            }

        // define a method to tear down the test fixture
        void TearDown() override
            {
            // delete the RecipeManager object
            delete storage;
            }
    };

// define a test case for serializing the recipes to a file
TEST_F( ParamsRecipeStorageTest, Serialize )
    {
    // create a vector of params for the recipe
    std::vector<float> params = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    storage->recipes[ 0 ].name = "Test1";
    storage->recipes[ 0 ].params = params;
    // serialize the recipes to a file named "test.txt"
    storage->serialize( "test.txt" );
    // create an input file stream with the file name
    std::ifstream infile( "test.txt" );
    // check if the file is opened successfully
    ASSERT_TRUE( infile.is_open() );
    // create variables to store the values from the file
    int rc, pc;
    std::string name;
    float p1, p2, p3, p4;
    // read the values from the file
    infile >> rc >> pc;
    infile.ignore();
    std::getline( infile, name );
    infile >> p1 >> p2 >> p3 >> p4;
    // close the file stream
    infile.close();
    // check if the values from the file match with the expected values
    EXPECT_EQ( rc, 3 );
    EXPECT_EQ( pc, 5 );
    EXPECT_EQ( name, "Test1" );
    EXPECT_EQ( p1, 1.0 );
    EXPECT_EQ( p2, 2.0 );
    EXPECT_EQ( p3, 3.0 );
    EXPECT_EQ( p4, 4.0 );
    }

// define a test case for deserializing the recipes from a file
TEST_F( ParamsRecipeStorageTest, Deserialize )
    {
    // create a vector of params for the recipe
    std::vector<float> params = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    storage->recipes[ 0 ].name = "Test1";
    storage->recipes[ 0 ].params = params;
    // serialize the recipes to a file named "test1.txt"
    storage->serialize( "test1.txt" );

    // deserialize the recipes from a file named "test1.txt"
    storage->deserialize( "test1.txt" );
    // check if the name of the recipe at index 0 is equal to "Test1"
    EXPECT_EQ( storage->recipes[ 0 ].name, "Test1" );
    // check if the params of the recipe at index 0 are equal to {1.0, 2.0, 3.0, 4.0, 5.0}
    EXPECT_EQ( storage->recipes[ 0 ].params[ 0 ], 1.0 );
    EXPECT_EQ( storage->recipes[ 0 ].params[ 1 ], 2.0 );
    EXPECT_EQ( storage->recipes[ 0 ].params[ 2 ], 3.0 );
    EXPECT_EQ( storage->recipes[ 0 ].params[ 3 ], 4.0 );
    EXPECT_EQ( storage->recipes[ 0 ].params[ 4 ], 5.0 );
    }

TEST_F( ParamsRecipeStorageTest, GetId )
    {
    EXPECT_EQ( 1, storage->getId() );
    }

TEST_F( ParamsRecipeStorageTest, GetCount )
    {
    EXPECT_EQ( 3, storage->getCount() );
    }

TEST_F( ParamsRecipeStorageTest, GetParamsCount )
    {
    EXPECT_EQ( 5, storage->getParamsCount() );
    }

TEST_F( ParamsRecipeStorageTest, SetRecPar )
    {
    storage->setRecPar( 1, 1, 42.0f );
    EXPECT_EQ( 42.0f, storage->getRecPar( 1, 1 ) );
    }

TEST_F( ParamsRecipeStorageTest, GetActiveRecipe )
    {
    EXPECT_EQ( 1, storage->getActiveRecipe() );
    storage->setActiveRecipe( 2 );
    EXPECT_EQ( 2, storage->getActiveRecipe() );
    }

TEST_F( ParamsRecipeStorageTest, GetActiveRecipeRef )
    {
    storage->setActiveRecipe( 1 );
    storage->recipes[ 0 ].name = "Test1";
    EXPECT_EQ( 5, storage->getActiveRecipeRef().name.length() );
    }


class ParamsRecipeTest : public ::testing::Test
    {
    protected:
        ParamsRecipe recipe;

        void SetUp() override
            {
            recipe = ParamsRecipe( "My Recipe", { 1.0f, 2.0f, 3.0f } );
            }
    };

TEST_F( ParamsRecipeTest, NameTest )
    {
    EXPECT_EQ( recipe.name, "My Recipe" );
    }

TEST_F( ParamsRecipeTest, ParamsTest )
    {
    std::vector<float> expected_params = { 1.0f, 2.0f, 3.0f };
    EXPECT_EQ( recipe.params, expected_params );
    }

TEST_F( ParamsRecipeTest, DefaultConstructorTest )
    {
    ParamsRecipe default_recipe;
    EXPECT_EQ( default_recipe.name, "" );
    EXPECT_EQ( default_recipe.params.size(), 0 );
    }

class ParamsRecipeManagerTest : public ::testing::Test
    {
    protected:
        virtual void SetUp()
            {
            m_paramsRecipeManager = G_PARAMS_RECIPE_MANAGER();
            }

        virtual void TearDown()
            {
            }

        ParamsRecipeManager* m_paramsRecipeManager;
    };

TEST_F( ParamsRecipeManagerTest, CreateRecipes )
    {
    int size = 5;
    int quantity = 3;
    ParamsRecipeStorage* recipes1 =
        m_paramsRecipeManager->createRecipes( size, quantity );
    ParamsRecipeStorage* recipes2 =
        m_paramsRecipeManager->createRecipes( size, quantity );
    EXPECT_EQ( recipes1->getCount(), size );
    for ( int i = 0; i < quantity; i++ )
        {
        EXPECT_EQ( recipes1->recipes[ i ].params.size(), quantity );
        }
    delete recipes1;
    delete recipes2;
    m_paramsRecipeManager->recPacks.clear();
    }

TEST_F( ParamsRecipeManagerTest, CreateAdapter )
    {
    ParamsRecipeStorage* recipes = m_paramsRecipeManager->createRecipes( 5, 3 );
    ParamsRecipeAdapter* adapter =
        m_paramsRecipeManager->createAdapter( recipes );
    EXPECT_EQ( adapter->getRecStorage(), recipes );
    delete adapter;
    delete recipes;
    m_paramsRecipeManager->recPacks.clear();
    m_paramsRecipeManager->recAdapters.clear();
    }

TEST_F( ParamsRecipeManagerTest, save_device )
    {
    const int BUFF_SIZE = 1000;
    char buff[ BUFF_SIZE ] = { 0 };
    m_paramsRecipeManager->save_device( buff );
    EXPECT_STREQ( "t.RECMAN = \n\t{\n\t}\n", buff );

    memset( buff, '\0', sizeof( buff ) );
    m_paramsRecipeManager->createAdapter(
        m_paramsRecipeManager->createRecipes( 5, 3 ) );
    m_paramsRecipeManager->save_device( buff );
    auto REF_STR0 = R"(t.RECMAN = 
	{
		{
			CMD=0,
			ACT=1,
			NMR=1,
			NAME='none',
			LIST='1##none||2##none||3##none||4##none||5##none||',
			LIST1='',
			PAR=
			{
			0,0,0,
			},
		},
	}
)";
    EXPECT_STREQ( REF_STR0, buff );

    m_paramsRecipeManager->recPacks.clear();
    m_paramsRecipeManager->recAdapters.clear();
    }

TEST_F( ParamsRecipeManagerTest, evaluate )
    {
    m_paramsRecipeManager->createAdapter(
        m_paramsRecipeManager->createRecipes( 4, 3 ) );
    m_paramsRecipeManager->createAdapter(
        m_paramsRecipeManager->createRecipes( 2, 1 ) );

    m_paramsRecipeManager->recAdapters[ 1 ]->isChanged = true;
    m_paramsRecipeManager->recPacks[ 1 ]->isChanged = true;
    m_paramsRecipeManager->recAdapters[ 0 ]->recipeListChanged = true;
    m_paramsRecipeManager->recAdapters[ 0 ]->isLoaded = true;

    DeltaMilliSecSubHooker::set_millisec( 10001UL );
    m_paramsRecipeManager->evaluate();
    DeltaMilliSecSubHooker::set_default_time();

    EXPECT_FALSE( m_paramsRecipeManager->recAdapters[ 1 ]->isChanged );
    EXPECT_FALSE( m_paramsRecipeManager->recPacks[ 1 ]->isChanged );
    EXPECT_FALSE( m_paramsRecipeManager->recAdapters[ 0 ]->recipeListChanged );
    EXPECT_FALSE( m_paramsRecipeManager->recAdapters[ 0 ]->isLoaded );

    m_paramsRecipeManager->recPacks.clear();
    m_paramsRecipeManager->recAdapters.clear();
    }

TEST_F( ParamsRecipeManagerTest, get_lua_name )
    {
    std::string buff = m_paramsRecipeManager->get_name_in_Lua();
    EXPECT_STREQ( "ParamsRecipeManager", buff.c_str() );
    }

TEST_F( ParamsRecipeManagerTest, pars_cmd )
    {
    std::string cmd = "__RECMAN[1]:set_cmd(hello, 1, 2.5)";
    int val = m_paramsRecipeManager->parseDriverCmd( cmd.c_str() );
    EXPECT_EQ( 0, val );

    cmd = "__RECMAN[1]:set_cmd( \"hello\", 1, 2.5 )";
    val = m_paramsRecipeManager->parseDriverCmd( cmd.c_str() );
    EXPECT_EQ( 1, val );
    }

class ParamsRecipeAdapterTest : public ::testing::Test
    {
    protected:
        ParamsRecipeStorage* m_recipes;
        ParamsRecipeAdapter* m_adapter;

        virtual void SetUp() override
            {
            m_recipes = new ParamsRecipeStorage( 1, 3, 5 );
            m_adapter = new ParamsRecipeAdapter( 1, m_recipes );
            }

        virtual void TearDown() override
            {
            delete m_adapter;
            delete m_recipes;
            }
    };

TEST_F( ParamsRecipeAdapterTest, SetActiveState )
    {
    m_adapter->setActiveState( 1 );
    EXPECT_EQ( m_adapter->getActiveState(), 1 );
    }

TEST_F( ParamsRecipeAdapterTest, set_cmd )
    {
    EXPECT_FALSE( m_adapter->recipeListChanged );
    int returned_value;
    returned_value = m_adapter->set_cmd( "ACT", 0, 1, "" );
    EXPECT_EQ( 0, returned_value );
    EXPECT_TRUE( m_adapter->recipeListChanged );

    returned_value = m_adapter->set_cmd( "NAME", 0, 0, "SUPER_ADAPTER" );
    EXPECT_EQ( 1, returned_value );
    EXPECT_TRUE( m_adapter->recipeListChanged );
    EXPECT_TRUE( m_adapter->isChanged );

    returned_value = m_adapter->set_cmd( "NMR", 0, 1, "" );
    EXPECT_EQ( 0, returned_value );

    m_adapter->serialize();
    returned_value = m_adapter->set_cmd( "CMD", 0, 1001, "" );
    EXPECT_EQ( 0, returned_value );

    returned_value = m_adapter->set_cmd( "CMD", 0, 789, "" );
    EXPECT_EQ( 0, returned_value );

    returned_value = m_adapter->set_cmd( "PAR", 1, 0, "SUPER_ADAPTER" );
    EXPECT_EQ( 0, returned_value );
    const ParamsRecipeStorage* temp = m_adapter->getRecStorage();
    EXPECT_TRUE( temp->isChanged );

    returned_value = m_adapter->set_cmd( "HELLO", 10, 10, "NEW_NAME" );
    EXPECT_EQ( 0, returned_value );
    }
