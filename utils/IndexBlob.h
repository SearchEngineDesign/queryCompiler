#pragma once

#ifndef INDEXBLOB_H
#define INDEXBLOB_H

// IndexBlob, a serialization of a HashTable into one contiguous
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

using Hash = HashTable< string, PostingList >;
using Pair = Tuple< string, PostingList >;
using HashBucket = Bucket< string, PostingList >;

class Index;
class PostingList;
class Post;

static const size_t Unknown = 0;


size_t RoundUp( size_t length, size_t boundary )
   {
   // Round up to the next multiple of the boundary, which
   // must be a power of 2.

   static const size_t oneless = boundary - 1,
      mask = ~( oneless );
   return ( length + oneless ) & mask;
   }

struct SerialPost 
   {
   char data[ Unknown ];
   };

struct SerialString
   {
   public:
      uint8_t delimiter = 0;
      char data[ Unknown ];

      static size_t BytesRequired(const string &str) 
         {
            // for chars
            size_t size = (str.size() * sizeof(char)) + sizeof(uint8_t);

            return RoundUp(size, sizeof(size_t));
         }

      static char *Write( char *buffer, const string *str ) {
            SerialString* t = reinterpret_cast<SerialString*>(buffer);
            for ( size_t i = 0; i < str->size(); i++ )
               t->data[i] = *(str->at(i));
         }

      const char *c_str() const {
         return data;
      }

   };

struct SerialDocumentVector
   {
   public:
      SerialString data[ Unknown ];

      static size_t BytesRequired(const vector<string> * vec) 
         {
            size_t size = 0;
            // for each post
            for (auto &i : *vec) {
               size += i.size();
            }
            
            return RoundUp(size, sizeof(size_t));
         }
         
   };

struct SerialPostingList
   {
   public:

      size_t documentCount, posts;
      char type;
      uint8_t seekIndex;
      // byte offsets to seek pairs + each individual post 
      size_t offsets[ Unknown + Unknown ];

      static size_t BytesRequired(const PostingList * p) 
         {
            vector<Post> list = *(p->getList());

            size_t size = 0;
            // the 2 size_t member variables
            size += sizeof(size_t) << 1;
            // token type
            size += sizeof(char);
            // the uint8_t member variable -- seek index
            size += sizeof(uint8_t);
            size = RoundUp(size, sizeof(size_t));
            // the list of seek offsets
            size += sizeof(size_t) * p->getSeekIndex();
            // list of post offsets
            size += sizeof(size_t) * list.size();

            // seek list size
            size += (sizeof(size_t) << 1) * p->getSeekIndex();

            // post vector size
            for (int i = 0; i < list.size(); i++) {
               size += list[i].length();
            }
            
            
            return RoundUp(size, sizeof(size_t));
         }
         

      static char *Write( char *buffer, size_t len,
         const PostingList *p ) {
            size_t offset = 0;
            SerialPostingList* t = reinterpret_cast<SerialPostingList*>(buffer);
            vector<Post> listIn = *(p->getList());

            t->documentCount = p->getDocCount();
            t->posts = listIn.size();
            t->type = p->getType();

            t->seekIndex = p->getSeekIndex();

            offset += sizeof(size_t) << 1;
            offset += sizeof(char);
            offset += sizeof(uint8_t);
            offset = RoundUp(offset, sizeof(size_t));
            offset += (sizeof(size_t)) * p->getSeekIndex();
            offset += (sizeof(size_t)) * listIn.size();

            const std::pair<size_t, size_t> *seekTable = p->getSeekTable();
            size_t increment = sizeof(size_t) << 1;
            for (int i = 0; i < p->getSeekIndex(); i++) {
               memcpy(buffer + offset, &(seekTable[i]), increment);
               t->offsets[i] = offset;
               offset += increment;
            }

            offset = RoundUp(offset, sizeof(size_t));
            uint8_t postSize = 0;
            Post post;

            for (int i = 0; i < listIn.size(); i++) {
               postSize = listIn[i].length();
               memcpy(buffer + offset, listIn[i].getData(), postSize);
               t->offsets[i + t->seekIndex] = offset;
               offset += postSize;
            }
               
         }

         const SerialPost *getPost( size_t i ) const {
            if (i >= posts)
               return nullptr;
            return reinterpret_cast<SerialPost*>((char*)this + offsets[i + seekIndex]);
         }

         const std::pair<size_t, size_t> *getSeekPair( size_t i ) const {
            if (i >= seekIndex)
               return nullptr;
            return reinterpret_cast<std::pair<size_t, size_t>*>((char*)this + offsets[i]);
         }
   };

struct SerialTuple
   {

   public:

      // Total size, starting point of value
      size_t size, valueOffset;

      // Calculate the bytes required to encode a HashBucket as a
      // SerialTuple.

      static size_t BytesRequired( const HashBucket *b )
         {
            size_t size = 0;

            // size_t size, valueOffset
            size += sizeof(size_t) << 1;

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
         SerialTuple* t = reinterpret_cast<SerialTuple*>(buffer);
         size_t keySize = SerialString::BytesRequired(b->tuple.key);
         size_t offset = sizeof(size_t) << 1;

         // writing the key (string)
         SerialString::Write(buffer + offset, &b->tuple.key);
         offset += keySize;
         offset = RoundUp(offset, sizeof(size_t));
         t->valueOffset = offset;
         
         // writing the value (posting list)
         size_t valueSize = SerialPostingList::BytesRequired(&(b->tuple.value));
         SerialPostingList::Write(buffer + offset, valueSize, &(b->tuple.value));
         offset += valueSize;
         offset = RoundUp(offset, sizeof(size_t));

         // finally, write the size
         t->size = offset;

         }

      const size_t getSize() {
         return size;
      }

      const SerialString* Key() const {
         return reinterpret_cast<SerialString*>((char *)this + (sizeof(size_t) << 1));
      }

      const SerialPostingList* Value() const {
         return reinterpret_cast<SerialPostingList*>((char *)this + valueOffset);
      }
  };


