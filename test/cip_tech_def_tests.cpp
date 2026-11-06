#include "cip_tech_def_tests.h"
#include "lua_manager.h"
#include <cstring>

using namespace ::testing;

void InitStationParams()
    {
    printf( "===================================Init station params\n" );
    cipline_tech_object::set_station_par( 1, 1.5f );
    cipline_tech_object::set_station_par( 2, 0.3f );
    cipline_tech_object::set_station_par( 3, 0.1f );
    cipline_tech_object::set_station_par( 4, 1.0f );
    cipline_tech_object::set_station_par( 5, 0.2f );
    cipline_tech_object::set_station_par( 6, 0.1f );
    cipline_tech_object::set_station_par( 7, 20 );
    cipline_tech_object::set_station_par( 8, 5 );
    cipline_tech_object::set_station_par( 9, 0 );
    cipline_tech_object::set_station_par( 10, 8 );
    cipline_tech_object::set_station_par( 11, 1.51f );
    cipline_tech_object::set_station_par( 12, 1.8f );
    cipline_tech_object::set_station_par( 13, 47 );
    cipline_tech_object::set_station_par( 14, 57 );
    cipline_tech_object::set_station_par( 15, 1.47f );
    cipline_tech_object::set_station_par( 16, 1.45f );
    cipline_tech_object::set_station_par( 17, 1 );
    cipline_tech_object::set_station_par( 18, 52 );
    cipline_tech_object::set_station_par( 19, 0.5f );
    cipline_tech_object::set_station_par( 20, 30.8f );
    cipline_tech_object::set_station_par( 21, 0.4f );
    cipline_tech_object::set_station_par( 22, 30 );
    cipline_tech_object::set_station_par( 23, 120 );
    cipline_tech_object::set_station_par( 24, 50 );
    cipline_tech_object::set_station_par( 25, 10000 );
    cipline_tech_object::set_station_par( 26, 10000 );
    cipline_tech_object::set_station_par( 27, 10000 );
    cipline_tech_object::set_station_par( 28, 100 );
    cipline_tech_object::set_station_par( 29, 100 );
    cipline_tech_object::set_station_par( 30, 50 );
    cipline_tech_object::set_station_par( 31, 0 );
    cipline_tech_object::set_station_par( 32, 0 );
    cipline_tech_object::set_station_par( 33, 12 );
    cipline_tech_object::set_station_par( 34, 11 );
    cipline_tech_object::set_station_par( 35, 20 );
    }

void InitCipDevices()
    {
    auto dm = device_manager::get_instance();
    dm->clear_io_devices();
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V0", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V1", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V10", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V11", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V12", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V13", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V2", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V3", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V4", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V5", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V6", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V7", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V8", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V9", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "V101", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "V102", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_VC, device::DEVICE_SUB_TYPE::DST_VC_VIRT, "LINE1VC14", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_M, device::DEVICE_SUB_TYPE::DST_M_VIRT, "LINE1M1", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LINE1LS1", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LINE1LS2", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LINE1LS3", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS4", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS5", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS6", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS7", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS8", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS9", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_TE, device::DEVICE_SUB_TYPE::DST_TE_VIRT, "LINE1TE1", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_TE, device::DEVICE_SUB_TYPE::DST_TE_VIRT, "LINE1TE2", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_FS, device::DEVICE_SUB_TYPE::DST_FS_VIRT, "LINE1FS1", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_FQT, device::DEVICE_SUB_TYPE::DST_FQT_VIRT, "LINE1FQT1", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_QT, device::DEVICE_SUB_TYPE::DST_QT_VIRT, "LINE1QT1", "", "" );

    //Watchdog device.
    dm->add_io_device( device::DEVICE_TYPE::DT_WATCHDOG, device::DEVICE_SUB_TYPE::DST_WATCHDOG, "LINE1WATCHDOG1", "", "" );
    }

void ClearCipDevices()
    {
    device_manager::get_instance()->clear_io_devices();
    }


TEST( cipline_tech_object, lua_init_params )
    {
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    const auto PAR_VALUE = 1.5f;
    cipline_tech_object::set_station_par( P_CZAD_S, PAR_VALUE );
    EXPECT_EQ( cip1.get_station_par( P_CZAD_S ), PAR_VALUE );
    cip1.lua_init_params();
    EXPECT_EQ( cip1.get_station_par( P_CZAD_S ), 0.0f );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, lua_init_runtime_params )
    {
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    // После создания объекта параметры должны иметь 0-е значения.
    EXPECT_EQ( cip1.rt_par_float[ workParameters::P_R_NO_FLOW ], 0.0f );
    cip1.lua_init_runtime_params();
    const auto PAR_VALUE = 2.f;
    // После инициализации рабочих параметров должны быть значения по умолчанию.
    EXPECT_EQ( cip1.rt_par_float[ workParameters::P_R_NO_FLOW ], PAR_VALUE );

    G_LUA_MANAGER->free_Lua();
    }


