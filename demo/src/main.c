#include "runeforge.h"
#include <stdlib.h>
#include "editor_layer.h"
#include "editor_menu_layer.h"
void game_main(application* app,int argc, char** argv){
    if(argc>1){
        set_main_scene("assets/scene.json");
        short game_width = 120;
        short game_height = 34;
        set_window_size(game_width,game_height);
        entity_registry* Reg =load_scene("assets/scene.json");
        add_layer(app->Layer_Registry,create_editor_layer(Reg));
    }
    else{
        short game_width = 120;
        short game_height = 34;
        set_window_size(game_width,game_height);
        add_layer(app->Layer_Registry,create_editor_menu_layer());
    }
}