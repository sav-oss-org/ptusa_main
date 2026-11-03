#if !defined WIN_OS && \
    !( defined LINUX_OS && defined PAC_PC ) && \
	!( defined LINUX_OS && defined PAC_PLCNEXT )
#error You must define OS!
#endif

#include "base_mem.h"
#include "log.h"

#ifdef LINUX_OS
#include <unistd.h>
#endif

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>

#ifdef WIN_OS
#include <windows.h>
#include <fileapi.h>
#include <io.h>
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
SRAM::SRAM( const std::filesystem::path& file_name, u_int size ) :
    file_path( file_name ),
    tmp_path( file_name.string() + ".tmp" ),
    total_size( size )
    {
    params_data = new std::byte[ total_size ];
    memset( params_data, 0, total_size );
    }
//-----------------------------------------------------------------------------
SRAM::~SRAM()
    {
    if ( params_data )
        {
        delete[] params_data;
        params_data = nullptr;
        }
    }
//-----------------------------------------------------------------------------
int SRAM::load_data()
    {
    zero_fill();

    if ( std::error_code ec; !std::filesystem::exists( file_path, ec ) )
        {
        G_LOG->notice( "SRAM() - File (%s) not found.",
            file_path.string().c_str() );
        return 1;
        }
    else
        {
        G_LOG->notice( "SRAM() - File (%s) found, loading.",
            file_path.string().c_str() );
        auto f = fopen( file_path.string().c_str(), "rb" );

        if ( f )
            {
            fseek( f, 0, SEEK_SET );
            auto res = fread( get_data(), sizeof( std::byte ), get_size(), f );
            fclose( f );
            f = nullptr;

            if ( res == 0 )
                {
                G_LOG->error( "Error reading device (%s) : %s.\n",
                    file_path.string().c_str(), strerror( errno ) );
                return 3;
                }
            // Если прочитано меньше, чем нужно, то выдаем предупреждение, но
            // продолжаем работу. Далее, в функции
            // `params_manager::get_instance()->final_init()` будет
            // проверяться CRC и при несоответствии будет выдана ошибка.
            if ( res < get_size() )
                {
                G_LOG->warning(
                    "SRAM() - Warning: fread (%s) read %zu of %u bytes.",
                    file_path.string().c_str(), res, get_size() );
                }
            }
        else
            {
            G_LOG->error(
                "SRAM() - ERROR: Can't open device (%s) : %s.\n",
                file_path.string().c_str(), strerror( errno ) );
            return 2;
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int SRAM::safe_save()
    {
    return safe_save_buffer( get_data(), true );
    }
//-----------------------------------------------------------------------------
int SRAM::safe_save( const std::byte* data )
    {
    return safe_save_buffer( data, false );
    }
//-----------------------------------------------------------------------------
int SRAM::safe_save_buffer( const std::byte* data, bool log_messages )
    {
    std::chrono::high_resolution_clock::time_point start;
    if ( log_messages && G_DEBUG )
        {
        start = std::chrono::high_resolution_clock::now();
        }

    // Схема атомарного сохранения:
    //    1. записать данные во временный файл
    //    2. fsync( temp )
    //    3. rename( temp, target )
    //    4. fsync( directory ) - для Linux

    if ( FILE* temp = fopen( tmp_path.string().c_str(), "w+b" ); !temp )
        {
        if ( log_messages )
            G_LOG->error( "SRAM() - ERROR: Can't open file (%s) : %s.\n",
                tmp_path.string().c_str(), strerror( errno ) );

        return 1;
        }
    else
        {
        if ( auto res =
            fwrite( data, sizeof( std::byte ), get_size(), temp );
            res != get_size() )
            {
            if ( log_messages )
                G_LOG->error( "SRAM() - ERROR: fwrite (%s) wrote %zu of %u bytes.",
                    tmp_path.string().c_str(), res, get_size() );
            fclose( temp );
            return 2;
            }

        if ( fflush( temp ) != 0 )
            {
            if ( log_messages )
                G_LOG->error( "SRAM() - ERROR: fflush (%s) failed: %s.",
                    tmp_path.string().c_str(), strerror( errno ) );
            fclose( temp );
            return 2;
            }

        auto fd =
#ifdef WIN_OS
            _fileno( temp );
#else
            fileno( temp );
#endif

#ifdef WIN_OS
        auto hFile = (HANDLE)_get_osfhandle( fd );

        if ( hFile == INVALID_HANDLE_VALUE )
            {
            if ( log_messages )
                G_LOG->error( "SRAM() - ERROR: Can't get handle (%s).",
                    tmp_path.string().c_str() );
            fclose( temp );
            return 2;
            }
        else
            {
            if ( !FlushFileBuffers( hFile ) )
                {
                if ( log_messages )
                    G_LOG->error(
                        "SRAM() - ERROR: FlushFileBuffers (%s) failed (%lu).",
                        file_path.string().c_str(), GetLastError() );

                fclose( temp );
                return 2;
                }
            }
#else
        if ( fsync( fd ) != 0 )
            {
            if ( log_messages )
                G_LOG->error( "SRAM() - ERROR: fsync (%s) failed: %s.",
                    tmp_path.string().c_str(), strerror( errno ) );
            fclose( temp );
            return 2;
            }
#endif

        if ( fclose( temp ) != 0 )
            {
            if ( log_messages )
                G_LOG->error( "SRAM() - ERROR: fclose (%s) failed: %s.",
                    tmp_path.string().c_str(), strerror( errno ) );
            return 2;
            }
        temp = nullptr;

#ifdef WIN_OS
        if ( !MoveFileExA( tmp_path.string().c_str(), file_path.string().c_str(),
            MOVEFILE_REPLACE_EXISTING ) )
            {
            if ( log_messages )
                G_LOG->error( "SRAM() - ERROR: Can't replace (%s): %lu.",
                    file_path.string().c_str(), GetLastError() );
            return 3;
            }
#else
        std::error_code ec;
        std::filesystem::rename( tmp_path, file_path, ec );
        if ( ec )
            {
            if ( log_messages )
                G_LOG->error( "SRAM() - ERROR: Can't rename (%s) to (%s) : %s.\n",
                    tmp_path.string().c_str(), file_path.string().c_str(),
                    ec.message().c_str() );
            return 3;
            }

        if ( auto dir = open( tmp_path.parent_path().string().c_str(), O_RDONLY );
            dir != -1 )
            {
            if ( fsync( dir ) != 0 )
                {
                if ( log_messages )
                    G_LOG->error( "SRAM() - ERROR: fsync directory (%s) failed: %s.",
                        tmp_path.parent_path().string().c_str(), strerror( errno ) );
                close( dir );
                return 3;
                }
            close( dir );
            }
        else
            {
            if ( log_messages )
                G_LOG->error( "SRAM() - ERROR: Can't open directory (%s): %s.",
                    tmp_path.parent_path().string().c_str(), strerror( errno ) );
            return 3;
            }
#endif

        if ( log_messages && G_DEBUG )
            {
            auto end = std::chrono::high_resolution_clock::now();
            const auto duration = std::chrono::duration_cast<
                std::chrono::microseconds>( end - start ).count();
            G_LOG->debug( "SRAM::safe_save() - write time: %lld us (%s).",
                static_cast<long long>( duration ),
                file_path.string().c_str() );
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
std::byte* SRAM::get_data()
    {
    return params_data;
    }
//-----------------------------------------------------------------------------
void SRAM::zero_fill()
    {
    if ( params_data )
        {
        memset( params_data, 0, total_size );
        }
    }
//-----------------------------------------------------------------------------
u_int SRAM::get_size() const
    {
    return total_size;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
