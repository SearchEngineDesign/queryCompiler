// string.h
// 
// Starter file for a string template

#pragma once
#include <cstddef>   // for size_t
#include <iostream>  // for ostream

class string
   {
   public:  

      // Default Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Creates an empty string
      string( ) : m_size( 0 ), m_capacity( 1 ), m_data( new char[ 1 ] )
         {
            m_data[0] = '\0';
         }

      // string Literal / C string Constructor
      // REQUIRES: cstr is a null terminated C style string
      // MODIFIES: *this
      // EFFECTS: Creates a string with equivalent contents to cstr
      string( const char *cstr )
         {
            if ( cstr )
            {
               size_t length = 0;
               while ( cstr[length] != '\0' )
               {
                  ++length;
               }
               m_size = length;
               m_capacity = m_size + 1;
               m_data = new char[m_capacity];
               for ( size_t i = 0; i < m_size; ++i ) 
               {
                  m_data[i] = cstr[i];
               }
            }
            else
            {
               m_size = 0;
               m_capacity = 1;
               m_data = new char[1];
               m_data[0] = '\0';
            }
         }

      // Size
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns the number of characters in the string
      size_t size( ) const
         {
            return m_size;
         }

      // C string Conversion
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a pointer to a null terminated C string of *this
      const char *cstr( ) const
         {
            return m_data;
         }

      // Iterator Begin
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a random access iterator to the start of the string
      const char *begin( ) const
         {
            return m_data;
         }

      // Iterator End
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a random access iterator to the end of the string
      const char *end( ) const
         {
            return m_data + m_size;
         }

      // Element Access
      // REQUIRES: 0 <= i < size()
      // MODIFIES: Allows modification of the i'th element
      // EFFECTS: Returns the i'th character of the string
      char &operator [ ]( size_t i )
         {
            // assert ( i < m_size );
            return m_data[i];
         }

      // string Append
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Appends the contents of other to *this, resizing any
      //      memory at most once
      void operator+=( const string &other )
         {
            if ( m_size + other.m_size >= m_capacity )
            {
               m_capacity = m_size + other.m_size + 1;
               char* new_data = new char[m_capacity];
               for ( size_t i = 0; i < m_size; ++i )
               {
                  new_data[i] = m_data[i];
               }
               delete[] m_data;
               m_data = new_data;
            }
            for ( size_t i = 0; i < other.m_size; ++i )  
            {
               m_data[m_size + i] = other.m_data[i];
            }
            m_size += other.m_size;
            m_data[m_size] = '\0';
         }

      // Push Back
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Appends c to the string
      void pushBack( char c )
         {
            if ( m_size == m_capacity - 1 )
            {
               m_capacity *= 2;
               char* new_data = new char[m_capacity];
               for ( size_t i = 0; i < m_size; ++i )
               {
                     new_data[i] = m_data[i];
               }
               delete[] m_data;
               m_data = new_data;
            }
            m_data[m_size++] = c;
            m_data[m_size] = '\0';
         }

      // Pop Back
      // REQUIRES: string is not empty
      // MODIFIES: *this
      // EFFECTS: Removes the last charater of the string
      void popBack( )
         {
            if ( m_size > 0 )
            {
               m_data[--m_size] = '\0';
            }
         }

      // Equality Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether all the contents of *this
      //    and other are equal
      bool operator==( const string &other ) const
         {
            if ( other.m_size != m_size )
            {
               return false;
            }
            for ( size_t i = 0; i < m_size; ++i )
            {
               if ( m_data[i] != other.m_data[i] )
               {
                  return false;
               }
            }
            return true; 
         }

      // Not-Equality Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether at least one character differs between
      //    *this and other
      bool operator!=( const string &other ) const
         {
            return !( *this == other );
         }

      // Less Than Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether *this is lexigraphically less than other
      bool operator<( const string &other ) const
         {
            size_t i1 = 0;
            size_t i2 = 0;

            while ( i1 < m_size and i2 < other.m_size )
            {
               if ( m_data[i1] < other.m_data[i2] )
               {
                  return true;
               }
               else if ( m_data[i1] > other.m_data[i2] )
               {
                  return false;
               }
               ++ i1;
               ++ i2;
            }
            return ( i1 == m_size ) && ( i2 < other.m_size );
         }

      // Greater Than Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether *this is lexigraphically greater than other
      bool operator>( const string &other ) const
         {
            return !( *this <= other );
         }

      // Less Than Or Equal Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether *this is lexigraphically less or equal to other
      bool operator<=( const string &other ) const
         {
            return ( *this < other ) || ( *this == other );
         }

      // Greater Than Or Equal Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether *this is lexigraphically less or equal to other
      bool operator>=( const string &other ) const
         {
            return ( *this == other ) || ( *this > other );
         }

   private:
      size_t m_size;
      size_t m_capacity;
      char *m_data;
   };

std::ostream &operator<<( std::ostream &os, const string &s )
   {
      os << s.cstr();
      return os;
   }
