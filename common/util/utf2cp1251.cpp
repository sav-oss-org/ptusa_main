#include "utf2cp1251.h"
#include <string.h>

typedef struct ConvLetter
    {
        char win1251;
        int unicode;
    } Letter;


static Letter g_letters[] = {
    { '\x82', 0x201A }, // SINGLE LOW-9 QUOTATION MARK
    { '\x83', 0x0453 }, // CYRILLIC SMALL LETTER GJE
    { '\x84', 0x201E }, // DOUBLE LOW-9 QUOTATION MARK
    { '\x85', 0x2026 }, // HORIZONTAL ELLIPSIS
    { '\x86', 0x2020 }, // DAGGER
    { '\x87', 0x2021 }, // DOUBLE DAGGER
    { '\x88', 0x20AC }, // EURO SIGN
    { '\x89', 0x2030 }, // PER MILLE SIGN
    { '\x8A', 0x0409 }, // CYRILLIC CAPITAL LETTER LJE
    { '\x8B', 0x2039 }, // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
    { '\x8C', 0x040A }, // CYRILLIC CAPITAL LETTER NJE
    { '\x8D', 0x040C }, // CYRILLIC CAPITAL LETTER KJE
    { '\x8E', 0x040B }, // CYRILLIC CAPITAL LETTER TSHE
    { '\x8F', 0x040F }, // CYRILLIC CAPITAL LETTER DZHE
    { '\x90', 0x0452 }, // CYRILLIC SMALL LETTER DJE
    { '\x91', 0x2018 }, // LEFT SINGLE QUOTATION MARK
    { '\x92', 0x2019 }, // RIGHT SINGLE QUOTATION MARK
    { '\x93', 0x201C }, // LEFT DOUBLE QUOTATION MARK
    { '\x94', 0x201D }, // RIGHT DOUBLE QUOTATION MARK
    { '\x95', 0x2022 }, // BULLET
    { '\x96', 0x2013 }, // EN DASH
    { '\x97', 0x2014 }, // EM DASH
    { '\x99', 0x2122 }, // TRADE MARK SIGN
    { '\x9A', 0x0459 }, // CYRILLIC SMALL LETTER LJE
    { '\x9B', 0x203A }, // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
    { '\x9C', 0x045A }, // CYRILLIC SMALL LETTER NJE
    { '\x9D', 0x045C }, // CYRILLIC SMALL LETTER KJE
    { '\x9E', 0x045B }, // CYRILLIC SMALL LETTER TSHE
    { '\x9F', 0x045F }, // CYRILLIC SMALL LETTER DZHE
    { '\xA0', 0x00A0 }, // NO-BREAK SPACE
    { '\xA1', 0x040E }, // CYRILLIC CAPITAL LETTER SHORT U
    { '\xA2', 0x045E }, // CYRILLIC SMALL LETTER SHORT U
    { '\xA3', 0x0408 }, // CYRILLIC CAPITAL LETTER JE
    { '\xA4', 0x00A4 }, // CURRENCY SIGN
    { '\xA5', 0x0490 }, // CYRILLIC CAPITAL LETTER GHE WITH UPTURN
    { '\xA6', 0x00A6 }, // BROKEN BAR
    { '\xA7', 0x00A7 }, // SECTION SIGN
    { '\xA8', 0x0401 }, // CYRILLIC CAPITAL LETTER IO
    { '\xA9', 0x00A9 }, // COPYRIGHT SIGN
    { '\xAA', 0x0404 }, // CYRILLIC CAPITAL LETTER UKRAINIAN IE
    { '\xAB', 0x00AB }, // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    { '\xAC', 0x00AC }, // NOT SIGN
    { '\xAD', 0x00AD }, // SOFT HYPHEN
    { '\xAE', 0x00AE }, // REGISTERED SIGN
    { '\xAF', 0x0407 }, // CYRILLIC CAPITAL LETTER YI
    { '\xB0', 0x00B0 }, // DEGREE SIGN
    { '\xB1', 0x00B1 }, // PLUS-MINUS SIGN
    { '\xB2', 0x0406 }, // CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
    { '\xB3', 0x0456 }, // CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
    { '\xB4', 0x0491 }, // CYRILLIC SMALL LETTER GHE WITH UPTURN
    { '\xB5', 0x00B5 }, // MICRO SIGN
    { '\xB6', 0x00B6 }, // PILCROW SIGN
    { '\xB7', 0x00B7 }, // MIDDLE DOT
    { '\xB8', 0x0451 }, // CYRILLIC SMALL LETTER IO
    { '\xBA', 0x0454 }, // CYRILLIC SMALL LETTER UKRAINIAN IE
    { '\xBB', 0x00BB }, // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    { '\xBC', 0x0458 }, // CYRILLIC SMALL LETTER JE
    { '\xBD', 0x0405 }, // CYRILLIC CAPITAL LETTER DZE
    { '\xBE', 0x0455 }, // CYRILLIC SMALL LETTER DZE
    { '\xBF', 0x0457 } // CYRILLIC SMALL LETTER YI
};

