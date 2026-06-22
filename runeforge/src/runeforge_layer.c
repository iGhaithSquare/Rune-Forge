#include "runeforge_layer.h"
#include "asset_manager.h"
#include "entity_registry.h"
#include "update.h"
#include "input.h"
#include <string.h>
#include <stdlib.h>
#include "register_type.h"
static const char* main_scene=NULL;
static renderer* Renderer=NULL;
static asset_manager* Asset_Manager=NULL;
static entity_registry* Entity_Registry=NULL;
static short width=80;
static short height=24;
static char* Scene_Path;
static short offsetX;
static short offsetY;
uint8_t State=0;
void set_main_scene(const char *path){
    main_scene=path;
}
void set_window_size(short Width,short Height){
    width=Width;
    height=Height;
}

short get_window_width(void){
    return width;
}
short get_window_height(void){
    return height;
}
void main_layer_ondetach(layer* self){
    destroy_asset_manager(Asset_Manager);
    destroy_entity_registry(Entity_Registry);
    Destroy_TypeDB();
    destroy_runewall(Renderer);
    free(Scene_Path);
}
void main_layer_polling_callback(layer* self,void* ctx){
    input_polling();
    poll_delta_time();
    poll_entities(Entity_Registry);
    return;
}

void main_update_layer(layer* self, void* ctx){
    update_context* dt = (update_context*)ctx;
    double deltaTime = *dt;
    if(State&1)
        update_entities(Entity_Registry,deltaTime);
}
void main_layer_rendering_start_callback(layer* self,void *ctx){
    runewall_start_render_frame(Renderer);
}
void main_layer_rendering_callback(layer* self,void *ctx){
    render_entities(Entity_Registry);
}
void main_layer_rendering_end_callback(layer* self,void *ctx){
    runewall_end_render_frame(Renderer);
}
void main_layer_rendering_overlay_end_callback(layer* self,void *ctx){
    runewall_end_overlay_render_frame(Renderer);
}

void main_layer_on_event(layer* self, event* Event){
    if(State&1)
        entities_on_event(Entity_Registry,Event);
}

layer* create_main_layer(const char *name){
    layer* main_layer = calloc(1,sizeof(layer));
    main_layer->Name=name;
    bind_layer_phase(main_layer,layer_phase_polling,main_layer_polling_callback);
    bind_layer_phase(main_layer,layer_phase_render_begin,main_layer_rendering_start_callback);
    bind_layer_phase(main_layer,layer_phase_render,main_layer_rendering_callback);
    bind_layer_phase(main_layer,layer_phase_render_end,main_layer_rendering_end_callback);
    bind_layer_phase(main_layer,layer_phase_render_overlay_end,main_layer_rendering_overlay_end_callback);
    bind_layer_phase(main_layer,layer_phase_Update,main_update_layer);
    main_layer->OnEvent=main_layer_on_event;
    main_layer->OnDettach=main_layer_ondetach;
    Asset_Manager=create_asset_manager();
    

    
    main_layer->LayerData=NULL;
    return main_layer;
}
void draw_game_sprite(sprite Sprite,short X,short Y,short Z){
    draw_sprite(Renderer,Sprite,X,Y,Z);
}
void draw_game_overlay_sprite(sprite Sprite,short X,short Y,short Z){
    draw_overlay_sprite(Renderer,Sprite,X,Y,Z);
}

size_t load_game_asset(const char* Path,asset_type Type){
    return add_asset_from_file(Asset_Manager,Type,Path);
}
sprite get_game_sprite(size_t id){
    return get_sprite(Asset_Manager,id);
}
/* Using the prebuilt Gaven Main workflow */
void add_entity(entity* e){
    add_entity_to_registry(Entity_Registry,e);
}
void set_panel_offset(short X,short Y){
    offsetX=X;
    offsetY=Y;
    set_renderer_offset(Renderer,X,Y);
}
application* runeforge_main(void){
    /* We create the application*/
    application* app = create_gaven_application();
    /* Start Updates*/
    init_updates(app);
    /*Start Input*/
    init_input();
    /* Create main layer */
    layer* main_layer = create_main_layer("Main Layer");
    register_types();
    Entity_Registry=create_entity_registry();
    Renderer = create_runewall(width,height);
    add_layer(app->Layer_Registry,main_layer);
    set_runewall_resizable(Renderer,0);
    return app;
}
entity_registry* load_scene(const char* path){
    Entity_Registry->Path=strdup(path);
    deserialize_entity_registry(path,Entity_Registry);
    Scene_Path=Entity_Registry->Path;
    return Entity_Registry;
}
void save_scene(char* Path){
    if(Path!=NULL) Entity_Registry->Path=Path;
    serialize_entity_registry(Entity_Registry->Path,Entity_Registry);
}

void unload_scene(void){
    unload_entity_registry(Entity_Registry);
}
void change_update_state(uint8_t new_state){
    if(!(new_state&1)&&State&1){
        unload_scene();
        load_scene(Scene_Path);
    }
    State=new_state;
}
uint8_t get_state(void){
    return State;
}
void remove_asset(size_t ID,asset_type Type){
    remove_asset_from_asset_manager(Asset_Manager,Type,ID);
}
size_t get_asset_id_from_path(const char* Path,asset_type Type){
    return find_asset_from_asset_manager_with_path(Asset_Manager,Type,Path);
}
short get_relative_mouse_x(void){
    return get_mouse_X()-offsetX;
}
short get_relative_mouse_y(void){
    return get_mouse_Y()-offsetY;
}
entity* create_entity_in_registery(const char* Type_Name,const char* Entity_Name){
    Entity_Registry->Version++;
    return create_entity(Entity_Registry->Root,Type_Name,Entity_Name,NULL);
}