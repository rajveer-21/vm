#ifndef memory_h
#define memory_h
#include "common.h"
#define GROW_CAPACITY(capacity)((capacity) < 8 ? 8 : capacity * 2)
#define GROW_DYNAMIC_ARRAY(type, array, old_size, new_size)((type*)reallocate(array, sizeof(type) * old_size, sizeof(type) * new_size))
#define FREE_DYNAMIC_ARRAY(type, array, old_size)(reallocate(array, sizeof(type) * old_size, 0))
void* reallocate(void* array, size_t old_size, size_t new_size);
void* reallocate(void* array, size_t old_size, size_t new_size)
{
    if(new_size == 0)
    {
        free(array);
        return NULL;
    }
    void* new_array = realloc(array, new_size);
    if(new_array == NULL)
    exit(1);
    return new_array;
}
#endif