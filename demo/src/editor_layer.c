#include <runeforge.h>
#include <stdlib.h>
#include "panels/panel.h"
#include "panels/panel_button.h"
typedef struct editor_layer_data{
    panel_registry* Panel_Registry;
}editor_layer_data;
void update_editor_layer(layer* self,void* ctx){
    editor_layer_data *Data=(editor_layer_data*)self->LayerData;
    update_panels(Data->Panel_Registry);

}
void render_editor_layer(layer* self,void* ctx){
    editor_layer_data *Data=(editor_layer_data*)self->LayerData;
    render_panels(Data->Panel_Registry);
}
void detach_editor_layer(layer* self){
    editor_layer_data *Data=(editor_layer_data*)self->LayerData;
    destroy_panel_registry(Data->Panel_Registry);
}
void example_button_update(panel_button* Self){
    GAVEN_ASSERT(0,"Button Pressed");
}
layer* create_editor_layer(void){
    layer* Editor =calloc(1,sizeof(layer));
    Editor->Name="Editor Layer";
    editor_layer_data *Data=(editor_layer_data*)malloc(sizeof(editor_layer_data));
    Data->Panel_Registry=create_panel_registry();
    Editor->LayerData=Data;
    bind_layer_phase(Editor,layer_phase_overlay_render,render_editor_layer);
    bind_layer_phase(Editor,layer_phase_Update,update_editor_layer);
    Editor->OnDettach=detach_editor_layer;
    
    panel_data P={
        .Name="Panel",
        .Background_Char=',',
        .Height=24,
        .Width=20,
        .Min_Height=4,
        .Min_Width=5,
        .Is_Draggable=1,
        .Is_Resizable=1,
        .X=0,
        .Y=0,
        .Is_Viewport=0
    };
    panel_data View={
        .Name="Viewport",
        .Background_Char=' ',
        .Height=24,
        .Width=80,
        .Min_Height=4,
        .Min_Width=5,
        .Is_Draggable=1,
        .Is_Resizable=0,
        .X=24,
        .Y=0,
        .Is_Viewport=1
    };
    panel *Viewport = create_panel(View);
    add_panel_to_registry(Viewport,Data->Panel_Registry);
    panel *Pan = create_panel(P);
    panel_button *Button =create_panel_button(1,1,"BRRR",8,example_button_update);
    add_element_to_panel(Pan,&Button->Base);
    add_panel_to_registry(Pan,Data->Panel_Registry);
    return Editor;
    
}