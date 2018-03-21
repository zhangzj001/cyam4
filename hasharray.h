#ifndef __HASHARRAY_H__
#define __HASHARRAY_H__

#include <stdlib.h>
#include <inttypes.h>
#include <vector>
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>
using namespace std;

typedef uint32_t (*funcHashCode)(const void* key);
typedef int (*funcKeyCompare)(const void* key1, const void* key2);

typedef void (*funcWalkHashArray)(const void* key, const void* data, const void* user_data);

struct array_node_t{
    void* key;
    void* value;
    array_node_t* prev;
    array_node_t* next;
};

class hasharray{
public:
    enum KeyType{
        INT32, INT64, DOUBLE, STRING, DIRECT, OTHER
    };

public:
    hasharray(KeyType type);
    ~hasharray();

public:
    void iget(int idx, void* key, void* value);
    void* get(const void* key);
    void* put(void* key, void* value, bool front = false);
    void* remove(void* key);
    bool contains(void* key);
    void keys(vector<void*>& vec);
    void values(vector<void*>& vec);
    size_t size();

    bool shift(void* key, void* value);
    void unshift(void* key, void* value);
    void push(void* key, void* value);
    bool pop(void* key, void* value);

public:
    void walk(funcWalkHashArray func, const void* user_data);

public:
    void setHashCodeFunc(funcHashCode hashCode){
        this->hashCode = hashCode;
    }
    void setKeyCompareFunc(funcKeyCompare keyCompare){
        this->keyCompare = keyCompare;
    }

private:
    void* table;
    array_node_t* head;
    array_node_t* tail;

    funcHashCode hashCode;
    funcKeyCompare keyCompare;

private:
    //内存池
    boost::object_pool<array_node_t> _pool;
    inline array_node_t* create_node(){
        return _pool.malloc();
    } 
    inline void free_node(array_node_t* node){
        _pool.free(node);
    }
    array_node_t* getNode(const void* key);
};

#endif /* end of include guard: __HASHARRAY_H__ */
