#include "runeforge.h"
#include <stdlib.h>
#include "editor.h"
#include "editor_menu_layer.h"
void game_main(application* app,int argc, char** argv){
    short game_width = 120;
    short game_height = 34;
    set_window_size(game_width,game_height);
    if(argc>1){
        open_project(app->Layer_Registry,argv[1]);
    }
    else{
        add_layer(app->Layer_Registry,create_editor_menu_layer(app->Layer_Registry));
    }
}