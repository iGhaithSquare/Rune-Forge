#include "runeforge_layer.h"
#include <asset_manager.h>
#include <entity_registry.h>
#include "update.h"
#include "input.h"
#include <stdlib.h>
static SCENE_MODE scene_mod = SCENE_LOAD;
static const char* main_scene=NULL;
static renderer* Renderer=NULL;
static asset_manager* Asset_Manager=NULL;
static entity_registry* Entity_Registry=NULL;
static short width=80;
static short height=24;
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
}
void main_layer_polling_callback(layer* self,void* ctx){
    input_polling();
    poll_delta_time();
    return;
}

void main_update_layer(layer* self, void* ctx){
    update_context* dt = (update_context*)ctx;
    double deltaTime = *dt;
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
layer* create_main_layer(const char *name){
    layer* main_layer = calloc(1,sizeof(layer));
    main_layer->Name=name;
    bind_layer_phase(main_layer,layer_phase_polling,main_layer_polling_callback);
    bind_layer_phase(main_layer,layer_phase_render_begin,main_layer_rendering_start_callback);
    bind_layer_phase(main_layer,layer_phase_render,main_layer_rendering_callback);
    bind_layer_phase(main_layer,layer_phase_render_end,main_layer_rendering_end_callback);
    bind_layer_phase(main_layer,layer_phase_Update,main_update_layer);
    main_layer->OnDettach=main_layer_ondetach;
    Asset_Manager=create_asset_manager();
    Entity_Registry=create_entity_registry();
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

void set_scene_mode(SCENE_MODE mod){
    scene_mod=mod;
}
void add_entity(entity* e){
    add_entity_to_registry(Entity_Registry,e);
}
void set_panel_offset(short X,short Y){
    set_renderer_offset(Renderer,X,Y);
}
application* gaven_main(int argc, char** argv){
    /* We create the application*/
    application* app = create_gaven_application();
    /* Start Updates*/
    init_updates(app);
    /*Start Input*/
    init_input();
    /* Create main layer */
    layer* main_layer = create_main_layer("Main Layer");
    game_main(app,argc,argv);
    Renderer = create_runewall(width,height);
    GAVEN_ASSERT(main_scene,"No main scene detected");
    if(scene_mod==SCENE_LOAD)
        deserialize_entity_registry(main_scene,Entity_Registry);
    else
        serialize_entity_registry(main_scene,Entity_Registry);
    add_layer(app->Layer_Registry,main_layer);
    return app;
}