TEST( cipline_tech_object, _CheckErr )
    {
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    virtual_device temp_sensor_supply( "LINE1TE1", device::DT_TE, device::DST_TE_VIRT );
    virtual_device temp_sensor_return( "LINE1TE2", device::DT_TE, device::DST_TE_VIRT );
    virtual_device conductivity_sensor( "LINE1QT1", device::DT_QT, device::DST_QT_VIRT );
    cip1.TP = reinterpret_cast<i_AI_device*>( &temp_sensor_supply );
    cip1.TR = reinterpret_cast<i_AI_device*>( &temp_sensor_return );
    cip1.Q = reinterpret_cast<i_AI_device*>( &conductivity_sensor );
    EXPECT_EQ( 0, cip1._CheckErr() );
    temp_sensor_supply.set_state( -1 );
    EXPECT_EQ( ERR_SUPPLY_TEMP_SENSOR, cip1._CheckErr() );
    temp_sensor_supply.set_state( 0 );
    temp_sensor_return.set_state( -1 );
    EXPECT_EQ( ERR_RETURN_TEMP_SENSOR, cip1._CheckErr() );
    temp_sensor_return.set_state( 0 );
    conductivity_sensor.set_state( -1 );
    EXPECT_EQ( 0, cip1._CheckErr() );
    for ( int step = 0; step < 100; step++ )
        {
        cip1.curstep = step;
        if ( cipline_tech_object::steps_caustic.count( step ) || cipline_tech_object::steps_acid.count( step ) )
            {
            EXPECT_EQ( ERR_CONCENTRATION_SENSOR, cip1._CheckErr() );
            }
        else
            {
            EXPECT_EQ( 0, cip1._CheckErr() );
            }
        }
    temp_sensor_supply.set_state( 0 );
    temp_sensor_return.set_state( 0 );
    conductivity_sensor.set_state( 0 );
    cip1.curstep = 0;
    temp_sensor_supply.set_value( -1 );
    EXPECT_EQ( ERR_SUPPLY_TEMP_SENSOR, cip1._CheckErr() );
    temp_sensor_supply.set_value( 0 );
    temp_sensor_return.set_value( -1 );
    EXPECT_EQ( ERR_RETURN_TEMP_SENSOR, cip1._CheckErr() );

    temp_sensor_supply.set_value( 0 );
    temp_sensor_return.set_value( 0 );
    temp_sensor_return.set_state( -1 );
    temp_sensor_supply.set_state( -1 );
    conductivity_sensor.set_state( -1 );
    cip1.curstep = *( cipline_tech_object::steps_caustic.begin() );
    cipline_tech_object::parpar[ 0 ][ P_BLOCK_ERRORS ] =
        1 << BE_ERR_SUPPLY_TEMP_SENSOR | 1 << BE_ERR_RETURN_TEMP_SENSOR;
    EXPECT_EQ( ERR_CONCENTRATION_SENSOR, cip1._CheckErr() );
    cipline_tech_object::parpar[ 0 ][ P_BLOCK_ERRORS ] =
        1 << BE_ERR_SUPPLY_TEMP_SENSOR | 1 << BE_ERR_CONCENTRATION_SENSOR;
    EXPECT_EQ( ERR_RETURN_TEMP_SENSOR, cip1._CheckErr() );
    cipline_tech_object::parpar[ 0 ][ P_BLOCK_ERRORS ] =
        1 << BE_ERR_RETURN_TEMP_SENSOR | 1 << BE_ERR_CONCENTRATION_SENSOR;
    EXPECT_EQ( ERR_SUPPLY_TEMP_SENSOR, cip1._CheckErr() );
    cipline_tech_object::parpar[ 0 ][ P_BLOCK_ERRORS ] =
        1 << BE_ERR_RETURN_TEMP_SENSOR | 1 << BE_ERR_CONCENTRATION_SENSOR | 1 << BE_ERR_SUPPLY_TEMP_SENSOR;
    EXPECT_EQ( 0, cip1._CheckErr() );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, evaluate )
    {

    lua_manager::get_instance()->set_Lua( luaL_newstate() );
    auto* cip1 = new cipline_tech_object(
        "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );

    cip1->initline();
    EXPECT_EQ( 0, cip1->evaluate() );

    cip1->rt_par_float[ P_SELECT_REC ] = 1;
    EXPECT_EQ( 0, cip1->evaluate() );

    cipline_tech_object::statsbase->apply();
    cipline_tech_object::statsbase->evaluate();

    delete cip1;

    cip1 = new cipline_tech_object(
        "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    cip1->initline();
    EXPECT_EQ( 0, cip1->evaluate() );

    EXPECT_EQ( nullptr, cip1->PRESSURE );
    auto current_recipe = cip1->lineRecipes->getCurrentRecipe();
    cip1->lineRecipes->setRecipeValue( current_recipe, TRecipeManager::RV_TO_DEFAULTS, 1 );
    cip1->lineRecipes->EvalRecipe();
    EXPECT_EQ( 0, cip1->lineRecipes->getRecipeValue( current_recipe, TRecipeManager::RV_TO_DEFAULTS ) );
    EXPECT_EQ( 200, cip1->lineRecipes->getRecipeValue( current_recipe, TRecipeManager::RV_V1 ) );
    EXPECT_EQ( 0, cip1->evaluate() );
    cip1->rt_par_float[ P_SELECT_REC ] = 1;
    EXPECT_EQ( 0, cip1->evaluate() );
    cip1->rt_par_float[ P_SELECT_PRG ] = SPROG_CAUSTIC_ACID_SANITIZER;
    cip1->SetCommand( MCMD_EVALUATE );
    EXPECT_EQ( 0, cip1->state );
    EXPECT_EQ( 0, cip1->evaluate() );
    EXPECT_NE( 0, cip1->state );

    delete cip1;
    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, _LoadProgram )
    {
    lua_manager::get_instance()->set_Lua( luaL_newstate() );
    auto* cip1 = new cipline_tech_object(
        "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );

    cip1->initline();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_RINSING_CLEAN;
    EXPECT_EQ( 83, cip1->_LoadProgram() );
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_SANITIZER;
    EXPECT_EQ( 71, cip1->_LoadProgram() );
    EXPECT_EQ( true, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_HOTWATER;
    EXPECT_EQ( 61, cip1->_LoadProgram() );
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( true, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_RINSING;
    EXPECT_EQ( 5, cip1->_LoadProgram() );
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_RINSING_HOTWATER;
    EXPECT_EQ( 5, cip1->_LoadProgram() );
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( true, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_ACID;
    EXPECT_EQ( 5, cip1->_LoadProgram() );
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( true, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_ACID_SANITIZER;
    EXPECT_EQ( 5, cip1->_LoadProgram() );
    EXPECT_EQ( true, cip1->program_has_cold_desinfection );
    EXPECT_EQ( true, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_ACID_HOTWATER;
    EXPECT_EQ( 5, cip1->_LoadProgram() );
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( true, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( true, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_CAUSTIC;
    EXPECT_EQ( 5, cip1->_LoadProgram() );
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( true, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_CAUSTIC_SANITIZER;
    EXPECT_EQ( 5, cip1->_LoadProgram() );
    EXPECT_EQ( true, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( true, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_CAUSTIC_HOTWATER;
    EXPECT_EQ( 5, cip1->_LoadProgram() );
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( true, cip1->program_has_caustic );
    EXPECT_EQ( true, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_CAUSTIC_ACID;
    EXPECT_EQ( 5, cip1->_LoadProgram() );
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( true, cip1->program_has_acid );
    EXPECT_EQ( true, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_CAUSTIC_ACID_SANITIZER;
    EXPECT_EQ( 5, cip1->_LoadProgram() );
    EXPECT_EQ( true, cip1->program_has_cold_desinfection );
    EXPECT_EQ( true, cip1->program_has_acid );
    EXPECT_EQ( true, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_CAUSTIC_ACID_HOTWATER;
    EXPECT_EQ( 5, cip1->_LoadProgram() );
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( true, cip1->program_has_acid );
    EXPECT_EQ( true, cip1->program_has_caustic );
    EXPECT_EQ( true, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset();

    delete cip1;
    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, waterTankIsEmpty )
    {
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    virtual_device waterTankLowLevel( "LWL", device::DT_LS, device::DST_LS_VIRT );
    virtual_device waterTankCurrentLevel( "LTW", device::DT_LT, device::DST_LT_VIRT );
    cip1.LWL = static_cast<i_DI_device*>( &waterTankLowLevel );
    cip1.LTW = static_cast<i_AI_device*>( &waterTankCurrentLevel );

    cip1.dont_use_water_tank = 0;

    waterTankLowLevel.direct_set_state( 0 );
    waterTankCurrentLevel.set_value( 0 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 0;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 0;
    EXPECT_EQ( true, cip1.waterTankIsEmpty() );

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 0 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 0;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 0;
    EXPECT_EQ( false, cip1.waterTankIsEmpty() );
    cip1.dont_use_water_tank = 1;
    EXPECT_EQ( true, cip1.waterTankIsEmpty() );
    cip1.dont_use_water_tank = 0;

    waterTankLowLevel.direct_set_state( 0 );

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 9 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( true, cip1.waterTankIsEmpty() );

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 11 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( true, cip1.waterTankIsEmpty() );

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 12 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( false, cip1.waterTankIsEmpty() );

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 11 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( false, cip1.waterTankIsEmpty() );

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 10 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( false, cip1.waterTankIsEmpty() );

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 9.99f );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( true, cip1.waterTankIsEmpty() );

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 10 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( true, cip1.waterTankIsEmpty() );

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 11.99f );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( true, cip1.waterTankIsEmpty() );

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 12 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( false, cip1.waterTankIsEmpty() );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, OpolRR )
    {
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    cip1.T[ TMR_CHK_CONC ]->set_countdown_time( 0 );
    float currentConcentration;
    float cVal;
    float testVal;
    testVal = 0.12f;
    auto isCheckPoint = false;
    cipline_tech_object::set_station_par( P_CKANAL_K, testVal );
    for ( currentConcentration = 20; currentConcentration >= 0; )
        {
        reinterpret_cast<device*>( cip1.Q )->set_value( currentConcentration );
        cVal = cip1.GetConc( KISL );
        if ( cVal <= testVal )
            {
            if ( isCheckPoint )
                {
                EXPECT_EQ( 1, cip1.OpolRR( TANK_K ) );
                }
            else
                {
                EXPECT_EQ( 0, cip1.OpolRR( TANK_K ) );
                std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
                EXPECT_EQ( 1, cip1.OpolRR( TANK_K ) );
                isCheckPoint = true;
                }
            }
        else
            {
            EXPECT_EQ( 0, cip1.OpolRR( TANK_K ) );
            }
        currentConcentration -= 0.25;
        }

    testVal = 0.15f;
    isCheckPoint = false;
    cipline_tech_object::set_station_par( P_CKANAL_S, testVal );
    for ( currentConcentration = 20; currentConcentration >= 0; )
        {
        reinterpret_cast<device*>( cip1.Q )->set_value( currentConcentration );
        cVal = cip1.GetConc( SHCH );
        if ( cVal <= testVal )
            {
            if ( isCheckPoint )
                {
                EXPECT_EQ( 1, cip1.OpolRR( TANK_S ) );
                }
            else
                {
                EXPECT_EQ( 0, cip1.OpolRR( TANK_S ) );
                std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
                EXPECT_EQ( 1, cip1.OpolRR( TANK_S ) );
                isCheckPoint = true;
                }
            }
        else
            {
            EXPECT_EQ( 0, cip1.OpolRR( TANK_K ) );
            }
        currentConcentration -= 0.25;
        }

    ClearCipDevices();
    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, _FromObject )
    {
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    cip1.T[ TMR_CHK_CONC ]->set_countdown_time( 0 );
    cip1.FL->set_cmd( "ST", 0, 1 );
    float currentConcentration;
    float cVal;
    float testVal;
    testVal = 0.12f;
    cipline_tech_object::set_station_par( P_CKANAL_K, testVal );
    for ( currentConcentration = 20; currentConcentration >= 0; )
        {
        reinterpret_cast<device*>( cip1.Q )->set_value( currentConcentration );
        cVal = cip1.GetConc( KISL );
        cip1.concentration_ok = 0;
        cip1._FromObject( TANK_W, TANK_K );
        if ( cVal <= testVal )
            {
            EXPECT_EQ( 1, cip1.concentration_ok );
            }
        else
            {
            EXPECT_EQ( 0, cip1.concentration_ok );
            }
        cip1.concentration_ok = 0;
        cip1._FromObject( TANK_K, KANAL );
        if ( cVal >= testVal )
            {
            EXPECT_EQ( 1, cip1.concentration_ok );
            }
        else
            {
            EXPECT_EQ( 0, cip1.concentration_ok );
            }
        currentConcentration -= 0.25;
        }

    testVal = 0.15f;
    cipline_tech_object::set_station_par( P_CKANAL_S, testVal );
    for ( currentConcentration = 20; currentConcentration >= 0; )
        {
        reinterpret_cast<device*>( cip1.Q )->set_value( currentConcentration );
        cVal = cip1.GetConc( SHCH );
        cip1.concentration_ok = 0;
        cip1._FromObject( TANK_W, TANK_S );
        if ( cVal <= testVal )
            {
            EXPECT_EQ( 1, cip1.concentration_ok );
            }
        else
            {
            EXPECT_EQ( 0, cip1.concentration_ok );
            }
        cip1.concentration_ok = 0;
        cip1._FromObject( TANK_S, KANAL );
        if ( cVal >= testVal )
            {
            EXPECT_EQ( 1, cip1.concentration_ok );
            }
        else
            {
            EXPECT_EQ( 0, cip1.concentration_ok );
            }
        currentConcentration -= 0.25;
        }

    ClearCipDevices();
    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, _DoStep )
    {
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    virtual_device circ_signal( "LINE1DO101", device::DT_DO, device::DST_DO_VIRT );
    virtual_device can_continue_operation_signal( "LINE1DI101", device::DT_DI, device::DST_DI_VIRT );
    cip1.dev_upr_circulation = &circ_signal;
    cip1.dev_os_can_continue = &can_continue_operation_signal;

    //test circulation signal
    EXPECT_EQ( 0, cip1.dev_upr_circulation->get_state() );
    cip1.curstep = 8;
    cip1._DoStep( 8 );
    EXPECT_EQ( 1, cip1.dev_upr_circulation->get_state() );
    cip1.curstep = 7;
    cip1._DoStep( 7 );
    EXPECT_EQ( 0, cip1.dev_upr_circulation->get_state() );
    cip1.use_circulation_on_v2_supply = true;
    cip1.curstep = 7;
    cip1._DoStep( 7 );
    EXPECT_EQ( 1, cip1.dev_upr_circulation->get_state() );
    can_continue_operation_signal.set_state( 1 );
    cip1.curstep = 8;
    cip1._DoStep( 8 );
    EXPECT_EQ( 1, cip1.dev_upr_circulation->get_state() );

    ClearCipDevices();
    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, circulation_signal_with_can_continue )
    {
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    virtual_device circ_signal( "LINE1DO101", device::DT_DO, device::DST_DO_VIRT );
    virtual_device can_continue_operation_signal( "LINE1DI101", device::DT_DI, device::DST_DI_VIRT );
    cip1.dev_upr_circulation = &circ_signal;
    cip1.dev_os_can_continue = &can_continue_operation_signal;

    // Test for circulation step 28 (alkaline circulation)
    cip1.circ_temp_reached = 1; // Temperature reached
    cip1.curstep = 28;
    cip1._DoStep( 28 );
    EXPECT_EQ( 1, cip1.dev_upr_circulation->get_state() );

    // Simulate "can continue" signal during circulation
    can_continue_operation_signal.set_state( 1 );
    cip1.curstep = 28;
    cip1._DoStep( 28 );

    // Circulation signal should remain ON for circulation steps
    EXPECT_EQ( 1, cip1.dev_upr_circulation->get_state() );

    // Test for circulation step 48 (acid circulation)
    cip1.curstep = 48;
    cip1._DoStep( 48 );
    EXPECT_EQ( 1, cip1.dev_upr_circulation->get_state() );

    ClearCipDevices();
    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, _ToObject_rinse_with_can_continue_signal )
    {
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    virtual_device can_continue_signal( "LINE1DI101", device::DT_DI, device::DST_DI_VIRT );
    cip1.dev_os_can_continue = &can_continue_signal;

    // Test rinse step 8 - can continue OFF should return 0
    cip1.curstep = 8;
    can_continue_signal.set_state( 0 );
    int result = cip1._ToObject( 0, 0 );
    EXPECT_EQ( 0, result );

    // Test rinse step 8 - can continue ON should allow operation to continue
    can_continue_signal.set_state( 1 );
    // Need to set up volume to not trigger return 1
    cip1.rt_par_float[ P_VRAB ] = 1000000; // Large value so cnt doesn't trigger
    result = cip1._ToObject( 0, 0 );
    EXPECT_NE( 1, result ); // Should not return 1 from can_continue check

    // Test rinse step 37
    cip1.curstep = 37;
    can_continue_signal.set_state( 0 );
    result = cip1._ToObject( 0, 0 );
    EXPECT_EQ( 0, result );

    // Test rinse step 57
    cip1.curstep = 57;
    can_continue_signal.set_state( 0 );
    result = cip1._ToObject( 0, 0 );
    EXPECT_EQ( 0, result );

    // Test rinse step 86
    cip1.curstep = 86;
    can_continue_signal.set_state( 0 );
    result = cip1._ToObject( 0, 0 );
    EXPECT_EQ( 0, result );

    // Test non-rinse step - should not be affected by can_continue
    cip1.curstep = 10;
    can_continue_signal.set_state( 0 );
    cip1.rt_par_float[ P_VRAB ] = 1000000;
    result = cip1._ToObject( 0, 0 );
    EXPECT_NE( 1, result ); // Should not return early from can_continue check

    // Test with dev_os_can_continue = NULL
    cip1.dev_os_can_continue = nullptr;
    cip1.curstep = 8;
    cip1.rt_par_float[ P_VRAB ] = 1000000;
    result = cip1._ToObject( 0, 0 );
    EXPECT_NE( 1, result ); // Should continue normally without checking signal

    ClearCipDevices();
    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, _Circ_timer_behavior )
    {
    // This test verifies that _Circ function handles timer states correctly
    // The main logic change (removal of wasflip) is tested via _DoStep
    // and _ToObject tests.

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    virtual_device can_continue_signal( "LINE1DI101", device::DT_DI, device::DST_DI_VIRT );
    cip1.dev_os_can_continue = &can_continue_signal;
    virtual_device TR_device( "LINE1TE102", device::DT_TE, device::DST_TE_VIRT );
    cip1.TR = &TR_device;
    TR_device.set_value( 100.0f );
    virtual_counter cnt_device( "LINE1CNT101" );
    cip1.cnt = &cnt_device;
    cnt_device.set_cmd( "F", 0, 20.0f );

    cip1.T[ TMR_OP_TIME ]->set_countdown_time( 1 );
    cip1.T[ TMR_OP_TIME ]->start();
    sleep_ms( 2 );

    // Test with can_continue ON.
    can_continue_signal.set_state( 1 );
    int result = cip1._Circ( WATER );
    // Should return 1.
    EXPECT_TRUE( result );

    // Test with can_continue OFF.
    can_continue_signal.set_state( 0 );
    result = cip1._Circ( WATER );
    // Should return 0.
    EXPECT_FALSE( result );

    ClearCipDevices();
    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, save_device )
    {
    // Удаляем сохраненные файлы рецептов, чтобы получить значения по умолчанию.
    const auto LINE_DEF_F_NAME_0 = "line0rec.bin";
    remove( LINE_DEF_F_NAME_0 );
    const auto MEDIUM_DEF_F_NAME_0 = "medium0rec.bin";
    remove( MEDIUM_DEF_F_NAME_0 );
    const auto LINE_DEF_F_NAME_1 = "line1rec.bin";
    remove( LINE_DEF_F_NAME_1 );
    const auto MEDIUM_DEF_F_NAME_1 = "medium1rec.bin";
    remove( MEDIUM_DEF_F_NAME_1 );

    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    const int BUFF_SIZE = 5000;
    char buff[ BUFF_SIZE ] = { 0 };

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    // Сброс параметров рецепта к значениям по умолчанию.
    cip1.lineRecipes->ResetRecipeToDefaults( 0 );
    // Сброс рабочих параметров к значениям по умолчанию.
    cip1.lua_init_runtime_params();
    cip1.save_device( buff );

    std::string REF_STR0 = R"(t.CIP1 = t.CIP1 or {}
t.CIP1=
	{
	CMD=0,
	STATE=0,
	OPER=0,
	LOADED_REC='Не выбран',
	CUR_REC='',
	LASTRECNMR=0,
	LASTRECNAME='Не выбран',
	CUR_PRG='Не выбран',
	PRG_LIST='256##Наведение кислоты||512##Наведение щелочи||',
	REC_LIST='',
	NCAR='',
	NCAR1='',
	NCAR2='',
	NCAR3='',
	NCAR4='',
	SWITCH1=0,
	SWITCH2=0,
	SWITCH3=0,
	SWITCH4=0,
	IDLE_TIME = '00:00:00',
	S_PAR_F=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, )"
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, " R"(
		},
	S_PAR_UI=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, )"
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, " R"(
		},
	RT_PAR_F=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, )"
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 20, 20, "
        "20, 95, 2, 30, 0.20, 500, 130, 0, 30, 0, 30, 0, 15, 0.50, 10, 0.10, 1000, 40, 0, 2, 0, "
        "15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, " R"(
		},
	RT_PAR_UI=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, )"
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, " R"(
		},
	PAR_MAIN=
		{
		1.50, 0.30, 0.10, 1, 0.20, 0.10, 20, 5, 0, 8, 1.51, 1.80, 47, 57, 1.47, 1.45, 1, 52, )"
        "0.50, 30.80, 0.40, 30, 120, 50, 10000, 10000, 10000, 100, 100, 50, 0, 0, 12, 11, 20, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, " R"(
		},
	REC_PAR = 
	{
		0.00, 0.00, 200.00, 250.00, 1.00, 15.00, 0.00, 10.00, 30.00, 30.00, 30.00, 5.00, )"
        "80.00, 70.00, 95.00, 20.00, 1000.00, 750.00, 750.00, 500.00, 75.00, 30.00, 75.00, "
        "600.00, 600.00, 600.00, 600.00, 600.00, 600.00, 600.00, 2500.00, 0.00, 0.00, 0.00, "
        "0.00, 5113.00, 5.00, 600.00, 5.00, 600.00, 15.00, 8.00, 50.00, 5.00, 20.00, 25.00, "
        "30.00, 95.00, 1.50, 15.00, 0.20, 500.00, 130.00, 0.00, 15.00, 0.00, 30.00, 0.00, "
        "15.00, 0.50, 10.00, 0.10, 1000.00, 66.00, 0.00, 2.00, 0.00, 15.00, 0.00, 120.00, "
        "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 1.00, 0.00, 0.00, 0.00, 0.00, 0.00, "
        "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, "
        "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, "
        "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, " R"(
	},
	OBJSTATS_LASTWASH='Не известно',
	OBJSTATS_LASTPROGRAM='Не определено',
	OBJSTATS_CAUSTICCOUNT=0,
	OBJSTATS_LASTACIDWASH='Не известно',
	CAUSTIC_REC_LIST='',
	CAUSTIC_REC_NMR='0',
	CAUSTICNAME='',
	CAUSTIC_PAR_NAME='',
	CAUSTIC_PAR = 
	{
		0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, )"
        "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, " R"(
	},
	ACID_REC_LIST='',
	ACID_REC_NMR='0',
	ACIDNAME='',
	ACID_PAR_NAME='',
	ACID_PAR = 
	{
		0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, )"
        "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, " R"(
	}
	}
)";
    EXPECT_STREQ( REF_STR0.c_str(), buff );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, set_cmd )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    cip1.lineRecipes->ResetRecipeToDefaults( 0 );

    auto res = cip1.set_cmd( "CUR_REC", 0, "Test name" );
    EXPECT_EQ( res, 0 );
    res = cip1.set_cmd( "CUR_REC", 0, "" );
    EXPECT_EQ( res, 0 );

    res = cip1.set_cmd( "CAUSTIC_PAR_NAME", 0, "Test name" );
    EXPECT_EQ( res, 0 );
    res = cip1.set_cmd( "CAUSTIC_PAR_NAME", 0, "" );
    EXPECT_EQ( res, 0 );

    res = cip1.set_cmd( "ACID_PAR_NAME", 0, "Test name" );
    EXPECT_EQ( res, 0 );
    res = cip1.set_cmd( "ACID_PAR_NAME", 0, "" );
    EXPECT_EQ( res, 0 );

    res = cip1.set_cmd( "NCAR", 1, "NN 2200" );
    EXPECT_EQ( res, 0 );
    res = cip1.set_cmd( "NCAR", 1, "" );
    EXPECT_EQ( res, 0 );

    // Too-long name must be truncated, not ignored.
    const auto maxLen = TRecipeManager::recipeNameLength * UNICODE_MULTIPLIER;
    std::string tooLongName;
    for ( auto i = 0; i < maxLen + 10; i++ ) tooLongName += "Ж";
    res = cip1.set_cmd( "CUR_REC", 0, tooLongName.c_str() );
    EXPECT_EQ( res, 0 );
    EXPECT_GT( strnlen( cip1.lineRecipes->currentRecipeName, maxLen ), 0u );
    EXPECT_LT(
        strnlen( cip1.lineRecipes->currentRecipeName, maxLen ),
        static_cast<size_t>( maxLen ) );

    const auto medMaxLen =
        TMediumRecipeManager::recipeNameLength * UNICODE_MULTIPLIER;
    const std::string tooLongMedName( medMaxLen + 10, 'B' );
    res = cip1.set_cmd( "CAUSTIC_PAR_NAME", 0, tooLongMedName.c_str() );
    EXPECT_EQ( res, 0 );
    EXPECT_GT( strnlen( cip1.causticRecipes->currentRecipeName, medMaxLen ), 0u );
    EXPECT_LT(
        strnlen( cip1.causticRecipes->currentRecipeName, medMaxLen ),
        static_cast<size_t>( medMaxLen ) );

    res = cip1.set_cmd( "ACID_PAR_NAME", 0, tooLongMedName.c_str() );
    EXPECT_EQ( res, 0 );
    EXPECT_GT( strnlen( cip1.acidRecipes->currentRecipeName, medMaxLen ), 0u );
    EXPECT_LT(
        strnlen( cip1.acidRecipes->currentRecipeName, medMaxLen ),
        static_cast<size_t>( medMaxLen ) );

    const auto ncarMaxLen = CAR_NAME_MAX_LENGTH * UNICODE_MULTIPLIER;
    const std::string tooLongNcarName( ncarMaxLen + 10, 'C' );
    res = cip1.set_cmd( "NCAR", 1, tooLongNcarName.c_str() );
    EXPECT_EQ( res, 0 );
    EXPECT_GT( strnlen( cip1.ncar1, ncarMaxLen ), 0u );
    EXPECT_LT( strnlen( cip1.ncar1, ncarMaxLen ), static_cast<size_t>( ncarMaxLen ) );

    res = cip1.set_cmd( "NCAR", 2, tooLongNcarName.c_str() );
    EXPECT_EQ( res, 0 );
    EXPECT_GT( strnlen( cip1.ncar2, ncarMaxLen ), 0u );
    EXPECT_LT( strnlen( cip1.ncar2, ncarMaxLen ), static_cast<size_t>( ncarMaxLen ) );

    res = cip1.set_cmd( "NCAR", 3, tooLongNcarName.c_str() );
    EXPECT_EQ( res, 0 );
    EXPECT_GT( strnlen( cip1.ncar3, ncarMaxLen ), 0u );
    EXPECT_LT( strnlen( cip1.ncar3, ncarMaxLen ), static_cast<size_t>( ncarMaxLen ) );

    res = cip1.set_cmd( "NCAR", 4, tooLongNcarName.c_str() );
    EXPECT_EQ( res, 0 );
    EXPECT_GT( strnlen( cip1.ncar4, ncarMaxLen ), 0u );
    EXPECT_LT( strnlen( cip1.ncar4, ncarMaxLen ), static_cast<size_t>( ncarMaxLen ) );

    cip1.lineRecipes->SaveToFile( "tmp4.txt" );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, SCInitPumping )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1.SCInitPumping( -1, -1, -1, 0, 0, 1 );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, InitFilRR )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1.InitFilRR( TANK_S );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, InitCircRR )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1.InitCircRR( TANK_S );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, CircRR )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1.CircRR( TANK_S );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, timeIsOut )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1.timeIsOut();
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, InitCheckConc )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1.InitCheckConc( TANK_S );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, InitAddRR )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1.InitAddRR( TANK_S, 0, 0 );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, InitOpolRR )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1.InitOpolRR( TANK_S );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, _InitToObject )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1._InitToObject( WATER, TANK_W, 5, 0 );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, _InitFromObject )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1._InitFromObject( WATER, TANK_W, 5, 0 );
    EXPECT_EQ( res, 0 );

    res = cip1._InitFromObject( TANK_S, TANK_W, 5, 0 );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, _InitOporCIP )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1._InitOporCIP( TANK_W, 5, 0 );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, _InitFilCirc )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1._InitFilCirc( WITH_WATER, 5, 0 );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, _InitOporCirc )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1._InitOporCirc( TANK_W, 5, 0 );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, _InitCirc )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1._InitCirc( WATER, 5, 0 );
    EXPECT_EQ( res, 0 );

    res = cip1._InitCirc( SANITIZER, 5, 0 );
    EXPECT_EQ( res, 0 );

    res = cip1._InitCirc( HOT_WATER, 5, 0 );
    EXPECT_EQ( res, 0 );

    res = cip1._InitCirc( SHCH, 5, 0 );
    EXPECT_EQ( res, 0 );

    res = cip1._InitCirc( KISL, 5, 0 );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, _InitDoseRR )
    {
    auto L = luaL_newstate();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    InitStationParams();

    auto res = cip1._InitDoseRR( SANITIZER, 5, 0 );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

class cipline_tech_object_test : public ::testing::Test
    {
    protected:
        void SetUp() override
            {
            lua_manager::get_instance()->set_Lua( luaL_newstate() );
            InitCipDevices();
            InitStationParams();
            }

        void TearDown() override
            {
            ClearCipDevices();
            G_LUA_MANAGER->free_Lua();
            }

        class cipline_tech_object_mock : public cipline_tech_object
            {
            public:
                cipline_tech_object_mock()
                    : cipline_tech_object( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
                        "CIP1", 1, 1, 200, 200, 200, 200 )
                    {}

                int DoStep( int step_to_do ) override
                    {
                    return -1; // mock implementation
                    }
            };

        cipline_tech_object_mock cip1{};
    };

TEST_F( cipline_tech_object_test, EvalCipInProgress )
    {
    cip1.initline();

    auto res = cip1.EvalCipInProgress();
    EXPECT_EQ( res, -1 );
    }

// =========================== WATCHDOG TESTS ===========================

TEST( cipline_tech_object, check_device_watchdog )
    {
    // Test the check_device() helper function for watchdog device validation
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );

    device* outdev;

    // Test with bad device type - should not work
    int result = cip1.check_device( outdev, P_WATCHDOG, device::C_DEVICE_TYPE_CNT );
    EXPECT_EQ( -1, result ); // No device configured

    // Test with zero device number (no device) - should work without initialization
    cip1.rt_par_float[ P_WATCHDOG ] = 0;
    result = cip1.check_device( outdev, P_WATCHDOG, device::DT_WATCHDOG );
    EXPECT_EQ( 0, result ); // No device configured

    // Test with negative parameter - should work without initialization
    cip1.rt_par_float[ P_WATCHDOG ] = -1;
    result = cip1.check_device( outdev, P_WATCHDOG, device::DT_WATCHDOG );
    EXPECT_EQ( 0, result ); // Invalid parameter

    // Now test with valid device - this might be the problematic part
    cip1.rt_par_float[ P_WATCHDOG ] = 1;
    result = cip1.check_device( outdev, P_WATCHDOG, device::DT_WATCHDOG );
    EXPECT_EQ( 0, result ); // Should find device

    // Test with non-existent device number
    cip1.rt_par_float[ P_WATCHDOG ] = 99;
    result = cip1.check_device( outdev, P_WATCHDOG, device::DT_WATCHDOG );
    EXPECT_EQ( -2, result ); // Should not find device

    ClearCipDevices();
    }