int convert_utf8_to_windows1251(
    const char* utf8, char* windows1251, size_t n, size_t buff_size )
    {
    if ( !buff_size )
        {
        *windows1251 = '\0';
        return 0;
        }
    buff_size -= 1; // Оставляем последний символ для сохранения '\0'.

    size_t j = 0;
    int first5bit;
    int sec6bit;
    int unicode_char;

    for ( size_t i = 0; i < n && utf8[ i ] != 0 && j < buff_size; ++i )
        {
        char prefix = utf8[ i ];
        char suffix = utf8[ i + 1 ];
        if ( prefix == '\xE2' && suffix == '\x84' && utf8[ i + 2 ] == '\x96' )
            {
            windows1251[ j ] = '\xB9'; //№
            i++;
            goto NEXT_LETTER;
            }

        if ( ( prefix & 0x80 ) == 0 )
            {
            windows1251[ j ] = (char)prefix;
            ++j;
            }
        else if ( ( ~prefix ) & 0x20 )
            {
            first5bit = prefix & 0x1F;
            first5bit <<= 6;
            sec6bit = suffix & 0x3F;
            unicode_char = first5bit + sec6bit;


            if ( unicode_char >= 0x410 && unicode_char <= 0x44F )
                {
                windows1251[ j ] = (char)( unicode_char - 0x350 );
                }
            else if ( unicode_char >= 0x80 && unicode_char <= 0xFF )
                {
                windows1251[ j ] = (char)( unicode_char );
                }
            else if ( unicode_char >= 0x402 && unicode_char <= 0x403 )
                {
                windows1251[ j ] = (char)( unicode_char - 0x382 );
                }
            else
                {
                int count = sizeof( g_letters ) / sizeof( Letter );
                for ( int k = 0; k < count; ++k )
                    {
                    if ( unicode_char == g_letters[ k ].unicode )
                        {
                        windows1251[ j ] = g_letters[ k ].win1251;
                        goto NEXT_LETTER;
                        }
                    }
                // can't convert this char
                windows1251[ j ] = 0;
                return 0;
                }
        NEXT_LETTER:
            ++i;
            ++j;
            }
        else
            {
            // can't convert this chars
            windows1251[ j ] = 0;
            return 0;
            }
        }
    windows1251[ j ] = 0;
    return 1;
    }

void convert_windows1251_to_utf8( char* out, const char* in )
    {
    static const int table[ 128 ] = { 0x82D0, 0x83D0, 0x9A80E2, 0x93D1,
        0x9E80E2, 0xA680E2, 0xA080E2, 0xA180E2, 0xAC82E2, 0xB080E2, 0x89D0,
        0xB980E2, 0x8AD0, 0x8CD0, 0x8BD0, 0x8FD0, 0x92D1, 0x9880E2, 0x9980E2,
        0x9C80E2, 0x9D80E2, 0xA280E2, 0x9380E2, 0x9480E2, 0, 0xA284E2, 0x99D1,
        0xBA80E2, 0x9AD1, 0x9CD1, 0x9BD1, 0x9FD1, 0xA0C2, 0x8ED0, 0x9ED1,
        0x88D0, 0xA4C2, 0x90D2, 0xA6C2, 0xA7C2, 0x81D0, 0xA9C2, 0x84D0, 0xABC2,
        0xACC2, 0xADC2, 0xAEC2, 0x87D0, 0xB0C2, 0xB1C2, 0x86D0, 0x96D1, 0x91D2,
        0xB5C2, 0xB6C2, 0xB7C2, 0x91D1, 0x9684E2, 0x94D1, 0xBBC2, 0x98D1,
        0x85D0, 0x95D1, 0x97D1, 0x90D0, 0x91D0, 0x92D0, 0x93D0, 0x94D0, 0x95D0,
        0x96D0, 0x97D0, 0x98D0, 0x99D0, 0x9AD0, 0x9BD0, 0x9CD0, 0x9DD0, 0x9ED0,
        0x9FD0, 0xA0D0, 0xA1D0, 0xA2D0, 0xA3D0, 0xA4D0, 0xA5D0, 0xA6D0, 0xA7D0,
        0xA8D0, 0xA9D0, 0xAAD0, 0xABD0, 0xACD0, 0xADD0, 0xAED0, 0xAFD0, 0xB0D0,
        0xB1D0, 0xB2D0, 0xB3D0, 0xB4D0, 0xB5D0, 0xB6D0, 0xB7D0, 0xB8D0, 0xB9D0,
        0xBAD0, 0xBBD0, 0xBCD0, 0xBDD0, 0xBED0, 0xBFD0, 0x80D1, 0x81D1, 0x82D1,
        0x83D1, 0x84D1, 0x85D1, 0x86D1, 0x87D1, 0x88D1, 0x89D1, 0x8AD1, 0x8BD1,
        0x8CD1, 0x8DD1, 0x8ED1, 0x8FD1 };
    while ( *in )
        if ( *in & 0x80 )
            {
            int v = table[ (int)( 0x7f & *in++ ) ];
            if ( !v ) continue;
            *out++ = (char)v;
            *out++ = (char)( v >> 8 );
            if ( v >>= 16 ) *out++ = (char)v;
            }
        else
            *out++ = *in++;
    *out = 0;
    }

int utf8_strlen( const char* str )
    {
    int c, i, ix, q;
    for ( q = 0, i = 0, ix = strlen( str ); i < ix; i++, q++ )
        {
        c = (unsigned char)str[ i ];
        if ( c <= 127 )
            i += 0;
        else if ( ( c & 0xE0 ) == 0xC0 )
            i += 1;
        else if ( ( c & 0xF0 ) == 0xE0 )
            i += 2;
        else if ( ( c & 0xF8 ) == 0xF0 )
            i += 3;
        else
            return 0;//invalid utf8
        }
    return q;
    }