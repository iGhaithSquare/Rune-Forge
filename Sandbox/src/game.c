#include "game.h"
/*creating a player*/
/*player struct (data of the player)*/

typedef struct player{
    entity Base;
    size_t Sprite_ID;
    double X,Y,Z;
}player;
/* player update function */
void player_update(entity* self,double deltaTime){
    player* P=(player*)self;
    if(is_key_pressed(RUNEFORGE_KEY_W))
        P->Y-=10.0*deltaTime; //moves up 10 per second
    if(is_key_pressed(RUNEFORGE_KEY_A))
        P->X-=10.0*deltaTime; //moves left 10 per second
    if(is_key_pressed(RUNEFORGE_KEY_S))
        P->Y+=10.0*deltaTime; //moves down 10 per second
    if(is_key_pressed(RUNEFORGE_KEY_D))
        P->X+=10.0*deltaTime; //moves right 10 per second
}
/* player render function */
void player_render(entity* self){    
    player* P=(player*)self;
    draw_game_sprite(get_game_sprite(P->Sprite_ID),(short)P->X,(short)P->Y,(short)P->Z);
}
static property_info Player_Props[] = {
    {"x",PROPERTY_TYPE_DOUBLE,offsetof(player,X),NULL,NULL,NULL},
    {"y",PROPERTY_TYPE_DOUBLE,offsetof(player,Y),NULL,NULL,NULL},
    {"z",PROPERTY_TYPE_DOUBLE,offsetof(player,Z),NULL,NULL,NULL},
    {"spriteID",PROPERTY_TYPE_SIZET,offsetof(player,Sprite_ID),NULL,NULL,NULL},
};
static type_info Player_Type = {
    .Name= "Player",
    .Parent= "entity",
    .Size=sizeof(player),
    .Create= NULL,
    .Destroy=NULL,
    .Update= player_update,
    .Render=player_render,
    .Properties=Player_Props,
    .Property_Count=4,
    .Flags=0
};

GAME_API void game(void){
    TypeDB_Register(&Player_Type);
}
