#include "modbus_client_tests.h"
#include <array>
#include <cstring>
#include <memory>

#include "tolua++.h"
#include "PAC_dev_lua_tests.h" // содержит TOLUA_API int tolua_PAC_dev_open(lua_State*);
#include "PAC_info.h"
#include "lua_manager.h"


using namespace ::testing;

// Единые константы для адреса и байтов
constexpr unsigned int kAddr = 7;
constexpr unsigned char kA = 0x3A;
constexpr unsigned char kB = 0xDE;
constexpr unsigned char kC = 0x68;
constexpr unsigned char kD = 0xB1;

// Вспомогательные функции для преобразования.
static int_4 from_bytes( unsigned char b0, unsigned char b1,
    unsigned char b2, unsigned char b3 )
    {
    int_4 v = 0;
    unsigned char data[ 4 ] = { b0, b1, b2, b3 };
    std::memcpy( &v, data, 4 );
    return v;
    }

// Сдвиг между началом области чтения и записи.
// write_buff_start (13) - read_buff_start (9) = 4.
constexpr unsigned int kRWShift = 4;

// Смещение для чтения write-области через `get_byte`.
constexpr auto kBaseOff = kRWShift + kAddr * 2;

// Записать 4 байта в "зону чтения" (read_buff_start + address*2)
// через set_byte с отрицательным смещением относительно write_buff_start.
static void preset_read_area( modbus_client& cli, unsigned int address,
    unsigned char a, unsigned char b, unsigned char c, unsigned char d )
    {
    const int base = static_cast<int>( address ) * 2 - kRWShift; // (9 + addr*2) - 13
    cli.set_byte( base + 0, a );
    cli.set_byte( base + 1, b );
    cli.set_byte( base + 2, c );
    cli.set_byte( base + 3, d );
    }

// Наследник для доступа к защищённым членам init_frame и tcpclient.
class test_modbus_client : public modbus_client
    {
    public:
        test_modbus_client( unsigned int id, const char* ip,
            const char* const name = nullptr ) :
            modbus_client( id, ip, 502, 50, name )
            {
            }

        auto get_tcp_client()
            {
            return tcpclient;
            }

        saved_params_u_int_4* get_error_params() const
            {
            return error_params;
            }

        void test_init_frame( unsigned int address, unsigned int value,
            unsigned int function_code )
            {
            init_frame( address, value, function_code );
            }
    };

// Для теста init_frame.
class ModbusClientInitFrameTest : public ::testing::Test
    {
    protected:
        test_modbus_client m_client{ 1, "127.0.0.1" };
    };


// Для тестов modbus_client без Lua.
class ModbusClientTest : public ::testing::Test
    {
    protected:
        modbus_client cli{ 1, "127.0.0.1" };
    };

// Для Lua-тестов.
class ModbusClientLuaTest : public ::testing::Test
    {
    protected:
        lua_State* L = nullptr;

        void SetUp() override
            {
            L = lua_open();
            ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
            ASSERT_EQ( 0, luaL_dostring( L,
                "cli = modbus_client(1, '127.0.0.1')" ) );
            }

        void TearDown() override
            {
            if ( L )
                {
                lua_close( L );
                L = nullptr;
                }
            }

        static long get_lua_global_int( lua_State* L, const char* name )
            {
            lua_getfield( L, LUA_GLOBALSINDEX, name );
            auto v = static_cast<long>( tolua_tonumber( L, -1, 0 ) );
            lua_pop( L, 1 );
            return v;
            }

        // Предварительная настройка read-области и проверка результата
        // Lua-командой cmd.
        void test_bytes( const int_4 expected,
            const char* cmd /*например, "res = cli:get_int4_ab_cd(ADDR)\n"*/ )
            {
            lua_pushnumber( L, static_cast<lua_Number>( kAddr ) );
            lua_setglobal( L, "ADDR" );
            lua_pushnumber( L, kA ); lua_setglobal( L, "A" );
            lua_pushnumber( L, kB ); lua_setglobal( L, "B" );
            lua_pushnumber( L, kC ); lua_setglobal( L, "C" );
            lua_pushnumber( L, kD ); lua_setglobal( L, "D" );

            // Предварительно настроим read-область через set_byte
            // (адрес = addr*2 - kRWShift + offset).
            auto lua_script =
                "base_rd = ADDR * 2 - 4\n"
                "cli:set_byte(base_rd + 0, A)\n"
                "cli:set_byte(base_rd + 1, B)\n"
                "cli:set_byte(base_rd + 2, C)\n"
                "cli:set_byte(base_rd + 3, D)\n";
            ASSERT_EQ( 0, luaL_dostring( L, lua_script ) );
            ASSERT_EQ( 0, luaL_dostring( L, cmd ) );

            long res = get_lua_global_int( L, "res" );
            EXPECT_EQ( static_cast<int_4>( res ), expected );
            }

        void set_bytes( const char* cmd /*например, "cli:set_int4_cd_ab(ADDR, VAL)\n"*/ )
            {
            const int_4 val = from_bytes( kA, kB, kC, kD );
            lua_pushnumber( L, static_cast<lua_Number>( kAddr ) );
            lua_setglobal( L, "ADDR" );
            lua_pushnumber( L, static_cast<lua_Number>( val ) );
            lua_setglobal( L, "VAL" );

            ASSERT_EQ( 0, luaL_dostring( L, cmd ) );
            auto lua_script =
                "base = 4 + ADDR * 2\n"
                "b0 = cli:get_byte(base + 0)\n"
                "b1 = cli:get_byte(base + 1)\n"
                "b2 = cli:get_byte(base + 2)\n"
                "b3 = cli:get_byte(base + 3)\n";
            ASSERT_EQ( 0, luaL_dostring( L, lua_script ) );
            }
    };

