#ifndef value_h
#define value_h 
#include "common.h"
#include "memory.h"
#define BOOL_VAL(value) ((VALUE){VAL_BOOL, {.boolean = value}})
#define NIL_VAL() ((VALUE){VAL_NIL, {.number = 0}})
#define NUM_VAL(value) ((VALUE){VAL_NUM, {.number = (double)(value)}})
#define OBJ_VAL(object) ((VALUE){VAL_OBJ, {.obj = OBJ*(object)}})
#define GET_BOOL(value) ((value).as.boolean)
#define GET_NUM(value) ((value).as.number)
#define GET_OBJ(value) ((value).as.obj)
#define GET_STRING(value) ((OBJ_STRING)* GET_OBJ(value))
#define GET_CSTRING(value) ((OBJ_STRING)* GET_OBJ(value)->chars)
#define IS_BOOL(value) ((value).valuetype == VAL_BOOL)
#define IS_NIL(value)  ((value).valuetype == VAL_NIL)
#define IS_NUM(value)  ((value).valuetype == VAL_NUM)
#define IS_OBJ(value)  ((value).valuetype = VAL_OBJ)
#define IS_STRING(value) isObjectType(value, OBJ_STRING)
#define OBJ_TYPE(value) (GET_OBJ(value)->type)
#define ALLOCATE(type, size) ((type*)reallocate(NULL, 0, sizeof(type) * size))
#define ALLOCATE_OBJ(type, objectType) (type*)(allocateObject(sizeof(type), objectType))
typedef struct OBJ OBJ;
typedef enum 
{
    OBJ_STRING,
}ObjectType;
struct OBJ
{
    ObjectType objecttype;
};
typedef struct OBJString OBJString;
struct OBJString
{
    OBJ obj;
    int length;
    char* string;
};
typedef enum 
{
    VAL_BOOL,
    VAL_NIL,
    VAL_NUM,
    VAL_OBJ,
}ValueType;
typedef struct 
{
    ValueType valuetype;
    union 
    {
        bool boolean;
        double number;
        OBJ* obj;
    }as;
}VALUE;
typedef struct 
{
    int occupied;
    int capacity;
    VALUE* stuff;
}ValueArray;
void initializeValArr(ValueArray* array);
void writeToValArr(ValueArray* array, VALUE current);
void freeUpValArr(ValueArray* array);
inline bool isObjectType(VALUE value, ObjectType type);
OBJString* copyString(const char* chars, int length);
void initializeValArr(ValueArray* array)
{
    array->occupied = 0;
    array->capacity = 0;
    array->stuff = NULL;
}
void writeToValArr(ValueArray* array, VALUE value)
{
    if(array->occupied + 1 > array->capacity)
    {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->stuff = GROW_DYNAMIC_ARRAY(VALUE, array->stuff, oldCapacity, array->capacity);
    }
    array->stuff[array->occupied] = value;
    array->occupied = array->occupied + 1;
}
void freeUpValArr(ValueArray* array)
{
    FREE_DYNAMIC_ARRAY(VALUE, array->stuff, array->capacity);
    initializeValArr(array);
}
inline bool isObjectType(VALUE value, ObjectType type)
{
    if(IS_OBJ(value) && GET_OBJ(value)->objecttype == type)
    return true;
    return false;
}
OBJString* copyString(const char* chars, int length)
{
    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(heapChars, length);
}
OBJString* allocateString(char* chars, int length)
{
    OBJString* created_string = ALLOCATE_OBJ(OBJString, OBJ_STRING);
    created_string->length = length;
    created_string->string = chars;
    return created_string;
}
OBJ* allocateObject(size_t size, ObjectType type) 
{
  OBJ* object = (OBJ*)reallocate(NULL, 0, size);
  object->objecttype = type;
  return object;
}
#endif