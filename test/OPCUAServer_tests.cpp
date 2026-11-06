#include "OPCUAServer_tests.h"
#include "g_errors.h"
#include <chrono>

using namespace ::testing;

class OPCUA_server_test : public Test
    {
    protected:
        void TearDown() override
            {
            G_OPCUA_SERVER.shutdown();
            }
    };

TEST_F( OPCUA_server_test, evaluate_non_blocking )
    {
    auto res = G_OPCUA_SERVER.init_all_and_start();
    ASSERT_EQ( UA_STATUSCODE_GOOD, res );

    constexpr auto ITERATIONS = 20;
    const auto start = std::chrono::steady_clock::now();
    for ( auto i = 0; i < ITERATIONS; ++i )
        {
        G_OPCUA_SERVER.evaluate();
        }
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start );

    EXPECT_LT( elapsed.count(), 10 );
    }

TEST( OPCUA_server, evaluate )
    {
    G_OPCUA_SERVER.evaluate();  //Correct evaluate() with no initialization.

    G_OPCUA_SERVER.init();
    G_OPCUA_SERVER.init();      //Correct init() even call again.

    G_DEVICE_MANAGER()->add_io_device( device::DT_V, device::DST_V_DO1,
        "Valve1", "Test Valve", "" );
    auto valve1 = G_DEVICE_MANAGER()->get_V( "Valve1" );
    valve1->on();

    G_OPCUA_SERVER.create_dev_objects();
    G_OPCUA_SERVER.create_PAC_info();

    auto res = G_OPCUA_SERVER.start();
    EXPECT_EQ( UA_STATUSCODE_GOOD, res  );
    res = G_OPCUA_SERVER.start();
    // Bad start() when call again.
    EXPECT_EQ( UA_STATUSCODE_BADINTERNALERROR, res );

    G_OPCUA_SERVER.evaluate();

    res = G_OPCUA_SERVER.read_state( nullptr, nullptr, nullptr,
        nullptr, nullptr, false, nullptr, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BAD, res );

    res = G_OPCUA_SERVER.write_state( nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BAD, res );
    UA_DataValue v;
    res = G_OPCUA_SERVER.write_state( nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, &v );
    EXPECT_EQ( UA_STATUSCODE_BAD, res );

    res = G_OPCUA_SERVER.read_value( nullptr, nullptr, nullptr,
        nullptr, nullptr, false, nullptr, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BAD, res );

    res = G_OPCUA_SERVER.write_value( nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BAD, res );
    res = G_OPCUA_SERVER.write_value( nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, &v );
    EXPECT_EQ( UA_STATUSCODE_BAD, res );

    UA_BrowseDescription bd;
    UA_BrowseDescription_init( &bd );
    bd.resultMask = UA_BROWSERESULTMASK_ALL;
    bd.nodeClassMask = UA_BROWSERESULTMASK_ALL;
    bd.includeSubtypes = true;
    bd.nodeId = UA_NODEID_NUMERIC( 0, UA_NS0ID_OBJECTSFOLDER );
    bd.referenceTypeId = UA_NODEID_NUMERIC( 0, UA_NS0ID_ORGANIZES );

    bd.browseDirection = UA_BROWSEDIRECTION_FORWARD;
    bd.includeSubtypes = true;

    auto UA_server = G_OPCUA_SERVER.get_server();
    UA_BrowseResult br = UA_Server_browse( UA_server, 0, &bd );
    EXPECT_EQ( UA_STATUSCODE_GOOD, br.statusCode );

    bool is_exist_node = false;
    auto total = br.referencesSize;
    for ( auto i = 0u; i < total; ++i )
        {
        auto name = br.references[ i ].browseName.name;
        if ( std::string( reinterpret_cast<char*>( name.data ),
            name.length ) == "devices" )
            {
            bd.nodeId = br.references[ i ].nodeId.nodeId;
            UA_BrowseResult br1 = UA_Server_browse( UA_server, 0, &bd );
            EXPECT_EQ( UA_STATUSCODE_GOOD, br.statusCode );

            auto devices_count = br1.referencesSize;
            for ( auto l = 0u; l < devices_count; ++l )
                {
                name = br1.references[ l ].browseName.name;
                if ( std::string( reinterpret_cast<char*>( name.data ),
                    name.length ) == "Valve1" )
                    {
                    is_exist_node = true;
                    }
                }
            UA_BrowseResult_clear( &br1 );
            }
        }
    EXPECT_TRUE( is_exist_node );
    UA_BrowseDescription_clear( &bd );
    UA_BrowseResult_clear( &br );

    UA_Variant out;
    UA_Variant_init( &out );
    UA_NodeId valve1_state_NodeId = UA_NODEID_STRING_ALLOC( 0, "Valve1.state" );
    res = UA_Server_readValue( UA_server, valve1_state_NodeId, &out );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    EXPECT_TRUE( out.type == &UA_TYPES[ UA_TYPES_INT32 ] );
    auto state = static_cast<UA_Int32*>( out.data );
    EXPECT_EQ( 1, *state );
    UA_Variant_clear( &out );


    UA_NodeId valve1_value_NodeId = UA_NODEID_STRING_ALLOC( 0, "Valve1.value" );
    res = UA_Server_readValue( UA_server, valve1_value_NodeId, &out );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    EXPECT_TRUE( out.type == &UA_TYPES[ UA_TYPES_FLOAT ] );
    auto value = static_cast<UA_Float*>( out.data );
    EXPECT_EQ( .0f, *value );
    UA_Variant_clear( &out );


    UA_Variant val;
    UA_Variant_init( &val );
    UA_Int32 new_state = 10;
    UA_Variant_setScalar( &val, &new_state, &UA_TYPES[ UA_TYPES_INT32 ] );
    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] = 0;
    res = UA_Server_writeValue( UA_server, valve1_state_NodeId, val );
    EXPECT_EQ( UA_STATUSCODE_BADUSERACCESSDENIED, res );
    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] = 1;
    res = UA_Server_writeValue( UA_server, valve1_state_NodeId, val );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    res = UA_Server_readValue( UA_server, valve1_state_NodeId, &out );
    UA_NodeId_clear( &valve1_state_NodeId );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    state = static_cast<UA_Int32*>( out.data );
    EXPECT_EQ( 10, *state );
    UA_Variant_clear( &out );
    // Вызывать UA_Variant_clear( &val ); не надо, так как значение храниться в
    // локальной переменной new_state.


    UA_Float new_value = 10.f;
    UA_Variant_setScalar( &val, &new_value, &UA_TYPES[ UA_TYPES_FLOAT ] );
    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] = 0;
    res = UA_Server_writeValue( UA_server, valve1_value_NodeId, val );
    EXPECT_EQ( UA_STATUSCODE_BADUSERACCESSDENIED, res );
    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] = 1;
    res = UA_Server_writeValue( UA_server, valve1_value_NodeId, val );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    res = UA_Server_readValue( UA_server, valve1_value_NodeId, &out );
    UA_NodeId_clear( &valve1_value_NodeId );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    value = static_cast<UA_Float*>( out.data );
    EXPECT_EQ( 10.f, *value );
    UA_Variant_clear( &out );


    UA_NodeId uptime_NodeId = UA_NODEID_STRING_ALLOC( 0, "PAC_info.uptime" );
    res = UA_Server_readValue( UA_server, uptime_NodeId, &out );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    auto str = static_cast<UA_String*>( out.data );
    EXPECT_EQ( "0 дн. 0:0:0", std::string( reinterpret_cast<char*>( str->data ),
        str->length ) );
    UA_Variant_clear( &out );
    UA_NodeId_clear( &uptime_NodeId );

    UA_NodeId version_NodeId = UA_NODEID_STRING_ALLOC( 0, "PAC_info.version" );
    res = UA_Server_readValue( UA_server, version_NodeId, &out );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    str = static_cast<UA_String*>( out.data );
    EXPECT_EQ( PRODUCT_VERSION_FULL_STR,
        std::string( reinterpret_cast<char*>( str->data ), str->length ) );
    UA_Variant_clear( &out );
    UA_NodeId_clear( &version_NodeId );

    G_OPCUA_SERVER.shutdown();

    res = G_OPCUA_SERVER.init_all_and_start();
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    G_OPCUA_SERVER.shutdown();

    G_DEVICE_MANAGER()->clear_io_devices();
    }

