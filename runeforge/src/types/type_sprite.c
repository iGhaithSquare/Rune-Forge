#include "type_sprite.h"
void type_sprite_render(entity* self){    
    type_sprite* Sprite=(type_sprite*)self;
    sprite S=get_game_sprite(Sprite->SpriteID);
    if(S.Data)
        draw_game_sprite(S,(short)Sprite->Base.X,(short)Sprite->Base.Y,(short)Sprite->Base.Z);
}
static property_info Sprite_Props[] = {
    {"Sprite",PROPERTY_TYPE_SIZET,offsetof(type_sprite,SpriteID),NULL,NULL,NULL},
};
type_info Type_Sprite = {
    .Name= "Sprite",
    .Parent= &Type_Entity2D,
    .Size=sizeof(type_sprite),
    .Create= NULL,
    .Destroy=NULL,
    .Poll=NULL,
    .Update= NULL,
    .Render=type_sprite_render,
    .OnEvent=NULL,
    .Properties=Sprite_Props,
    .Property_Count=1,
    .Flags=0
};