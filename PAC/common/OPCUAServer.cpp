#include <OPCUAServer.h>
#include "log.h"
#include <stdio.h>

#include "prj_mngr.h"
#include "device/device.h"
#include "device/manager.h"
#include "PAC_info.h"
#include "tech_def.h"
#include "lua_manager.h"
#include "PAC_err.h"

OPCUA_server& OPCUA_server::get_instance()
    {
    static OPCUA_server instance;
    return instance;
    }

void OPCUA_server::init( short int port )
    {
    if ( nullptr == server )
        {
        server = UA_Server_new();
        UA_Int16 portNumber = port;
        UA_ServerConfig_setMinimal( UA_Server_getConfig( server ),
            portNumber, nullptr );
        }
    }

void OPCUA_server::create_dev_objects()
    {
    if ( !server ) return;
    if ( is_dev_objects_created ) return;

    UA_NodeId dev_root;
    //Create root object node.
    auto oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", "devices" );
    oAttr.description = UA_LOCALIZEDTEXT_ALLOC( "ru-RU", "devices" );
    auto qn = UA_QUALIFIEDNAME_ALLOC( 1, "devices" );
    auto addObjectNodeStatus = UA_Server_addObjectNode( server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_OBJECTSFOLDER ),
        UA_NODEID_NUMERIC( 0, UA_NS0ID_ORGANIZES ),
        qn,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEOBJECTTYPE ),
        oAttr, nullptr, &dev_root );
    if ( addObjectNodeStatus != UA_STATUSCODE_GOOD )
        {
        G_LOG->error( "OPCUA_server::UA_Server_addObjectNode "
            "failed create root object node: `%s` (0x%08x)",
            UA_StatusCode_name( addObjectNodeStatus ),
            static_cast<unsigned int>( addObjectNodeStatus ) );
        return;
        }

    UA_ObjectAttributes_clear( &oAttr );
    UA_QualifiedName_clear( &qn );

    auto deviceCount = static_cast<u_int>(
        G_DEVICE_MANAGER()->get_device_count() );
    for ( u_int i = 0; i < deviceCount; i++ )
        {
        UA_NodeId deviceId;
        auto dev = G_DEVICE_MANAGER()->get_device( i );

        //Create object node.
        oAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US",
            dev->get_name() );
        oAttr.description = UA_LOCALIZEDTEXT_ALLOC( "ru-RU",
            dev->get_description() );
        qn = UA_QUALIFIEDNAME_ALLOC( 1, dev->get_name() );
        addObjectNodeStatus= UA_Server_addObjectNode( server, UA_NODEID_NULL,
            dev_root,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_ORGANIZES ),
            qn,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEOBJECTTYPE ),
            oAttr, nullptr, &deviceId );
        if ( addObjectNodeStatus != UA_STATUSCODE_GOOD )
            {
            G_LOG->error( "OPCUA_server::UA_Server_addObjectNode "
                "failed add device node (`%s`): `%s` (0x%08x)",
                dev->get_name(),
                UA_StatusCode_name( addObjectNodeStatus ),
                static_cast<unsigned int>( addObjectNodeStatus ) );
            continue;
            }
        UA_ObjectAttributes_clear( &oAttr );
        UA_QualifiedName_clear( &qn );

        //Creating value variable node.
        auto valueAttr = UA_VariableAttributes_default;
        valueAttr.accessLevel = UA_ACCESSLEVELMASK_READ |
            UA_ACCESSLEVELMASK_WRITE;
        UA_Float value = 0;
        UA_Variant_setScalarCopy( &valueAttr.value, &value,
            &UA_TYPES[ UA_TYPES_FLOAT ] );

        const std::string VALUE = "value";
        std::string node_name = dev->get_name();
        node_name += "." + VALUE;

        valueAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US",
            VALUE.c_str() );
        valueAttr.dataType = UA_TYPES[ UA_TYPES_FLOAT ].typeId;
        auto valueNodeId = UA_NODEID_STRING_ALLOC( 0, node_name.c_str() );

        qn = UA_QUALIFIEDNAME_ALLOC( 1, VALUE.c_str() );
        auto addVariableNodeStatus = UA_Server_addVariableNode( server,
            valueNodeId, deviceId,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
            qn,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEDATAVARIABLETYPE ),
            valueAttr, dev, nullptr );
        if ( addVariableNodeStatus != UA_STATUSCODE_GOOD )
            {
            G_LOG->debug( "OPCUA_server::UA_Server_addVariableNode "
                "failed: `%s` (0x%08x)",
                UA_StatusCode_name( addVariableNodeStatus ),
                static_cast<unsigned int>( addVariableNodeStatus ) );
            }
        UA_VariableAttributes_clear( &valueAttr );
        UA_QualifiedName_clear( &qn );


        //Creating value variable read/write callbacks.
        UA_DataSource valueDataSource;
        valueDataSource.read = read_value;
        valueDataSource.write = write_value;
        UA_Server_setVariableNode_dataSource( server, valueNodeId,
            valueDataSource );
        UA_NodeId_clear( &valueNodeId );

        //Creating state variable node.
        auto stateAttr = UA_VariableAttributes_default;
        stateAttr.accessLevel = UA_ACCESSLEVELMASK_READ |
            UA_ACCESSLEVELMASK_WRITE;
        UA_Int32 state = 0;
        UA_Variant_setScalarCopy( &stateAttr.value, &state,
            &UA_TYPES[ UA_TYPES_INT32 ] );

        const std::string STATE = "state";
        node_name = dev->get_name();
        node_name += "." + STATE;

        stateAttr.displayName = UA_LOCALIZEDTEXT_ALLOC(
            "en-US", STATE.c_str() );
        stateAttr.dataType = UA_TYPES[ UA_TYPES_INT32 ].typeId;
        auto stateNodeId = UA_NODEID_STRING_ALLOC( 0, node_name.c_str() );

        qn = UA_QUALIFIEDNAME_ALLOC( 1, STATE.c_str() );
        addVariableNodeStatus = UA_Server_addVariableNode( server,
            stateNodeId, deviceId,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
            qn,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEDATAVARIABLETYPE ),
            stateAttr, dev, nullptr );
        if ( addVariableNodeStatus != UA_STATUSCODE_GOOD )
            {
            G_LOG->debug( "OPCUA_server::UA_Server_addVariableNode "
                "failed: `%s` (0x%08x)",
                UA_StatusCode_name( addVariableNodeStatus ),
                static_cast<unsigned int>( addVariableNodeStatus ) );
            }
        UA_VariableAttributes_clear( &stateAttr );
        UA_QualifiedName_clear( &qn );

        //Creating state variable read/write callbacks.
        UA_DataSource stateDataSource;
        stateDataSource.read = read_state;
        stateDataSource.write = write_state;
        UA_Server_setVariableNode_dataSource( server, stateNodeId,
            stateDataSource );
        UA_NodeId_clear( &stateNodeId );

        add_device_methods( deviceId, dev );
        }

    is_dev_objects_created = true;
    }

