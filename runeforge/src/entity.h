#ifndef ENTITY_H
#define ENTITY_H
#include "runewall.h"

#ifndef RUNEFORGE_API
#ifdef _WIN32
    #ifdef RUNEFORGE_BUILD_DLL
        #define RUNEFORGE_API __declspec(dllexport)
    #else
        #define RUNEFORGE_API __declspec(dllimport)
    #endif
#else
    #define RUNEFORGE_API __attribute__((visibility("default")))
#endif
#endif

typedef struct entity entity;
typedef enum {
    PROPERTY_TYPE_INT,
    PROPERTY_TYPE_FLOAT,
    PROPERTY_TYPE_STRING,
    PROPERTY_TYPE_DOUBLE,
    PROPERTY_TYPE_SIZET
}property_types;
typedef  struct property_info{
    const char* Name;
    int Type;
    int Usage;
    const char* Getter;
    const char* Setter;
    void *Default_Value;
} property_info;
//todo make property arrays heterogeneous objects
typedef struct type_info{
    const char* Name;
    const char* Parent;
    size_t Size;
    void (*Create)(entity* Self);
    void (*Update)(entity* Self,double Delta_Time);
    void (*OnEvent)(entity* Self,event* Event);
    void (*Render)(entity* Self); 
    void (*Destroy)(entity* Self);
    property_info *Properties;
    size_t Property_Count;
    unsigned Flags;
}type_info;
//todo one single typeinfo that handles all the property arrays confounded with the type
struct entity{
    size_t ID;
    type_info *Type;
    char* Type_Name;
    char *Name;
};
RUNEFORGE_API entity* create_entity(const char* Type_Name,const char* Entity_Name);
RUNEFORGE_API void TypeDB_Register(type_info* Type);
RUNEFORGE_API void Destroy_TypeDB(void);
RUNEFORGE_API type_info** Get_Entity_Types(size_t* Count);
RUNEFORGE_API void TypeDB_Clear(void);
RUNEFORGE_API type_info* TypeDB_Get(const char* name);
//todo add entity type buckets, we want millions of entities at once without lag.
#endif