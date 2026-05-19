#include "runeforge.h"
#include <stdlib.h>
#include "editor.h"
#include "editor_menu_layer.h"
int main(int argc, char** argv){
    short game_width = 120;
    short game_height = 34;
    set_window_size(game_width,game_height);
    application* app = runeforge_main();
    if(argc>1){
        open_project(app->Layer_Registry,argv[1]);
    }
    else{
        add_layer(app->Layer_Registry,create_editor_menu_layer(app->Layer_Registry));
    }
    run_application();
    destroy_application();
    return 1;
}