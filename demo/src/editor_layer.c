#include <runeforge.h>
#include <stdlib.h>
#include "panels/panel_button.h"
#include "panels/file_explorer.h"
#include "panels/scene_explorer.h"
#include "panels/inspector.h"
#include "panels/navbar.h"
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
layer* create_editor_layer(entity_registry* Entity_Registry){
    layer* Editor =calloc(1,sizeof(layer));
    Editor->Name="Editor Layer";
    editor_layer_data *Data=(editor_layer_data*)malloc(sizeof(editor_layer_data));
    Data->Panel_Registry=create_panel_registry();
    Editor->LayerData=Data;
    bind_layer_phase(Editor,layer_phase_overlay_render,render_editor_layer);
    bind_layer_phase(Editor,layer_phase_Update,update_editor_layer);
    Editor->OnDettach=detach_editor_layer;
    
    panel_data View={
        .Name="Viewport",
        .Background_Char=' ',
        .Height=24,
        .Width=80,
        .Min_Height=24,
        .Min_Width=80,
        .Anchor=15, //anchored all
        .Is_Resizable=0,
        .Z_Index=-1//viewport is -1
    };
    panel* Navbar=create_navbar();
    add_panel_to_registry(Navbar,Data->Panel_Registry);
    panel* Pan = create_scene_explorer();
    navbar_add_scene_explorer(Navbar,Pan);
    add_panel_neighbor(Navbar,Pan,1);
    add_panel_to_registry(Pan,Data->Panel_Registry);
    panel *Viewport = create_panel(View);
    add_panel_neighbor(Navbar,Viewport,1);
    add_panel_neighbor(Pan,Viewport,0);
    panel* File_Explorer = create_file_explorer();
    add_panel_neighbor(Pan,File_Explorer,0);
    panel* Inspector = create_inspector();
    navbar_add_inspector(Navbar,Inspector);
    scene_explorer_point_to_inspector(Pan,Inspector);
    add_panel_neighbor(Navbar,Inspector,1);
    add_panel_neighbor(Viewport,Inspector,0);
    add_panel_neighbor(File_Explorer,Inspector,0);
    add_panel_neighbor(Viewport,File_Explorer,1);
    add_panel_to_registry(Viewport,Data->Panel_Registry);
    add_panel_to_registry(Inspector,Data->Panel_Registry);
    add_panel_to_registry(File_Explorer,Data->Panel_Registry);

    for(size_t i =0;i<Entity_Registry->Count;i++)
        add_entity_to_scene_explorer(Entity_Registry->Entities[i],Pan);
    return Editor;
}