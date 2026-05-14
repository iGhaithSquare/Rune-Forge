#include <runeforge.h>
#ifdef _WIN32
    #ifdef GAME_BUILD_DLL
        #define GAME_API __declspec(dllexport)
    #else
        #define GAME_API __declspec(dllimport)
    #endif
#else
    #define GAME_API __attribute__((visibility("default")))
#endif
GAME_API void game(void){
//Register Types Here
}