void OPCUA_server::add_device_methods( const UA_NodeId& deviceId, device* dev )
    {
    if ( !server || !dev )
        {
        return;
        }

    auto methodAttr = UA_MethodAttributes_default;
    methodAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", "set_state" );
    methodAttr.description = UA_LOCALIZEDTEXT_ALLOC( "ru-RU",
        "Установить новое состояние." );
    methodAttr.executable = true;
    methodAttr.userExecutable = true;

    UA_Argument inputState;
    UA_Argument_init( &inputState );
    inputState.description = UA_LOCALIZEDTEXT_ALLOC( "ru-RU",
        "Новое состояние." );
    inputState.name = UA_STRING_ALLOC( "state" );
    inputState.dataType = UA_TYPES[ UA_TYPES_INT32 ].typeId;
    inputState.valueRank = UA_VALUERANK_SCALAR;

    auto qn = UA_QUALIFIEDNAME_ALLOC( 1, "set_state" );
    auto addMethodStatus = UA_Server_addMethodNode( server,
        UA_NODEID_NULL, deviceId,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
        qn, methodAttr, method_set_state,
        1, &inputState, 0, nullptr, dev, nullptr );
    if ( addMethodStatus != UA_STATUSCODE_GOOD )
        {
        G_LOG->debug( "OPCUA_server::UA_Server_addMethodNode "
            "failed for set_state: `%s` (0x%08x)",
            UA_StatusCode_name( addMethodStatus ),
            static_cast<unsigned int>( addMethodStatus ) );
        }

    UA_MethodAttributes_clear( &methodAttr );
    UA_Argument_clear( &inputState );
    UA_QualifiedName_clear( &qn );

    methodAttr = UA_MethodAttributes_default;
    methodAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", "set_value" );
    methodAttr.description = UA_LOCALIZEDTEXT_ALLOC( "ru-RU",
        "Установить новое значение." );
    methodAttr.executable = true;
    methodAttr.userExecutable = true;

    UA_Argument inputValue;
    UA_Argument_init( &inputValue );
    inputValue.description = UA_LOCALIZEDTEXT_ALLOC( "ru-RU",
        "Новое значение." );
    inputValue.name = UA_STRING_ALLOC( "value" );
    inputValue.dataType = UA_TYPES[ UA_TYPES_FLOAT ].typeId;
    inputValue.valueRank = UA_VALUERANK_SCALAR;

    qn = UA_QUALIFIEDNAME_ALLOC( 1, "set_value" );
    addMethodStatus = UA_Server_addMethodNode( server,
        UA_NODEID_NULL, deviceId,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
        qn, methodAttr, method_set_value,
        1, &inputValue, 0, nullptr, dev, nullptr );
    if ( addMethodStatus != UA_STATUSCODE_GOOD )
        {
        G_LOG->debug( "OPCUA_server::UA_Server_addMethodNode "
            "failed for set_value: `%s` (0x%08x)",
            UA_StatusCode_name( addMethodStatus ),
            static_cast<unsigned int>( addMethodStatus ) );
        }

    UA_MethodAttributes_clear( &methodAttr );
    UA_Argument_clear( &inputValue );
    UA_QualifiedName_clear( &qn );

    methodAttr = UA_MethodAttributes_default;
    methodAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", "on" );
    methodAttr.description = UA_LOCALIZEDTEXT_ALLOC( "ru-RU", "Включить." );
    methodAttr.executable = true;
    methodAttr.userExecutable = true;

    qn = UA_QUALIFIEDNAME_ALLOC( 1, "on" );
    addMethodStatus = UA_Server_addMethodNode( server,
        UA_NODEID_NULL, deviceId,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
        qn, methodAttr, method_on,
        0, nullptr, 0, nullptr, dev, nullptr );
    if ( addMethodStatus != UA_STATUSCODE_GOOD )
        {
        G_LOG->debug( "OPCUA_server::UA_Server_addMethodNode "
            "failed for on: `%s` (0x%08x)",
            UA_StatusCode_name( addMethodStatus ),
            static_cast<unsigned int>( addMethodStatus ) );
        }

    UA_MethodAttributes_clear( &methodAttr );
    UA_QualifiedName_clear( &qn );

    methodAttr = UA_MethodAttributes_default;
    methodAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", "off" );
    methodAttr.description = UA_LOCALIZEDTEXT_ALLOC( "ru-RU", "Выключить." );
    methodAttr.executable = true;
    methodAttr.userExecutable = true;

    qn = UA_QUALIFIEDNAME_ALLOC( 1, "off" );
    addMethodStatus = UA_Server_addMethodNode( server,
        UA_NODEID_NULL, deviceId,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
        qn, methodAttr, method_off,
        0, nullptr, 0, nullptr, dev, nullptr );
    if ( addMethodStatus != UA_STATUSCODE_GOOD )
        {
        G_LOG->debug( "OPCUA_server::UA_Server_addMethodNode "
            "failed for off: `%s` (0x%08x)",
            UA_StatusCode_name( addMethodStatus ),
            static_cast<unsigned int>( addMethodStatus ) );
        }

    UA_MethodAttributes_clear( &methodAttr );
    UA_QualifiedName_clear( &qn );
    }

