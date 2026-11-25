#ifdef WIN_OS
#pragma warning( push )
#pragma warning( disable : 4996 )
#endif // WIN_OS
#include "cip_stats.h"

#ifdef PAC_PLCNEXT
#include <cstdlib>
#include <string>
using namespace std::string_literals;
#endif

cip_object_stats::cip_object_stats( const char* objname )
    {
    strcpy( objid, objname );
    strcpy( objlastacidwash, "Не известно" );
    strcpy( objlastwashprogram, "Не определено" );
    strcpy( objlastwash, "Не известно" );
    objcausticwashes = 0;
    changed = 0;
    }

cip_object_stats::~cip_object_stats()
    {
    }

void cip_object_stats::serialize( std::ostream& stream )
    {
    int sl;
    stream << objcausticwashes;
    sl = strlen( objid );
    stream << '#' << sl << '#';
    stream.write( objid, sl );
    sl = strlen( objlastwash );
    stream << '#' << sl << '#';
    stream.write( objlastwash, sl );
    sl = strlen( objlastwashprogram );
    stream << '#' << sl << '#';
    stream.write( objlastwashprogram, sl );
    sl = strlen( objlastacidwash );
    stream << '#' << sl << '#';
    stream.write( objlastacidwash, sl );
    stream << std::endl;
    }

void cip_object_stats::deserialize( std::istream& stream )
    {
    int sl;
    char comma;
    stream >> objcausticwashes >> comma >> sl >> comma;
    stream.read( objid, sl );
    if ( sl >= 0 && sl < MAX_ID_LENGTH * UNICODE_MULTIPLIER )
        {
        objid[ sl ] = '\0';
        }
    stream >> comma >> sl >> comma;
    stream.read( objlastwash, sl );
    if ( sl >= 0 && sl < MAX_FIELD_LENGTH * UNICODE_MULTIPLIER )
        {
        objlastwash[ sl ] = '\0';
        }
    stream >> comma >> sl >> comma;
    stream.read( objlastwashprogram, sl );
    if ( sl >= 0 && sl < MAX_FIELD_LENGTH * UNICODE_MULTIPLIER )
        {
        objlastwashprogram[ sl ] = '\0';
        }
    stream >> comma >> sl >> comma;
    stream.read( objlastacidwash, sl );
    if ( sl >= 0 && sl < MAX_FIELD_LENGTH * UNICODE_MULTIPLIER )
        {
        objlastacidwash[ sl ] = '\0';
        }
    }

void cip_object_stats::resetstats()
    {
    strcpy( objlastacidwash, "Не известно" );
    strcpy( objlastwashprogram, "Не определено" );
    strcpy( objlastwash, "Не известно" );
    objcausticwashes = 0;
    changed = 0;
    }


cip_object_stats* cip_stats::get_obj_stats( char* objid )
    {
    std::map<char*, cip_object_stats*>::iterator ret = obj_stats.find( objid );
    if ( ret == obj_stats.end() )
        {
        char* keychar = new char[ MAX_ID_LENGTH * UNICODE_MULTIPLIER ];
        strcpy( keychar, objid );
        cip_object_stats* newos = new cip_object_stats( keychar );
        obj_stats.emplace( keychar, newos );
        return newos;
        }
    else
        {
        return ret->second;
        }
    }

cip_object_stats* cip_stats::stats_if_exists(
    char* objid, cip_object_stats* cip_if_empty )
    {
    std::map<char*, cip_object_stats*>::iterator ret = obj_stats.find( objid );
    if ( ret == obj_stats.end() )
        {
        return cip_if_empty;
        }
    else
        {
        return ret->second;
        }
    }

cip_stats::cip_stats()
    {
    changed = false;
    lastchanged = get_sec();
    changespending = 0;
    strcpy( filename, "cipstats.base" );
    }

cip_stats::~cip_stats()
    {
    clear();
    }


void cip_stats::serialize( std::ostream& stream )
    {
    int reccount = obj_stats.size();
    stream << reccount << std::endl;
    for ( std::map<char*, cip_object_stats*>::iterator i = obj_stats.begin();
        i != obj_stats.end(); i++ )
        {
        i->second->serialize( stream );
        }
    }

void cip_stats::deserialize( std::istream& stream )
    {
    int reccount;
    stream >> reccount;
    clear();
    for ( int i = 0; i < reccount; i++ )
        {
        char* obj_name = new char[ MAX_ID_LENGTH * UNICODE_MULTIPLIER ];
        cip_object_stats* cos = new cip_object_stats( "tmp" );
        cos->deserialize( stream );
        strcpy( obj_name, cos->objid );
        obj_stats.emplace( obj_name, cos );
        }
    }

void cip_stats::loadFromFile( const char* filename )
    {
    char fname[ 50 ];
#ifdef PAC_PLCNEXT
    sprintf( fname, "/opt/main/%s", filename );
#else
    sprintf( fname, "%s", filename );
#endif // PAC_PLCNEXT
    std::ifstream ifs( fname, std::ios::binary );
    if ( ifs.good() )
        {
        deserialize( ifs );
        ifs.close();
        }
    }

void cip_stats::saveToFile( const char* filename )
    {
    char fname[ 50 ];
#ifdef PAC_PLCNEXT
    sprintf( fname, "/opt/main/%s", filename );
#else
    sprintf( fname, "%s", filename );
#endif // PAC_PLCNEXT
    std::ofstream ofs( fname, std::ios::binary );
    serialize( ofs );
    ofs.close();

#ifdef PAC_PLCNEXT
    std::string syscommand = "chmod 777 "s + fname;
    system( syscommand.c_str() );
#endif
    }

void cip_stats::clear()
    {
    for ( std::map<char*, cip_object_stats*>::iterator i = obj_stats.begin();
        i != obj_stats.end(); i++ )
        {
        delete i->second;
        }
    obj_stats.clear();
    changed = false;
    lastchanged = get_sec();
    changespending = 0;
    }

void cip_stats::evaluate()
    {
    if ( changed )
        {
        saveToFile( filename );
        confirm();
        }
    }

void cip_stats::apply()
    {
    changed = true;
    lastchanged = get_sec();
    changespending++;
    }

void cip_stats::confirm()
    {
    changed = false;
    changespending = 0;
    }