// ------------------------------
// TEST_F: init_frame
// ------------------------------
TEST_F( ModbusClientInitFrameTest, init_frame )
    {
    const auto ADDRESS = 100;
    const auto VALUE = 10;
    const auto FUNCTION_CODE = 15;

    m_client.test_init_frame( ADDRESS, VALUE, FUNCTION_CODE );
    ASSERT_EQ( m_client.get_tcp_client()->buff[ 7 ], FUNCTION_CODE );
    ASSERT_EQ( m_client.get_tcp_client()->buff[ 9 ], ADDRESS );
    ASSERT_EQ( m_client.get_tcp_client()->buff[ 11 ], VALUE );
    }

// ------------------------------
// TEST_F: Запись (set_*) — порядок байтов (единый addr/байты)
// ------------------------------
TEST_F( ModbusClientTest, set_int4_ab_cd )
    {
    const int_4 val = from_bytes( kA, kB, kC, kD );
    cli.set_int4_ab_cd( kAddr, val );

    EXPECT_EQ( cli.get_byte( kBaseOff + 0 ), kA ); // A
    EXPECT_EQ( cli.get_byte( kBaseOff + 1 ), kB ); // B
    EXPECT_EQ( cli.get_byte( kBaseOff + 2 ), kC ); // C
    EXPECT_EQ( cli.get_byte( kBaseOff + 3 ), kD ); // D
    }

TEST_F( ModbusClientTest, set_int4_cd_ab )
    {
    const int_4 val = from_bytes( kA, kB, kC, kD );
    cli.set_int4_cd_ab( kAddr, val );

    EXPECT_EQ( cli.get_byte( kBaseOff + 0 ), kC ); // C
    EXPECT_EQ( cli.get_byte( kBaseOff + 1 ), kD ); // D
    EXPECT_EQ( cli.get_byte( kBaseOff + 2 ), kA ); // A
    EXPECT_EQ( cli.get_byte( kBaseOff + 3 ), kB ); // B
    }

TEST_F( ModbusClientTest, set_int4_dc_ba )
    {
    const int_4 val = from_bytes( kA, kB, kC, kD );
    cli.set_int4_dc_ba( kAddr, val );

    EXPECT_EQ( cli.get_byte( kBaseOff + 0 ), kD ); // D
    EXPECT_EQ( cli.get_byte( kBaseOff + 1 ), kC ); // C
    EXPECT_EQ( cli.get_byte( kBaseOff + 2 ), kB ); // B
    EXPECT_EQ( cli.get_byte( kBaseOff + 3 ), kA ); // A
    }

TEST_F( ModbusClientTest, set_int4 )
    {
    const int_4 val = from_bytes( kA, kB, kC, kD );
    cli.set_int4( kAddr, val );

    EXPECT_EQ( cli.get_byte( kBaseOff + 0 ), kB ); // B
    EXPECT_EQ( cli.get_byte( kBaseOff + 1 ), kA ); // A
    EXPECT_EQ( cli.get_byte( kBaseOff + 2 ), kD ); // D
    EXPECT_EQ( cli.get_byte( kBaseOff + 3 ), kC ); // C
    }

// ------------------------------
// TEST_F: Чтение (get_*) — порядок байтов (единый addr/байты)
// ------------------------------
TEST_F( ModbusClientTest, get_int4_ab_cd )
    {
    preset_read_area( cli, kAddr, kA, kB, kC, kD );

    const int_4 expected = from_bytes( kA, kB, kC, kD );
    EXPECT_EQ( cli.get_int4_ab_cd( kAddr ), expected );
    }

