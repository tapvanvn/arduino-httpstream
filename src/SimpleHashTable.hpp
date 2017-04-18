///SimpleHashTable
///author: Duy Nguyen <tapvanvn@gmail.com>
///This template helps making a simple hash table with S of size of type T.

#ifndef _H_SIMPLE_HASH_TABLE
#define _H_SIMPLE_HASH_TABLE

#include <stdint.h>
using namespace std;

template <int8_t S, typename T>
class SimpleHashTable
{
public:
    static int16_t hash(const char* key)
    {
        int16_t index = 0;
        char *ptr = (char*)key;
        do
        {
            index += *ptr % S;
        }
        while(*ptr++ != '\0');

        return index;
    }
	
	//put a object to table with key 
    void put(const char* key, T* val)
    {
        _table[hash(key)] = val;
    }
	
	//get object with key return the object or null if not exist.
    T* get(const char* key)
    {
        return _table[hash(key)];
    }
	
	//clear table without delete objects that contain in it (you must control by yourself).
    void clear()
    {
        for(int8_t i = 0; i< S; i++)
        {
            _table[i] = 0;
        }
    }
	
	//clear table and delete all objects that has been contained in it (so you must be care of deleteing an object twice).
    void deepClear()
    {
        for(int8_t i = 0; i< S; i++)
        {
            if(_table[i])
            {
                delete _table[i];
            }
            _table[i] = 0;
        }
    }
	
protected:
private:
    T *_table[S];
};

#endif
