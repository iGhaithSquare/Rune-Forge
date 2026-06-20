#include "types/type_sprite.h"
RUNEFORGE_API void register_types(void){
    TypeDB_Register(&Type_Entity);
    TypeDB_Register(&Type_Entity2D);
    TypeDB_Register(&Type_Sprite);
}