void OPCUA_server::create_PAC_info()
    {
    if ( !server ) return;
    if ( is_PAC_info_created ) return;

    UA_NodeId PAC_NodeId;

    //Create object node.
    UA_ObjectAttributes PAC_InfoObjAttr = UA_ObjectAttributes_default;
    PAC_InfoObjAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US",
        "PAC_info" );
    PAC_InfoObjAttr.description = UA_LOCALIZEDTEXT_ALLOC( "ru-RU",
        "PAC_info" );
    UA_QualifiedName qn = UA_QUALIFIEDNAME_ALLOC( 1, "PAC_info" );

    if ( auto addObjectNodeStatus = UA_Server_addObjectNode( server,
        UA_NODEID_NULL,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_OBJECTSFOLDER ),
        UA_NODEID_NUMERIC( 0, UA_NS0ID_ORGANIZES ),
        qn,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEOBJECTTYPE ),
        PAC_InfoObjAttr, nullptr, &PAC_NodeId );
        addObjectNodeStatus != UA_STATUSCODE_GOOD )
        {
        G_LOG->error( "OPCUA_server::UA_Server_addObjectNode "
            "failed create PAC info node: `%s` (0x%08x)",
            UA_StatusCode_name( addObjectNodeStatus ),
            static_cast<unsigned int>( addObjectNodeStatus ) );
        return;
        }
    UA_ObjectAttributes_clear( &PAC_InfoObjAttr );
    UA_QualifiedName_clear( &qn );

    //Uptime variable.
    UA_VariableAttributes uptimeVarAttr = UA_VariableAttributes_default;
    UA_String value = UA_String_fromChars( "0 дн. " );
    UA_Variant_setScalarCopy( &uptimeVarAttr.value, &value,
        &UA_TYPES[ UA_TYPES_STRING ] );
    std::string node_name = "PAC_info.uptime";
    uptimeVarAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", "uptime" );
    uptimeVarAttr.dataType = UA_TYPES[ UA_TYPES_STRING ].typeId;
    UA_NodeId uptimeNodeId = UA_NODEID_STRING_ALLOC( 0, node_name.c_str() );
    qn = UA_QUALIFIEDNAME_ALLOC( 1, node_name.c_str() );
    UA_Server_addVariableNode( server, uptimeNodeId, PAC_NodeId,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
        qn,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEDATAVARIABLETYPE ),
        uptimeVarAttr, nullptr, nullptr );
    UA_VariableAttributes_clear( &uptimeVarAttr );
    UA_QualifiedName_clear( &qn );
    UA_String_clear( &value );

    //PRODUCT_VERSION_FULL_STR variable.
    UA_VariableAttributes versionVarAttr = UA_VariableAttributes_default;
    UA_String version = UA_String_fromChars( PRODUCT_VERSION_FULL_STR );
    UA_Variant_setScalarCopy( &versionVarAttr.value, &version,
        &UA_TYPES[ UA_TYPES_STRING ] );
    node_name = "PAC_info.version";
    versionVarAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", "version" );
    versionVarAttr.dataType = UA_TYPES[ UA_TYPES_STRING ].typeId;
    UA_NodeId versionNodeId = UA_NODEID_STRING_ALLOC( 0, node_name.c_str() );
    qn = UA_QUALIFIEDNAME_ALLOC( 1, node_name.c_str() );
    UA_Server_addVariableNode( server, versionNodeId, PAC_NodeId,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
        qn,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEDATAVARIABLETYPE ),
        versionVarAttr, nullptr, nullptr );
    UA_VariableAttributes_clear( &versionVarAttr );
    UA_QualifiedName_clear( &qn );
    UA_String_clear( &version );
    UA_NodeId_clear( &PAC_NodeId );
    UA_NodeId_clear( &versionNodeId );

    // Creating Uptime variable read callback.
    UA_DataSource uptimeDataSource{ read_PAC_info_value, nullptr };
    uptimeDataSource.read = read_PAC_info_value;
    UA_Server_setVariableNode_dataSource( server, uptimeNodeId,
        uptimeDataSource );
    UA_NodeId_clear( &uptimeNodeId );

    is_PAC_info_created = true;
    }

