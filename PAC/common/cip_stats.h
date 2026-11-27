#pragma once
#ifndef cip_stats_h__
#define cip_stats_h__

#include <iostream>
#include <map>
#include <fstream>
#include "dtime.h"
#include <cstring>
#include "mcaRec.h"
#define MAX_ID_LENGTH    32
#define MAX_FIELD_LENGTH 24

class Serializable
    {
    public:
        Serializable()
            {
            }
        virtual ~Serializable()
            {
            }
        virtual void serialize( std::ostream& stream ) = 0;
        virtual void deserialize( std::istream& stream ) = 0;
    };

struct cmp_str
    {
        bool operator()( char const* a, char const* b ) const
            {
            return strcmp( a, b ) < 0;
            }
    };

class cip_object_stats : public Serializable
    {
    public:
        cip_object_stats( const char* objname );
        ~cip_object_stats();
        char objid[ MAX_ID_LENGTH * UNICODE_MULTIPLIER ];
        char objlastwash[ MAX_FIELD_LENGTH * UNICODE_MULTIPLIER ];
        char objlastwashprogram[ MAX_FIELD_LENGTH * UNICODE_MULTIPLIER ];
        int objcausticwashes;
        char objlastacidwash[ MAX_FIELD_LENGTH * UNICODE_MULTIPLIER ];
        int changed;
        virtual void serialize( std::ostream& stream );
        virtual void deserialize( std::istream& stream );
        void resetstats();

    private:
    };

class cip_stats : Serializable
    {
    public:
        cip_object_stats* get_obj_stats( char* objid );
        cip_object_stats* stats_if_exists(
            char* objid, cip_object_stats* cip_if_empty );
        cip_stats();
        ~cip_stats();
        virtual void serialize( std::ostream& stream );
        virtual void deserialize( std::istream& stream );
        void loadFromFile( const char* filename );
        void saveToFile( const char* filename );
        void clear();
        void evaluate();
        void apply();
        char filename[ MAX_ID_LENGTH ];

    private:
        std::map<char*, cip_object_stats*, cmp_str> obj_stats;
        bool changed;
        uint32_t lastchanged;
        unsigned int changespending;
        void confirm();
    };


#endif // cip_stats_h__
