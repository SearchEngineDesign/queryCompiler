#pragma once

// HashBlob, a serialization of a HashTable into one contiguous
// block of memory, possibly memory-mapped to a HashFile.

// Nicole Hamilton  nham@umich.edu

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <cstdint>
#include <stdlib.h> //instead of #include <malloc.h> DZ
#include <unistd.h>
#include <sys/mman.h>

#include "HashTable.h"
#include "string.h"
#include "vector.h"
#include "../index/index.h"

using Hash = HashTable< string, PostingList >;
using Pair = Tuple< string, PostingList >;
using HashBucket = Bucket< string, PostingList >;


static const size_t Unknown = 0;


size_t RoundUp( size_t length, size_t boundary )
   {
   // Round up to the next multiple of the boundary, which
   // must be a power of 2.

   static const size_t oneless = boundary - 1,
      mask = ~( oneless );
   return ( length + oneless ) & mask;
   }

struct SerialString
   {
   public:
      size_t size, capacity;
      char data[ Unknown ];

      static size_t BytesRequired(const string &str) 
         {
            // for size and capacity
            size_t size = sizeof(size_t) << 1;
            // for chars
            size += str.size() * sizeof(char);

            return RoundUp(size, sizeof(size_t));
         }

      static char *Write( char *buffer, size_t len,
         const string &str ) {
            SerialString* t = reinterpret_cast<SerialString*>(buffer);
            t->size = str.size();
            t->capacity = str.capacity();
            for ( size_t i = 0; i < str.size(); i++ )
               t->data[i] = *(str.at(i));
         }

   };

struct SerialVector
   {
   public:
      size_t size, capacity;

      Post data[ Unknown ];

      static size_t BytesRequired(const vector<Post> * vec) 
         {
            // for size and capacity
            size_t size = sizeof(size_t) << 1;
            // for each post -- but posts are variable length
            for (auto &i : *vec) {
               size += i.length();
            }
            
            return RoundUp(size, sizeof(size_t));
         }

      static char *Write( char *buffer, size_t len,
         const vector<Post> &vec ) {
            SerialVector* t = reinterpret_cast<SerialVector*>(buffer);
            t->size = vec.size();
            t->capacity = vec.capacity();
            for ( size_t i = 0; i < vec.size(); i++ )
               memcpy(&(t->data[i]), &vec[i], vec[i].length());
         }
         
   };

struct SerialPostingList
   {
   public:

      size_t lastPos, lastDoc, documentCount;
      char type;
      SerialString token;
      SerialVector list;
      uint8_t seekIndex;
      std::pair<size_t, size_t> SeekTable[256];

      static size_t BytesRequired(const PostingList * p) 
         {
            size_t size = 0;
            // the 3 size_t member variables -- 
            size += sizeof(size_t) * 3;
            // token type
            size += sizeof(char);
            // token
            size += SerialString::BytesRequired(p->getIndex());
            // post vector
            size += SerialVector::BytesRequired(p->getList());
            // the uint8_t member variable -- seek index
            size += sizeof(uint8_t);
            // the seek list
            size += sizeof(std::pair<size_t, size_t>) >> 8;
            
            return RoundUp(size, sizeof(size_t));
         }
         

      static char *Write( char *buffer, size_t len,
         const PostingList *p ) {
            size_t offset = 0;
            SerialPostingList* t = reinterpret_cast<SerialPostingList*>(buffer);

            t->lastPos = p->lastPos;
            t->lastDoc = p->lastDoc;
            t->documentCount = p->getDocCount();
            t->type = p->getType();

            offset += sizeof(size_t) * 3;
            offset += sizeof(char);

            size_t tokenSize = SerialString::BytesRequired(p->getIndex());
            SerialString::Write(buffer + offset, tokenSize, p->getIndex());
            offset += tokenSize;

            size_t vectorSize = SerialVector::BytesRequired(p->getList());
            SerialVector::Write(buffer + offset, vectorSize, *(p->getList()));
            offset += vectorSize;

            t->seekIndex = p->getSeekIndex();

            const std::pair<size_t, size_t> *table = p->getSeekTable();

            for (int i = 0; i < 256; i++)
               t->SeekTable[i] = (*table)[i];
         }
   };

struct SerialTuple
   {

   public:

      // The Key will be a C-string of whatever length.
      SerialString Key;

      SerialPostingList Value;

      // Calculate the bytes required to encode a HashBucket as a
      // SerialTuple.

      static size_t BytesRequired( const HashBucket *b )
         {
            size_t size = 0;
            // string key
            size += SerialString::BytesRequired(b->tuple.key);

            // PL value
            size += SerialPostingList::BytesRequired(&(b->tuple.value));

            
            return RoundUp(size, sizeof(size_t));
         }

      // Write the HashBucket out as a SerialTuple in the buffer,
      // returning a pointer to one past the last character written.

      static char *Write( char *buffer, size_t len,
            const HashBucket *b )
         {
         size_t offset = 0;
         SerialTuple* t = reinterpret_cast<SerialTuple*>(buffer);

         size_t keySize = SerialString::BytesRequired(b->tuple.key);
         SerialString::Write(buffer + offset, keySize, b->tuple.key);
         offset += keySize;
         
         size_t valueSize = SerialPostingList::BytesRequired(&(b->tuple.value));
         SerialPostingList::Write(buffer + offset, valueSize, &(b->tuple.value));
         offset += valueSize;

         return (buffer + len + 1);
         }
  };