UA_StatusCode OPCUA_server::start()
    {
    return UA_Server_run_startup( server );
    }

void OPCUA_server::shutdown()
    {
    if ( server )
        {
        UA_Server_run_shutdown( server );
        UA_Server_delete( server );
        server = nullptr;

        is_dev_objects_created = false;
        is_PAC_info_created = false;
        }
    }

void OPCUA_server::evaluate()
    {
    if ( server ) UA_Server_run_iterate( server, false );
    }

UA_StatusCode OPCUA_server::read_state( UA_Server*, const UA_NodeId*, void*,
    const UA_NodeId*, void* nodeContext, UA_Boolean, const UA_NumericRange*,
    UA_DataValue* dataValue )
    {
    if ( nodeContext != nullptr )
        {
        auto dev = (device*)nodeContext;
        UA_Int32 state = dev->get_state();
        UA_Variant_setScalarCopy( &dataValue->value, &state,
            &UA_TYPES[ UA_TYPES_INT32 ] );
        dataValue->hasValue = true;
        return UA_STATUSCODE_GOOD;
        }

    return UA_STATUSCODE_BAD;
    }

UA_StatusCode OPCUA_server::write_state( UA_Server*,
    const UA_NodeId*, void*,
    const UA_NodeId*, void* nodeContext,
    const UA_NumericRange*,
    const UA_DataValue* value )
    {
    if ( nodeContext != nullptr )
        {
        auto dev = (device*)nodeContext;
        if ( value->hasValue )
            {
            if ( G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] == 1 )
                {
                UA_Int32 state = ( (UA_Int32*)( value->value.data ) )[ 0 ];
                dev->set_state( state );
                return UA_STATUSCODE_GOOD;
                }
            else
                {
                return UA_STATUSCODE_BADUSERACCESSDENIED;
                }
            }
        }
    return UA_STATUSCODE_BAD;
    }