TEST( cipline_tech_object, _CheckErr_watchdog )
    {
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    // Test ERR_WATCHDOG error detection when watchdog device is inactive
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );

    cip1.initline();
    cip1.init_object_devices();

    auto res = cip1._CheckErr();
    EXPECT_EQ( res, 0 );

    cip1.rt_par_float[ P_WATCHDOG ] = 1;
    cip1.init_object_devices();
    res = cip1._CheckErr();
    EXPECT_EQ( res, ERR_WATCHDOG );

    ClearCipDevices();
    lua_manager::get_instance()->free_Lua();
    }

TEST( cipline_tech_object, _ResetLinesDevicesBeforeReset_watchdog )
    {
    // Test proper watchdog device reset in _ResetLinesDevicesBeforeReset()
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );

    // Set up watchdog device manually
    auto dm = device_manager::get_instance();
    auto watchdog_dev = dm->get_device("LINE1WATCHDOG1");
    ASSERT_NE(nullptr, watchdog_dev);
    cip1.dev_watchdog = watchdog_dev;

    // Set watchdog to active state
    watchdog_dev->set_state(1);
    EXPECT_EQ( 1, watchdog_dev->get_state() );

    // Call reset function
    cip1._ResetLinesDevicesBeforeReset();

    // Verify dev_watchdog pointer was nullified
    EXPECT_EQ( nullptr, cip1.dev_watchdog );

    ClearCipDevices();
    }

