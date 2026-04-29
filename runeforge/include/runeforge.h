#ifndef RUNEFORGE_H
#define RUNEFORGE_H
#include <stddef.h>
#include <stdint.h>
#include "../src/runeforge_layer.h"
#include "../src/input.h"
typedef enum asset_type {
    ASSET_TYPE_SPRITE
}asset_type;
create_layer_phase(Update,2);

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
typedef struct type_info{
    const char* Name;
    const char* Parent;
    size_t Size;
    void (*Create)(entity* Self);
    void (*Update)(entity* Self,double Delta_Time);
    void (*Render)(entity* Self); 
    void (*Destroy)(entity* Self);
    property_info *Properties;
    size_t Property_Count;
    unsigned Flags;
}type_info;
struct entity{
    size_t ID;
    type_info *Type;
};
void TypeDB_Register(type_info* Type);
size_t load_game_asset(const char* Path,asset_type Type);
void add_entity(entity* e);
entity* create_entity(const char* Type_Name);
#endif