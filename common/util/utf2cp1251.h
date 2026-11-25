#pragma once

#include <cstddef>

int convert_utf8_to_windows1251(
    const char* utf8, char* windows1251, size_t n, size_t buff_size );

void convert_windows1251_to_utf8( char* out, const char* in );

int utf8_strlen( const char* str );