UA_StatusCode OPCUA_server::read_value( UA_Server*, const UA_NodeId*, void*,
    const UA_NodeId*, void* nodeContext, UA_Boolean, const UA_NumericRange*,
    UA_DataValue* dataValue )
    {
    if ( nodeContext != nullptr )
        {
        auto dev = (device*)nodeContext;
        UA_Float newvalue = dev->get_value();
        UA_Variant_setScalarCopy( &dataValue->value, &newvalue,
            &UA_TYPES[ UA_TYPES_FLOAT ] );
        return UA_STATUSCODE_GOOD;
        }
    return UA_STATUSCODE_BAD;
    }

UA_StatusCode OPCUA_server::write_value( UA_Server*,
    const UA_NodeId*, void*,
    const UA_NodeId*, void* nodeContext,
    const UA_NumericRange*,
    const UA_DataValue* value )
    {
    if ( nodeContext != nullptr )
        {
        auto dev = (device*)nodeContext;
        if ( value->hasValue )
            if ( G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] == 1 )
                {
                UA_Float newvalue = ( (UA_Float*)( value->value.data ) )[ 0 ];
                dev->set_value( newvalue );
                return UA_STATUSCODE_GOOD;
                }
            else
                {
                return UA_STATUSCODE_BADUSERACCESSDENIED;
                }
        }
    return UA_STATUSCODE_BAD;
    }

