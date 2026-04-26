#include "runeforge.h"
#include <stdlib.h>

typedef struct main_layer_data{
    renderer* RuneWall;
    asset_manager* Asset_Manager;
    size_t Test_Sprite;
}main_layer_data;
void main_layer_ondetach(layer* self){
    main_layer_data* data = (main_layer_data*)self->LayerData;
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
    update_context* dt = (update_context*)ctx;
    double deltaTime = *dt;
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
        draw_sprite(data->RuneWall,S,get_mouse_X(),get_mouse_Y(),2);
    }
    draw_sprite(data->RuneWall,S,4,4,1);
    draw_sprite(data->RuneWall,S,1,1,-1);
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
    main_layer_data* Data = (main_layer_data*)calloc(1,sizeof(main_layer_data));
    Data->RuneWall = create_runewall(80,24);
    Data->Asset_Manager= create_asset_manager();
    Data->Test_Sprite = add_asset_from_file(Data->Asset_Manager,ASSET_TYPE_SPRITE,"assets/test.txt");
    main_layer->LayerData=Data;
    return main_layer;
}
/* Using the prebuilt Gaven Main workflow */
application* gaven_main(int argc, char** argv){
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