TEST_F( ModbusClientTest, get_int4_cd_ab )
    {
    preset_read_area( cli, kAddr, kA, kB, kC, kD );

    // Ожидаемое формирование: [C, D, A, B]
    const int_4 expected = from_bytes( kC, kD, kA, kB );
    EXPECT_EQ( cli.get_int4_cd_ab( kAddr ), expected );
    }

TEST_F( ModbusClientTest, get_int4_dc_ba )
    {
    preset_read_area( cli, kAddr, kA, kB, kC, kD );

    // Ожидаемое формирование: [D, C, B, A]
    const int_4 expected = from_bytes( kD, kC, kB, kA );
    EXPECT_EQ( cli.get_int4_dc_ba( kAddr ), expected );
    }

TEST_F( ModbusClientTest, get_int4 )
    {
    preset_read_area( cli, kAddr, kA, kB, kC, kD );

    // Ожидаемое формирование: [B, A, D, C]
    const int_4 expected = from_bytes( kB, kA, kD, kC );
    EXPECT_EQ( cli.get_int4( kAddr ), expected );
    }

TEST_F( ModbusClientTest, OutOfRangeAddressReturnsZero )
    {
    // Для read: условие выхода
    // address*2 > buff_size - read_buff_start - sizeof(int_4)
    // buff_size=256, read_buff_start=9, sizeof(int_4)=4 =>
    // порог 243 => address >= 122.
    const auto BAD_ADDRESS = 122;
    EXPECT_EQ( cli.get_int4( BAD_ADDRESS ), 0 );
    EXPECT_EQ( cli.get_int4_ab_cd( BAD_ADDRESS ), 0 );
    EXPECT_EQ( cli.get_int4_cd_ab( BAD_ADDRESS ), 0 );
    EXPECT_EQ( cli.get_int4_dc_ba( BAD_ADDRESS ), 0 );

    cli.set_int4( BAD_ADDRESS, 10 );
    cli.set_int4_ab_cd( BAD_ADDRESS, 10 );
    cli.set_int4_cd_ab( BAD_ADDRESS, 10 );
    cli.set_int4_dc_ba( BAD_ADDRESS, 10 );
    }

// ------------------------------
// TEST_F: Lua — запись (set_*) и проверка байтов
// ------------------------------
TEST_F( ModbusClientLuaTest, set_int4 )
    {
    // Проверка некорректного вызова (без параметров).
    ASSERT_TRUE( luaL_dostring( L, "cli:set_int4()\n" ) );

    set_bytes( "cli:set_int4(ADDR, VAL)\n" );
    EXPECT_EQ( kB, get_lua_global_int( L, "b0" ) );
    EXPECT_EQ( kA, get_lua_global_int( L, "b1" ) );
    EXPECT_EQ( kD, get_lua_global_int( L, "b2" ) );
    EXPECT_EQ( kC, get_lua_global_int( L, "b3" ) );
    }

TEST_F( ModbusClientLuaTest, set_int4_ab_cd )
    {
    // Проверка некорректного вызова (без параметров).
    ASSERT_TRUE( luaL_dostring( L, "cli:set_int4_ab_cd()\n" ) );

    set_bytes( "cli:set_int4_ab_cd(ADDR, VAL)\n" );
    EXPECT_EQ( kA, get_lua_global_int( L, "b0" ) );
    EXPECT_EQ( kB, get_lua_global_int( L, "b1" ) );
    EXPECT_EQ( kC, get_lua_global_int( L, "b2" ) );
    EXPECT_EQ( kD, get_lua_global_int( L, "b3" ) );
    }

TEST_F( ModbusClientLuaTest, set_int4_cd_ab )
    {
    // Проверка некорректного вызова (без параметров).
    ASSERT_TRUE( luaL_dostring( L, "cli:set_int4_cd_ab()\n" ) );

    set_bytes( "cli:set_int4_cd_ab(ADDR, VAL)\n" );
    EXPECT_EQ( kC, get_lua_global_int( L, "b0" ) );
    EXPECT_EQ( kD, get_lua_global_int( L, "b1" ) );
    EXPECT_EQ( kA, get_lua_global_int( L, "b2" ) );
    EXPECT_EQ( kB, get_lua_global_int( L, "b3" ) );
    }