TEST( cipline_tech_object, watchdog_error_message )
    {
    // Test ERR_WATCHDOG constant value and Russian error message
    EXPECT_EQ( -42, ERR_WATCHDOG );

    // Test error message exists in map
    auto it = ERR_MSG.find( ERR_WATCHDOG );
    ASSERT_NE( ERR_MSG.end(), it );

    // Test error message content (Russian)
    std::string expected_msg = "Ошибка сторожевого таймера";
    EXPECT_EQ( expected_msg, std::string( it->second ) );
    }

TEST( cipline_tech_object, recipe_RV_WATCHDOG_mapping_and_device_init )
    {
    // 1) Окружение.
    InitCipDevices(); // Добавляет LINE1WATCHDOG1 и прочие виртуальные устройства
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );

    cip1.initline();
    // До загрузки рецепта параметр P_WATCHDOG должен быть 0.
    EXPECT_EQ( 0.0f, cip1.rt_par_float[ P_WATCHDOG ] );

    // 2) В рецепте текущей линии выставляем RV_WATCHDOG = 1 (т.е. LINE1WATCHDOG1).
    auto* rm = cip1.lineRecipes;
    const auto curRec = rm->getCurrentRecipe();
    // Гарантируем базовые значения рецепта и выставляем watchdog.
    rm->ResetRecipeToDefaults( curRec );
    ASSERT_NE( 0, rm->GetParamsCount() ); // sanity check: рецепт доступен
    ASSERT_EQ( 0, rm->setRecipeValue( curRec, TRecipeManager::RV_WATCHDOG, 1.0f ) );

    // 3) Загружаем рецепт в рантайм‑параметры CIP.
    cip1.rt_par_float[ P_SELECT_REC ] =
        static_cast<float>( curRec + 1 ); // Нумерация рецептов в UI — с 1.
    EXPECT_EQ( 0, cip1.evaluate() );

    // Проверяем, что mapping сработал: RV_WATCHDOG -> P_WATCHDOG.
    EXPECT_EQ( 1.0f, cip1.rt_par_float[ P_WATCHDOG ] );

    // 4) Инициализируем устройства объекта, dev_watchdog должен подтянуться.
    EXPECT_EQ( 0, cip1.init_object_devices() );
    ASSERT_NE( nullptr, cip1.dev_watchdog );

    // 5) Пока watchdog «молчит», CIP должен вернуть ERR_WATCHDOG.
    EXPECT_EQ( ERR_WATCHDOG, cip1._CheckErr() );

    // 6) Дополнительно проверим, что можно «оживить» watchdog и ошибка уйдёт.
    auto* dm = device_manager::get_instance();
    auto* wd = dm->get_device( "LINE1WATCHDOG1" );
    ASSERT_NE( nullptr, wd );
    // Привяжем вход к дискретному датчику и сделаем два «толчка».
    wd->set_string_property( "DI_dev", "LINE1LS1" );
    auto* ls1 = dm->get_device( "LINE1LS1" );
    ASSERT_NE( nullptr, ls1 );

    ls1->set_state( 1 ); wd->evaluate_io();
    EXPECT_EQ( 0, cip1._CheckErr() ); // ошибка пропала

    ls1->set_state( 0 ); wd->evaluate_io();
    EXPECT_EQ( 0, cip1._CheckErr() ); // всё ещё ок

    // Завершение.
    G_LUA_MANAGER->free_Lua();
    ClearCipDevices();
    }