TEST( OPCUA_server, add_device_methods_nullptr )
    {
    UA_NodeId deviceId{};
    G_OPCUA_SERVER.add_device_methods( deviceId, nullptr );
    }

TEST( OPCUA_server, methods_callbacks )
    {
    const auto PREV_P_V_OFF_DELAY_TIME_VALUE = G_PAC_INFO()->par[
        PAC_info::PARAMETERS::P_V_OFF_DELAY_TIME ];
    const auto PREV_P_IS_OPC_UA_SERVER_CONTROL_VALUE =
        G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ];

    G_DEVICE_MANAGER()->add_io_device( device::DT_V, device::DST_V_DO1,
        "ValveM", "Test Valve", "" );
    auto dev = G_DEVICE_MANAGER()->get_device( "ValveM" );
    ASSERT_NE( nullptr, dev );

    auto res = UA_STATUSCODE_BAD;

    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] = 0;
    res = OPCUA_server::method_on( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 0, nullptr, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADUSERACCESSDENIED, res );

    res = OPCUA_server::method_off( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 0, nullptr, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADUSERACCESSDENIED, res );

    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] = 1;
    res = OPCUA_server::method_on( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 0, nullptr, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    EXPECT_EQ( 1, dev->get_state() );

    res = OPCUA_server::method_off( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 0, nullptr, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    // Вызываем evaluate() для обработки задержки выключения клапана.
    G_PAC_INFO()->par[ PAC_info::PARAMETERS::P_V_OFF_DELAY_TIME ] = 0;
    for ( auto i = 0; i < 10 && dev->get_state() != 0; ++i )
        {
        sleep_ms( 1 );
        valve::evaluate();
        }
    EXPECT_EQ( 0, dev->get_state() );

    res = OPCUA_server::method_on( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 1, nullptr, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADINVALIDARGUMENT, res );

    res = OPCUA_server::method_off( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 1, nullptr, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADINVALIDARGUMENT, res );

    res = OPCUA_server::method_on( nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, 0, nullptr, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADINTERNALERROR, res );

    res = OPCUA_server::method_off( nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, 0, nullptr, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADINTERNALERROR, res );

    UA_Variant in;
    UA_Variant_init( &in );

    res = OPCUA_server::method_set_state( nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, 0, &in, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADINTERNALERROR, res );

    res = OPCUA_server::method_set_state( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 0, &in, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADINVALIDARGUMENT, res );

    UA_Float wrong_state = 1.0f;
    UA_Variant_setScalar( &in, &wrong_state, &UA_TYPES[ UA_TYPES_FLOAT ] );
    res = OPCUA_server::method_set_state( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 1, &in, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADINVALIDARGUMENT, res );

    UA_Int32 new_state = 5;
    UA_Variant_setScalar( &in, &new_state, &UA_TYPES[ UA_TYPES_INT32 ] );
    res = OPCUA_server::method_set_state( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 1, &in, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    EXPECT_EQ( 5, dev->get_state() );

    res = OPCUA_server::method_set_value( nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, 0, &in, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADINTERNALERROR, res );

    res = OPCUA_server::method_set_value( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 0, &in, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADINVALIDARGUMENT, res );

    UA_Int32 wrong_value = 1;
    UA_Variant_setScalar( &in, &wrong_value, &UA_TYPES[ UA_TYPES_INT32 ] );
    res = OPCUA_server::method_set_value( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 1, &in, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADINVALIDARGUMENT, res );

    UA_Float new_value = 55.f;
    UA_Variant_setScalar( &in, &new_value, &UA_TYPES[ UA_TYPES_FLOAT ] );
    res = OPCUA_server::method_set_value( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 1, &in, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    EXPECT_EQ( 55.f, dev->get_value() );

    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] = 0;

    UA_Int32 denied_state = 7;
    UA_Variant_setScalar( &in, &denied_state, &UA_TYPES[ UA_TYPES_INT32 ] );
    res = OPCUA_server::method_set_state( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 1, &in, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADUSERACCESSDENIED, res );

    UA_Float denied_value = 77.f;
    UA_Variant_setScalar( &in, &denied_value, &UA_TYPES[ UA_TYPES_FLOAT ] );
    res = OPCUA_server::method_set_value( nullptr, nullptr, nullptr,
        nullptr, dev, nullptr, nullptr, 1, &in, 0, nullptr );
    EXPECT_EQ( UA_STATUSCODE_BADUSERACCESSDENIED, res );


    G_PAC_INFO()->par[ PAC_info::PARAMETERS::P_V_OFF_DELAY_TIME ] =
        PREV_P_V_OFF_DELAY_TIME_VALUE;
    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] =
        PREV_P_IS_OPC_UA_SERVER_CONTROL_VALUE;
    G_DEVICE_MANAGER()->clear_io_devices();
    }