TEST_F( ModbusClientLuaTest, set_int4_dc_ba )
    {
    // Проверка некорректного вызова (без параметров).
    ASSERT_TRUE( luaL_dostring( L, "cli:set_int4_dc_ba()\n" ) );

    set_bytes( "cli:set_int4_dc_ba(ADDR, VAL)\n" );
    EXPECT_EQ( kD, get_lua_global_int( L, "b0" ) );
    EXPECT_EQ( kC, get_lua_global_int( L, "b1" ) );
    EXPECT_EQ( kB, get_lua_global_int( L, "b2" ) );
    EXPECT_EQ( kA, get_lua_global_int( L, "b3" ) );
    }

// ------------------------------
// TEST_F: Lua — чтение (get_*) и проверка значений
// ------------------------------
TEST_F( ModbusClientLuaTest, get_int4 ) // B A D C
    {
    // Проверка некорректного вызова (без параметров).
    ASSERT_TRUE( luaL_dostring( L, "cli:get_int4()\n" ) );

    const int_4 expected = from_bytes( kB, kA, kD, kC );
    test_bytes( expected, "res = cli:get_int4(ADDR)\n" );
    }

TEST_F( ModbusClientLuaTest, get_int4_ab_cd )
    {
    // Проверка некорректного вызова (без параметров).
    ASSERT_TRUE( luaL_dostring( L, "cli:get_int4_ab_cd()\n" ) );

    const int_4 expected = from_bytes( kA, kB, kC, kD );
    test_bytes( expected, "res = cli:get_int4_ab_cd(ADDR)\n" );
    }

TEST_F( ModbusClientLuaTest, get_int4_cd_ab )
    {
    // Проверка некорректного вызова (без параметров).
    ASSERT_TRUE( luaL_dostring( L, "cli:get_int4_cd_ab()\n" ) );

    const int_4 expected = from_bytes( kC, kD, kA, kB );
    test_bytes( expected, "res = cli:get_int4_cd_ab(ADDR)\n" );
    }

TEST_F( ModbusClientLuaTest, get_int4_dc_ba )
    {
    // Проверка некорректного вызова (без параметров).
    ASSERT_TRUE( luaL_dostring( L, "cli:get_int4_dc_ba()\n" ) );

    const int_4 expected = from_bytes( kD, kC, kB, kA );
    test_bytes( expected, "res = cli:get_int4_dc_ba(ADDR)\n" );
    }


TEST( i_simple_error, modbus_client )
    {
    G_ERRORS_MANAGER->clear();
    modbus_client::clear_ids();

    test_modbus_client client_1{ 1u, "127.0.0.1", "M1" };
    test_modbus_client client_2{ 1u, "127.0.0.1", "M2" };

    // Проверка, что второй клиент получил уникальный идентификатор.
    EXPECT_EQ( client_1.get_serial_n(), 1u );
    EXPECT_EQ( client_2.get_serial_n(), 2u );

    i_simple_error& error = client_1;

    // В режиме эмулятора нет ошибки.
    EXPECT_EQ( 1, error.get_state() );

    G_PAC_INFO()->emulation_off();

    EXPECT_NE( nullptr, client_1.get_error_params() );
    EXPECT_STREQ( "M1", error.get_name() );
    EXPECT_STREQ( "нет связи (Modbus)", error.get_error_description() );
    EXPECT_EQ( -1, error.get_error_id() );
    EXPECT_EQ( -1, error.get_state() );
    EXPECT_EQ( 1U, error.get_serial_n() );
    EXPECT_EQ( 200, error.get_error_type() );

    client_1.get_tcp_client()->set_connected_state( tcp_client::ACS_CONNECTED );
    EXPECT_EQ( 0, error.get_state() );
    G_ERRORS_MANAGER->clear();

    G_PAC_INFO()->emulation_on();
    }

TEST( errors_manager, saves_modbus_simple_error )
    {
    std::array<char, 300> buffer{};
    u_int_2 error_id = 0;

    G_PAC_INFO()->emulation_off();
    G_ERRORS_MANAGER->clear();
    test_modbus_client client{ 1, "127.0.0.1", "M1" };
    G_ERRORS_MANAGER->evaluate();
    G_ERRORS_MANAGER->save_as_Lua_str( buffer.data(), error_id );

    const auto expected = R"s({
description="M1 - нет связи (Modbus)",
priority=250,
state=1,
type=4,
group="тревога",
id_n=1,
id_object_alarm_number=1,
id_type=200,
suppress=false
},
)s";
    EXPECT_STREQ( expected, buffer.data() );
    EXPECT_EQ( 1, error_id );
    G_ERRORS_MANAGER->clear();

    G_PAC_INFO()->emulation_on();
    }