TEST( cipline_tech_object, check_device_watchdog_fallback_no_line_prefix )
    {
    // When LINE1WATCHDOGn is absent but WATCHDOGn exists, check_device must
    // fall back to the device without a line prefix.
    InitCipDevices(); // Does not add WATCHDOG101.
    auto dm = device_manager::get_instance();
    dm->add_io_device( device::DEVICE_TYPE::DT_WATCHDOG,
        device::DEVICE_SUB_TYPE::DST_WATCHDOG, "WATCHDOG101", "", "" );

    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    cip1.rt_par_float[ P_WATCHDOG ] = 101;

    device* outdev = nullptr;
    int result = cip1.check_device( outdev, P_WATCHDOG,
        device::DEVICE_TYPE::DT_WATCHDOG );

    EXPECT_EQ( 0, result );
    ASSERT_NE( nullptr, outdev );
    EXPECT_STREQ( "WATCHDOG101", outdev->get_name() );

    ClearCipDevices();
    }

TEST( cipline_tech_object, check_device_watchdog_line_prefix_takes_priority )
    {
    // When both LINE1WATCHDOGn and WATCHDOGn exist, LINE1WATCHDOGn must be
    // preferred over WATCHDOGn.
    InitCipDevices(); // Adds LINE1WATCHDOG1.
    auto dm = device_manager::get_instance();
    dm->add_io_device( device::DEVICE_TYPE::DT_WATCHDOG,
        device::DEVICE_SUB_TYPE::DST_WATCHDOG, "WATCHDOG1", "", "" );

    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    cip1.rt_par_float[ P_WATCHDOG ] = 1;

    device* outdev = nullptr;
    int result = cip1.check_device( outdev, P_WATCHDOG,
        device::DEVICE_TYPE::DT_WATCHDOG );

    EXPECT_EQ( 0, result );
    ASSERT_NE( nullptr, outdev );
    EXPECT_STREQ( "LINE1WATCHDOG1", outdev->get_name() );

    ClearCipDevices();
    }

