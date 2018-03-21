#include "hasharray.h"
#include <glib.h>
#include <glist.h>

#define PHASH(pointer) ((GHashTable*)(pointer))
#define PNODE(pointer) ((array_node_t*)(pointer))

hasharray::hasharray(KeyType type){
    if(type == INT32){
        hashCode = g_int_hash;
        keyCompare = g_int_equal;
    }
    if(type == INT64){
        hashCode = g_int64_hash;
        keyCompare = g_int64_equal;
    }
    if(type == DOUBLE){
        hashCode = g_double_hash;
        keyCompare = g_double_equal;
    }
    if(type == STRING){
        hashCode = g_str_hash;
        keyCompare = g_str_equal;
    }
    if(type == DIRECT){
        hashCode = g_direct_hash;
        keyCompare = g_direct_equal;
    }

    table = g_hash_table_new_full(hashCode, keyCompare, NULL, NULL);
    head = tail = NULL;
}

hasharray::~hasharray(){
    g_hash_table_destroy(PHASH(table));
}

array_node_t* hasharray::getNode(const void* key){
    array_node_t* node = PNODE(g_hash_table_lookup(PHASH(table), key));
    return node;
}

void hasharray::iget(int idx, void* key, void* value){
    array_node_t* node = head;
    while(idx-- > 0 && node != NULL){
        node = node->next;
    }
    if(node != NULL){
        key = node->key;
        value = node->value;
    }
}

void* hasharray::get(const void* key){
    array_node_t* node = getNode(key);
    if(node == NULL) return NULL;
    return node->value;
}

void* hasharray::put(void* key, void* value, bool front){
    array_node_t* node = getNode(key);
    if(node != NULL){
        void* old_value = node->value;
        node->value = value;
        return old_value;
    }

    node = create_node();
    node->key = key;
    node->value = value;
    node->prev = node->next = NULL;

    //初始化的情况，会有 prev=tail=NULL
    if(tail == NULL){
        head = tail = node;
    }else{
        if(front){
            //从前面插入，呵呵
            node->next = head;
            head->prev = node;
            head = node;
        }else{
            //从后面插入，呵呵
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        g_hash_table_insert(PHASH(table), key, node);
    }
}

void* hasharray::remove(void* key){
    array_node_t* node = getNode(key);

    if(node != NULL){
        void* value = node->value;
        g_hash_table_remove(PHASH(table), key);
        free_node(node);
        return value;
    }
    return NULL;
}

bool hasharray::contains(void* key){
    return g_hash_table_contains(PHASH(table), key)!=0;
}

static void copyKeys(gpointer data, gpointer user_data){
    vector<void*>& vec = (*(vector<void*>*)user_data);
    vec.push_back(data);
}
void hasharray::keys(vector<void*>& vec){
    GList* list = g_hash_table_get_keys(PHASH(table));
    g_list_foreach(list, copyKeys, &vec);
}

static void copyValues(gpointer data, gpointer user_data){
    vector<void*>& vec = (*(vector<void*>*)user_data);
    array_node_t* node = PNODE(data);
    vec.push_back(node->value);
}
void hasharray::values(vector<void*>& vec){
    GList* list = g_hash_table_get_values(PHASH(table));
    g_list_foreach(list, copyValues, &vec);
}

size_t hasharray::size(){
    return g_hash_table_size(PHASH(table));
}

bool hasharray::shift(void* key, void* value){
    if(head == NULL)
        return false;

    key = head->key;
    value = head->value;

    head = head->next;
    if(head != NULL)
        head->prev = NULL;

    remove(key);
    return true;
}

void hasharray::unshift(void* key, void* value){
    put(key, value, true);
}

void hasharray::push(void* key, void* value){
    put(key, value, false);
}

bool hasharray::pop(void* key, void* value){
    if(tail == NULL)
        return false;

    key = tail->key;
    value = tail->value;

    tail = tail->prev;
    if(tail != NULL)
        tail->next = NULL;

    remove(key);
    return true;
}

void hasharray::walk(funcWalkHashArray func, const void* user_data){
    array_node_t* node = head;
    while(node != NULL){
        func(node->key, node->value, user_data);
        node = node->next;
    }
}

