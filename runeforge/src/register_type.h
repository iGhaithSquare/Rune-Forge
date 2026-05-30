#ifndef REGISTER_TYPE_H
#define REGISTER_TYPE_H

#ifndef RUNEFORGE_API
#ifdef _WIN32
    #ifdef RUNEFORGE_BUILD_DLL
        #define RUNEFORGE_API __declspec(dllexport)
    #else
        #define RUNEFORGE_API __declspec(dllimport)
    #endif
#else
    #define RUNEFORGE_API __attribute__((visibility("default")))
#endif
#endif

RUNEFORGE_API void register_types(void);
#endif