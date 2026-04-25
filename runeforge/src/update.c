#include "update.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif
static double DeltaTime=0.0;
void init_updates(application* Main_Application){
    static uint8_t A=0;
    if(!A){
        bind_phase_ctx(Main_Application->Layer_Registry,layer_phase_Update,&DeltaTime);
        A=0;
    }
}
double current_time(void){
    #ifdef _WIN32
    static LARGE_INTEGER freq;
    static uint8_t A=0;
    if(!A){
        QueryPerformanceFrequency(&freq);
        A=1;
    }
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)now.QuadPart/(double)freq.QuadPart;
    #else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
    #endif
}
void poll_delta_time(void){
    static uint8_t A=0;
    static double last = 0.0;
    double now = current_time();
    if(last!=0.0)
        DeltaTime = now-last;
    last=now;
}
#ifdef NDEBUG
void a;
#endif