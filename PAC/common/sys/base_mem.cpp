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
    total_size( size )
    {
    // Путь сохраняется в абсолютном виде, чтобы сообщения об ошибках были
    // однозначными и не зависели от текущего каталога процесса.
    std::error_code ec;
    auto resolved = std::filesystem::absolute( file_name, ec );
    if ( ec )
        {
        resolved = file_name;
        }

    file_path = resolved.lexically_normal();
    tmp_path = file_path.string() + ".tmp";

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
    std::chrono::high_resolution_clock::time_point start;
    if ( G_DEBUG )
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
        if ( !error_logged )
            {
            // errno нужно сохранить до обращения к другим функциям, иначе он
            // может быть перезаписан.
            const int err = errno;

            G_LOG->error( "SRAM() - ERROR: Can't open file (%s) : %s.\n",
                tmp_path.string().c_str(), strerror( err ) );
            error_logged = true;
            }

        return 1;
        }
    else
        {
        // После успешного создания файла ошибка может появиться снова, поэтому
        // сообщение разрешается повторить.
        error_logged = false;

        if ( auto res =
            fwrite( get_data(), sizeof( std::byte ), get_size(), temp );
            res != get_size() )
            {
            G_LOG->error( "SRAM() - ERROR: fwrite (%s) wrote %zu of %u bytes.",
                tmp_path.string().c_str(), res, get_size() );
            fclose( temp );
            return 2;
            }

        fflush( temp );

        auto fd =
#ifdef WIN_OS
            _fileno( temp );
#else
            fileno( temp );
#endif

#ifdef WIN_OS
        auto hFile = (HANDLE)_get_osfhandle( fd );

        if ( hFile != INVALID_HANDLE_VALUE )
            {
            if ( !FlushFileBuffers( hFile ) )
                {
                G_LOG->error(
                    "SRAM() - ERROR: FlushFileBuffers (%s) failed (%lu).",
                    file_path.string().c_str(), GetLastError() );

                fclose( temp );
                return 2;
                }
            }
#else
        fsync( fd );
#endif

        fclose( temp );
        temp = nullptr;

#ifdef WIN_OS
        MoveFileExA( tmp_path.string().c_str(), file_path.string().c_str(),
            MOVEFILE_REPLACE_EXISTING );
#else
        std::error_code ec;
        std::filesystem::rename( tmp_path, file_path, ec );
        if ( ec )
            {
            G_LOG->error( "SRAM() - ERROR: Can't rename (%s) to (%s) : %s.\n",
                tmp_path.string().c_str(), file_path.string().c_str(),
                ec.message().c_str() );
            return 3;
            }

        if ( auto dir = open( tmp_path.parent_path().string().c_str(), O_RDONLY );
            dir != -1 )
            {
            fsync( dir );
            close( dir );
            }
#endif

        if ( G_DEBUG )
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
std::filesystem::path SRAM::get_directory() const
    {
    return file_path.parent_path();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
