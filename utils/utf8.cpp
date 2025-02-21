// Utf8.cpp

// This file implements Utf8 library functions.


#include "Utf8.h"
#include <iostream>
size_t SizeOfUtf8( Unicode c )
   {
   if ( c <= 0x7F )
      return 1;  
   else if ( c <= 0x7FF )
      return 2;  
   else if ( c <= 0xFFFF ) 
      return 3;  
   else
      return 0;  
   }

size_t IndicatedLength( const Utf8 *p )
   {
   if ( ( *p & 0x80 ) == 0x00 )
      return 1;  
   else if ( ( *p & 0xE0 ) == 0xC0 )
      return 2;  
   else if ( ( *p & 0xF0 ) == 0xE0 )
      return 3;  
   else if ( ( *p & 0xF8 ) == 0xF0 )
      return 4;  
   else if ( ( *p & 0xFC ) == 0xF8 )
      return 5;  
   else if ( ( *p & 0xFE ) == 0xFC )
      return 6;  
   return 1;  
   }

Unicode GetUtf8( const Utf8 *p ) {
   size_t indicatedLength = IndicatedLength(p);  
   
   if ( indicatedLength == 1 )
      {
      if ( ( *p & 0x80 ) == 0x00 )
         return *p;  // ASCII, 1 byte
      else
        return ReplacementCharacter;  
      }

   if ( indicatedLength == 2 ) 
      {
      if ( *p == 0xc0 || *p == 0xc1 )
         return ReplacementCharacter;  // Overlong sequence
      if ( ( *( p + 1 ) & 0xC0 ) == 0x80 )
         return ( ( *p & 0x1F ) << 6 ) | ( *( p + 1 ) & 0x3F );  
      }

   if ( indicatedLength == 3 ) 
      {
      if ( *p == 0xe0 && *( p + 1 ) <= 0x9f )
         return ReplacementCharacter;  // Overlong sequence
      if ( ( *( p + 1 ) & 0xC0 ) == 0x80 && ( *( p + 2 ) & 0xC0 ) == 0x80 )
         return ( ( *p & 0x0F ) << 12 ) | 
            ( ( *( p + 1 ) & 0x3F ) << 6) | 
            ( *( p + 2 ) & 0x3F );  
      }

   return ReplacementCharacter;
   }

Unicode GetUtf8( const Utf8 *p, const Utf8 *bound )
   {
   size_t indicatedLength = IndicatedLength(p);  
    // Single byte (ASCII)
   if ( bound != nullptr && *bound && p + indicatedLength >= bound )
      return ReplacementCharacter;  
   return GetUtf8(p);  
   }

const Utf8 *NextUtf8( const Utf8 *p, const Utf8 *bound )
   {
   size_t indicatedLength = IndicatedLength(p);  

   // ASCII or invalid first byte
   if ( indicatedLength == 1 )
      return p + 1;  

   for ( int i = 1; i < indicatedLength; i ++ )
      {
      // invalid continuation byte
      if ( ( *( p + i ) & 0xC0 ) != 0x80 )
         return p + i;  
      // stop at the bound
      if ( bound != nullptr && *bound && ( p + i >= bound ) )
         return bound;  
      }

   return p + indicatedLength;  
   }

const Utf8 *PreviousUtf8( const Utf8 *p )
   {
   p --;  

   // while continuation byte
   while ( (*p & 0xC0 ) == 0x80 )
      p --;  

   return p;  
   }

Utf8 *WriteUtf8( Utf8 *p, Unicode c )
   {
   // one byte utf8
   if ( c <= 0x7F )
      {
      *p = c;  
      return p + 1;  
      }
   // two bytes
   else if ( c <= 0x7FF )
      {
      *p = ( c >> 6 ) | 0xC0;  
      *( p + 1 ) = ( c & 0x3F ) | 0x80;  
      return p + 2;  
      }
   // three bytes
   else if ( c <= 0xFFFF )
      {
      *p = ( c >> 12 ) | 0xE0;  
      *( p + 1 ) = ( ( c >> 6 ) & 0x3F ) | 0x80;  
      *( p + 2 ) = ( c & 0x3F ) | 0x80;  
      return p + 3;  
      }
   }


// Not part of hw

int StringCompare( const Utf8 *a, const Utf8 *b ) {}

// Unicode string compare up to 'N' UTF-8 characters (not bytes)
// from two UTF-8 strings.

int StringCompare( const Utf8 *a, const Utf8 *b, size_t N ) {}

// Case-independent compares.

int StringCompareI( const Utf8 *a, const Utf8 *b ) {}
int StringCompareI( const Utf8 *a, const Utf8 *b, size_t N ) {}

Unicode ToLower( Unicode c ) {}

bool IsPunctuation( Unicode c ) {}
bool IsSpace( Unicode c ) {}
bool IsControl( Unicode c ) {}

