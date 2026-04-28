#include "runeforge.h"
#include <stdlib.h>
/*creating a player*/
/*player struct (data of the player)*/

typedef struct player{
    entity Base;
    sprite Sprite;
    double X,Y,Z;
}player;
/* player update function */
void player_update(entity* self,double deltaTime){
    player* P=(player*)self;
    if(is_key_pressed(RUNEFORGE_KEY_W))
        P->Y-=10.0*deltaTime;// moves up 10 per second
    if(is_key_pressed(RUNEFORGE_KEY_A))
        P->X-=10.0*deltaTime;// moves left 10 per second
    if(is_key_pressed(RUNEFORGE_KEY_S))
        P->Y+=10.0*deltaTime;// moves down 10 per second
    if(is_key_pressed(RUNEFORGE_KEY_D))
        P->X+=10.0*deltaTime;// moves right 10 per second
}
/* player render function */
void player_render(entity* self,renderer* Renderer){
    player* P=(player*)self;
    draw_sprite(Renderer,P->Sprite,(short)P->X,(short)P->Y,(short)P->Z);
}

static property_info Player_Props[] = {
    {"x",PROPERTY_TYPE_DOUBLE,offsetof(player,X),NULL,NULL,NULL},
    {"y",PROPERTY_TYPE_DOUBLE,offsetof(player,Y),NULL,NULL,NULL},
    {"z",PROPERTY_TYPE_DOUBLE,offsetof(player,Z),NULL,NULL,NULL},
    {"sprite",PROPERTY_TYPE_SIZET,offsetof(player,Sprite),NULL,NULL,NULL},
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
typedef struct main_layer_data{
    renderer* RuneWall;
    asset_manager* Asset_Manager;
    size_t Test_Sprite;
    entity_registry* Entity_Registry;
}main_layer_data;
void main_layer_ondetach(layer* self){
    main_layer_data* data = (main_layer_data*)self->LayerData;
    destroy_asset_manager(data->Asset_Manager);
    destroy_entity_registry(data->Entity_Registry);
    Destroy_TypeDB();
    destroy_runewall(data->RuneWall);
}
void main_layer_polling_callback(layer* self,void* ctx){
    /* each poll should only happen once per frame. So do not repoll in another layer */
    input_polling();
    poll_delta_time();
    /* the polling stuff */
    return;
}
void main_update_layer(layer* self, void* ctx){
    main_layer_data* data = (main_layer_data*)self->LayerData;
    update_context* dt = (update_context*)ctx;
    double deltaTime = *dt;
    update_entities(data->Entity_Registry,deltaTime);
}
/* only do this once in your application*/
void main_layer_rendering_start_callback(layer* self,void *ctx){
    main_layer_data* data = (main_layer_data*)self->LayerData;
    runewall_start_render_frame(data->RuneWall);
}
void main_layer_rendering_callback(layer* self,void *ctx){
    main_layer_data* data = (main_layer_data*)self->LayerData;
    sprite S = get_sprite(data->Asset_Manager,data->Test_Sprite);
    if(is_key_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
        draw_sprite(data->RuneWall,S,get_mouse_X(),get_mouse_Y(),3);
    }
    draw_sprite(data->RuneWall,S,4,4,1);
    draw_sprite(data->RuneWall,S,1,1,-1);
    render_entities(data->Entity_Registry,data->RuneWall);
}
/* only do this once in your application*/
void main_layer_rendering_end_callback(layer* self,void *ctx){
    main_layer_data* data = (main_layer_data*)self->LayerData;
    runewall_end_render_frame(data->RuneWall);
}
layer* create_main_layer(const char *name){
    layer* main_layer = calloc(1,sizeof(layer));
    main_layer->Name=name;
    bind_layer_phase(main_layer,layer_phase_polling,main_layer_polling_callback);
    bind_layer_phase(main_layer,layer_phase_render_begin,main_layer_rendering_start_callback);
    bind_layer_phase(main_layer,layer_phase_render,main_layer_rendering_callback);
    bind_layer_phase(main_layer,layer_phase_render_end,main_layer_rendering_end_callback);
    bind_layer_phase(main_layer,layer_phase_Update,main_update_layer);
    main_layer->OnDettach=main_layer_ondetach;
    main_layer_data* Data = (main_layer_data*)calloc(1,sizeof(main_layer_data));
    Data->RuneWall = create_runewall(80,24);
    Data->Asset_Manager= create_asset_manager();
    Data->Entity_Registry=create_entity_registry();
    /* loading assets */
    Data->Test_Sprite = add_asset_from_file(Data->Asset_Manager,ASSET_TYPE_SPRITE,"assets/test.txt");
    TypeDB_Register(&Player_Type);
    /* creating a player entity */
    player *P =(player*)create_entity("Player");
    P->X=10;
    P->Y=10;
    P->Z=2;
    P->Sprite=get_sprite(Data->Asset_Manager,add_asset_from_file(Data->Asset_Manager,ASSET_TYPE_SPRITE,"assets/player.txt"));
    add_entity_to_registry(Data->Entity_Registry,&P->Base);
    main_layer->LayerData=Data;
    return main_layer;
}
/* Using the prebuilt Gaven Main workflow */
application* gaven_main(int argc, char** argv){//ko
    /* We create the application*/
    application* app = create_gaven_application();
    /* Start Updates*/
    init_updates(app);
    /*Start Input*/
    init_input();
    /* Create main layer */
    layer* main_layer = create_main_layer("Main Layer");
    add_layer(app->Layer_Registry,main_layer);
    /* We return the application*/
    return app;
}