class HashBlob
   {
   // This will be a hash specifically designed to hold an
   // entire hash table as a single contiguous blob of bytes.
   // Pointers are disallowed so that the blob can be
   // relocated to anywhere in memory

   // The basic structure should consist of some header
   // information including the number of buckets and other
   // details followed by a concatenated list of all the
   // individual lists of tuples within each bucket.

   public:

      // Define a MagicNumber and Version so you can validate
      // a HashBlob really is one of your HashBlobs.

      size_t keyCount,
         BlobSize,
         NumberOfBuckets,
         Buckets[ Unknown ];

      // The SerialTuples will follow immediately after.


      const SerialTuple *Find( const char *key ) const
         {
         // Search for the key k and return a pointer to the
         // ( key, value ) entry.  If the key is not found,
         // return nullptr.

         // Your code here.
         size_t index = Hash::hashbasic(key, NumberOfBuckets);
         size_t bucketStart = Buckets[index];
         SerialTuple *curr = reinterpret_cast<SerialTuple*>((char *)this + bucketStart);

         size_t bucketEnd;
         (index == NumberOfBuckets - 1) ? bucketEnd = BlobSize : bucketEnd = Buckets[index+1];

         while (bucketStart < bucketEnd)
         {
            if (!strcmp(curr->Key, key))
            {
               return curr;
            } else {
               bucketStart += curr->Length;
               curr = reinterpret_cast<SerialTuple*>((char *)this + bucketStart);
            }
            
         }

         return nullptr;
         }


      static size_t BytesRequired( const Hash *hashTable, size_t &offset )
         {
         // Calculate how much space it will take to
         // represent a HashTable as a HashBlob.

         //space for 2048 offsets of size size_t + the 3 hashblob members
         offset = (hashTable->size()+3) * sizeof(size_t);
         size_t bucketSpace = 0;
         for (int i = 0; i < hashTable->size(); i++)
            {
               HashBucket *curr = hashTable->at(i);
               while (curr != nullptr)
               {
                  // add the size of a bucket
                  bucketSpace += SerialTuple::BytesRequired(curr);
                  curr = curr->next;
               }
            }

         return offset + bucketSpace;
         }

      // Write a HashBlob into a buffer, returning a
      // pointer to the blob.

      static HashBlob *Write( HashBlob *hb, size_t bytes,
            const Hash *hashTable )
         {
         // Your code here.
         // What does this do ???

         return nullptr;
         }

      // Create allocates memory for a HashBlob of required size
      // and then converts the HashTable into a HashBlob.
      // Caller is responsible for discarding when done.

      // (No easy way to override the new operator to create a
      // variable sized object.)

      static HashBlob *Create( const Hash *hashTable )
         {
         //allocating the whole blob
         size_t offset = 0;
         size_t size = HashBlob::BytesRequired( hashTable, offset );
         char *mem = new char[size];
         HashBlob *blob = reinterpret_cast<HashBlob*>(mem);

         blob->BlobSize = size;
         blob->NumberOfBuckets = hashTable->size();

         for (int i = 0; i < hashTable->size(); i++)
         {
            blob->Buckets[i] = offset;
            HashBucket *curr = hashTable->at(i);
            
            while (curr != nullptr) {
               size_t tSize = SerialTuple::BytesRequired(curr);
               SerialTuple::Write(mem + offset, tSize, curr);
               offset += tSize;
               curr = curr->next;
            }
            
         }
         
         return blob;
         }

      // Discard

      static void Discard( HashBlob *blob )
         {
         // Your code here.
         }
   };

class HashFile
   {
   private:

      int fd;
      HashBlob *blob;
      struct stat fileInfo;

      size_t FileSize( int f )
         {
         fstat( f, &fileInfo );
         return fileInfo.st_size;
         }

   public:

      const HashBlob *Blob( )
         {
         return blob;
         }

      HashFile( const char *filename )
         {
            // Open the file for reading, map it, check the header,
            // and note the blob address.

            // Your code here.
            fd = open(filename, O_RDONLY); //open file
            if (fd == -1) 
               perror("open");

            if (FileSize(fd) == -1) //get file size
               perror("fstat");

            blob = reinterpret_cast<HashBlob*>(mmap(nullptr, fileInfo.st_size, 
                                       PROT_READ, MAP_PRIVATE, fd, 0)); //map bytes to 'blob'
            if (blob == MAP_FAILED)
               perror("mmap");

         }

      HashFile( const char *filename, const Hash *hashtable )
         {
         // Open the file for write, map it, write
         // the hashtable out as a HashBlob, and note
         // the blob address.

         // Your code here.
         }

      ~HashFile( )
         {
            munmap(blob, fileInfo.st_size);  //not sure if this should happen here or earlier
            close(fd);                       //not sure if this should happen here or earlier
         }
   };