UA_StatusCode OPCUA_server::read_PAC_info_value( UA_Server*, const UA_NodeId*,
    void*, const UA_NodeId*, void*, UA_Boolean,
    const UA_NumericRange*, UA_DataValue* dataValue )
    {
    static auto value = UA_STRING( const_cast<char*>(
        G_PAC_INFO()->get_up_time_str() ) );
    UA_Variant_setScalar( &dataValue->value, &value,
        &UA_TYPES[ UA_TYPES_STRING ] );
    dataValue->value.storageType = UA_VARIANT_DATA_NODELETE;
    dataValue->hasValue = true;

    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUA_server::method_set_state( UA_Server*,
    const UA_NodeId*, void*,
    const UA_NodeId*, void* methodContext,
    const UA_NodeId*, void*,
    size_t inputSize, const UA_Variant* input,
    size_t, UA_Variant* )
    {
    if ( !methodContext )
        {
        return UA_STATUSCODE_BADINTERNALERROR;
        }

    if ( inputSize != 1 || !input ||
        !UA_Variant_hasScalarType( &input[ 0 ], &UA_TYPES[ UA_TYPES_INT32 ] ) )
        {
        return UA_STATUSCODE_BADINVALIDARGUMENT;
        }

    if ( G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] != 1 )
        {
        return UA_STATUSCODE_BADUSERACCESSDENIED;
        }

    auto dev = static_cast<device*>( methodContext );
    const auto state = *static_cast<UA_Int32*>( input[ 0 ].data );
    dev->set_state( state );

    G_LOG->debug( "%s\t OPCUA_server::method_set_state( %d )",
        dev->get_name(), state );

    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUA_server::method_set_value( UA_Server*,
    const UA_NodeId*, void*,
    const UA_NodeId*, void* methodContext,
    const UA_NodeId*, void*,
    size_t inputSize, const UA_Variant* input,
    size_t, UA_Variant* )
    {
    if ( !methodContext )
        {
        return UA_STATUSCODE_BADINTERNALERROR;
        }

    if ( inputSize != 1 || !input ||
        !UA_Variant_hasScalarType( &input[ 0 ], &UA_TYPES[ UA_TYPES_FLOAT ] ) )
        {
        return UA_STATUSCODE_BADINVALIDARGUMENT;
        }

    if ( G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] != 1 )
        {
        return UA_STATUSCODE_BADUSERACCESSDENIED;
        }

    auto dev = static_cast<device*>( methodContext );
    const auto value = *static_cast<UA_Float*>( input[ 0 ].data );
    dev->set_value( value );

    G_LOG->debug( "%s\t OPCUA_server::method_set_value( %f )",
        dev->get_name(), value );

    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUA_server::method_on( UA_Server*,
    const UA_NodeId*, void*,
    const UA_NodeId*, void* methodContext,
    const UA_NodeId*, void*,
    size_t inputSize, const UA_Variant*,
    size_t, UA_Variant* )
    {
    if ( !methodContext )
        {
        return UA_STATUSCODE_BADINTERNALERROR;
        }

    if ( inputSize != 0 )
        {
        return UA_STATUSCODE_BADINVALIDARGUMENT;
        }

    if ( G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] != 1 )
        {
        return UA_STATUSCODE_BADUSERACCESSDENIED;
        }

    auto dev = static_cast<device*>( methodContext );
    dev->on();

    G_LOG->debug( "%s\t OPCUA_server::method_on()", dev->get_name() );

    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUA_server::method_off( UA_Server*,
    const UA_NodeId*, void*,
    const UA_NodeId*, void* methodContext,
    const UA_NodeId*, void*,
    size_t inputSize, const UA_Variant*,
    size_t, UA_Variant* )
    {
    if ( !methodContext )
        {
        return UA_STATUSCODE_BADINTERNALERROR;
        }

    if ( inputSize != 0 )
        {
        return UA_STATUSCODE_BADINVALIDARGUMENT;
        }

    if ( G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] != 1 )
        {
        return UA_STATUSCODE_BADUSERACCESSDENIED;
        }

    auto dev = static_cast<device*>( methodContext );
    dev->off();

    G_LOG->debug( "%s\t OPCUA_server::method_off()", dev->get_name() );

    return UA_STATUSCODE_GOOD;
    }

UA_Server* OPCUA_server::get_server() const
    {
    return server;
    }

OPCUA_server::~OPCUA_server()
    {
    shutdown();
    }
