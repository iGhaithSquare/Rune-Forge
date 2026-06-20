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
    struct type_info* Parent;
    size_t Size;
    void (*Create)(entity* Self);
    void (*Poll)(entity* Self);
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
    char *Path;
    entity** Children;
    size_t Count;
    size_t Cap;
    entity* Parent;
};
RUNEFORGE_API entity* create_entity(entity* Parent,const char* Type_Name,const char* Entity_Name,char *Path);
RUNEFORGE_API void add_entity_child(entity* Parent,entity* Child);
RUNEFORGE_API void free_child(entity *Parent,entity *Entity);
RUNEFORGE_API uint8_t poll_entity(entity * Self);
RUNEFORGE_API void update_entity(entity * Self,double deltaTime);
RUNEFORGE_API void entity_on_event(entity* Self,event *Event);
RUNEFORGE_API void render_entity(entity* Self);
RUNEFORGE_API entity* get_child(entity* Self,size_t id);
RUNEFORGE_API void TypeDB_Register(type_info* Type);
RUNEFORGE_API void Destroy_TypeDB(void);
RUNEFORGE_API type_info** Get_Entity_Types(size_t* Count);
RUNEFORGE_API void TypeDB_Clear(void);
RUNEFORGE_API type_info* TypeDB_Get(const char* name);
RUNEFORGE_API entity* deserialize_sub_registry(const char* path);
RUNEFORGE_API entity* deserialize_entity(cJSON* root, entity* Parent);
//todo add entity type buckets, we want millions of entities at once without lag.
#endif