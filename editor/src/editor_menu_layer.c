#include "editor_menu_layer.h"
#include <stdlib.h>
#include "panels/editor_menu_panels/background_panel.h"
typedef struct editor_layer_menu_data{
    panel_registry* Panel_Registry;
}editor_layer_menu_data;
void update_editor_menu_layer(layer* self,void* ctx){
    editor_layer_menu_data *Data=(editor_layer_menu_data*)self->LayerData;
    update_panels(Data->Panel_Registry);
}
void render_editor_menu_layer(layer* self,void* ctx){
    editor_layer_menu_data *Data=(editor_layer_menu_data*)self->LayerData;
    render_panels(Data->Panel_Registry);
}
void detach_editor_menu_layer(layer* self){
    editor_layer_menu_data *Data=(editor_layer_menu_data*)self->LayerData;
    destroy_panel_registry(Data->Panel_Registry);
}

layer* create_editor_menu_layer(layer_registry* Registry){
    layer* Editor =calloc(1,sizeof(layer));
    Editor->Name="Editor Menu Layer";
    editor_layer_menu_data* Data= (editor_layer_menu_data*)malloc(sizeof(editor_layer_menu_data));
    Data->Panel_Registry=create_panel_registry();
    Editor->LayerData=Data;
    bind_layer_phase(Editor,layer_phase_overlay_render,render_editor_menu_layer);
    bind_layer_phase(Editor,layer_phase_Update,update_editor_menu_layer);
    add_panel_to_registry(create_background_panel(Registry,Editor),Data->Panel_Registry);
    Editor->OnDettach=detach_editor_menu_layer;
    return Editor;
}