class IndexBlob
   {

   public:

      size_t 
         BlobSize, // size of blob
         WordsInIndex, // mv of index
         DocumentsInIndex, // mv of index
         keyCount, // mv of dict
         NumberOfBuckets; // mv of dict

      size_t offsets[ Unknown + Unknown ]; // arr of byte offsets to documents and buckets


      // Returns the bucket in dict with token == key.
      // If there is no matching key, returns null.
      const SerialTuple *Find( const char *key ) const
         {
         // Search for the key k and return a pointer to the
         // ( key, value ) entry.  If the key is not found,
         // return nullptr.

         // Your code here.
         size_t i = Hash::hashbasic(key, NumberOfBuckets) + DocumentsInIndex;
         size_t bucketStart = offsets[i];
         SerialTuple *curr = reinterpret_cast<SerialTuple*>((char *)this + bucketStart);

         size_t bucketEnd;
         (i == NumberOfBuckets - 1) ? bucketEnd = BlobSize : bucketEnd = offsets[i+1];

         while (bucketStart < bucketEnd)
         {
            if (!strcmp(curr->Key()->c_str(), key))
            {
               return curr;
            } else {
               bucketStart += curr->getSize();
               curr = reinterpret_cast<SerialTuple*>((char *)this + bucketStart);
            }
            
         }

         return nullptr; 
         }

      // Returns a SerialString of the document name in the document table with index i. 
      // If there is no index i, returns null.
      const SerialString *getDocument( size_t i ) {
         if (DocumentsInIndex <= i)
            return nullptr;
         return reinterpret_cast<SerialString*>((char *)this + offsets[i]);
      }

      static size_t BytesRequired( const Hash *hashTable )
         {
         // Calculate how much space it will take to
         // represent a HashTable as a IndexBlob.

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

         return bucketSpace;
         }


      // Create allocates memory for a IndexBlob of required size
      // and then converts the HashTable into a IndexBlob.
      // Caller is responsible for discarding when done.

      // (No easy way to override the new operator to create a
      // variable sized object.)

      static IndexBlob *Create( Index *index )
         {
         const Hash *hashTable = index->getDict();
         const vector<string> *documents = index->getDocuments();

         size_t offset = 0;
         // space for the 5 blob size_t members
         size_t size = 5 * sizeof(size_t);
         // space to store the document offset array
         size += documents->size() * sizeof(size_t);
         // space to store the bucket offset array
         size += hashTable->size() * sizeof(size_t);

         // set offset as the starting point for writing
         offset = size;

         // space for the document table
         size += SerialDocumentVector::BytesRequired(documents);
         // space for the dict
         size += IndexBlob::BytesRequired( hashTable );
         
         // allocating memory
         char *mem = new char[size];
         IndexBlob *blob = reinterpret_cast<IndexBlob*>(mem);

         // assigning mvs
         blob->BlobSize = size;
         blob->WordsInIndex = index->WordsInIndex;
         blob->DocumentsInIndex = index->DocumentsInIndex;
         blob->keyCount = hashTable->keyCount;
         blob->NumberOfBuckets = hashTable->size();

         // writing the document vector
         for (int i = 0; i < documents->size(); i++)
         {
            blob->offsets[i] = offset;
            const string * curr = &(documents->operator[](i));
            size_t sSize = SerialString::BytesRequired(*curr);
            SerialString::Write(mem + offset, curr);
            offset += sSize;
            offset = RoundUp(offset, sizeof(size_t));
         }

         for (int i = 0; i < hashTable->size(); i++)
         {
            blob->offsets[i + blob->DocumentsInIndex] = offset;
            HashBucket *curr = hashTable->at(i);
            
            // writing the buckets
            while (curr != nullptr) {
               size_t tSize = SerialTuple::BytesRequired(curr);
               SerialTuple::Write(mem + offset, tSize, curr);
               offset += tSize;
               offset = RoundUp(offset, sizeof(size_t));
               curr = curr->next;
            }
            
         }
                  
         return blob;
         }

      // Discard

      static void Discard( IndexBlob *blob )
         {
         delete blob;
         }
   };



class HashFile
   {
   private:

      int fd;
      IndexBlob *blob;
      struct stat fileInfo;

      size_t FileSize( int f )
         {
         fstat( f, &fileInfo );
         return fileInfo.st_size;
         }

   public:

      const IndexBlob *Blob( )
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

            blob = reinterpret_cast<IndexBlob*>(mmap(nullptr, fileInfo.st_size, 
                                       PROT_READ, MAP_PRIVATE, fd, 0)); //map bytes to 'blob'
            if (blob == MAP_FAILED)
               perror("mmap");

         }

      HashFile( const char *filename, const Hash *hashtable )
         {
         // Open the file for write, map it, write
         // the hashtable out as a IndexBlob, and note
         // the blob address.

         // Your code here.
         }

      ~HashFile( )
         {
            munmap(blob, fileInfo.st_size);  //not sure if this should happen here or earlier
            close(fd);                       //not sure if this should happen here or earlier
         }
   };

   

#endif