TEST( cipline_tech_object, no_cool_after_desinfection_GoToStep91From67 )
    {
    // Окружение.
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );
    cip1.initline();

    // P_PROGRAM = SPROG_HOTWATER.
    cip1.rt_par_float[ P_PROGRAM ] = SPROG_HOTWATER;

    // По умолчанию параметр = 0 (нет дополнительного объёма).
    EXPECT_EQ( 0.0f, cip1.rt_par_float[ P_DOP_V_OK_OP ] );

    // Устанавливаем параметр объёма в -1 (не охлаждать после дезинфекции).
    cip1.rt_par_float[ P_DOP_V_OK_OP ] = -1;
    auto res = cip1.LoadProgram();
    EXPECT_EQ( res, 61 );
    // После шага 67 при отрицательном параметре переходим на 91.
    EXPECT_EQ( 91, cip1._GoToStep( 67, 0 ) );

    // После шага 91 переходим на 555.
    EXPECT_EQ( 555, cip1._GoToStep( 91, 0 ) );
    cip1.ResetLinesDevicesBeforeReset();

    // Устанавливаем параметр в 100 - проверяем стандартное поведение.
    cip1.rt_par_float[ P_PROGRAM ] = SPROG_HOTWATER;
    cip1.rt_par_float[ P_DOP_V_OK_OP ] = 100;
    res = cip1.LoadProgram();
    EXPECT_EQ( res, 61 );
    EXPECT_EQ( 81, cip1._GoToStep( 67, 0 ) );

    // Завершение.
    G_LUA_MANAGER->free_Lua();
    ClearCipDevices();
    }

TEST( cipline_tech_object, ResetWP )
    {
    // Окружение.
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( luaL_newstate() );
    cip1.initline();

    cip1.rt_par_float[ P_ZAD_PODOGR ] = 1.0f;
    cip1.rt_par_float[ P_CONC_RATE ] = 10.0f;
    cip1.rt_par_float[ P_TM_R_NO_FLOW ] = 100.0f;
    cip1.ResetWP();
    // После сброса параметров (в 0) некоторые должны сохранить своё исходное
    // значение
    EXPECT_EQ( cip1.rt_par_float[ P_ZAD_PODOGR ], 0.0f );
    EXPECT_EQ( cip1.rt_par_float[ P_CONC_RATE ], 10.0f );
    EXPECT_EQ( cip1.rt_par_float[ P_TM_R_NO_FLOW ], 100.0f );


    // Завершение.
    G_LUA_MANAGER->free_Lua();
    ClearCipDevices();
    }
