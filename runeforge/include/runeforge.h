#ifndef RUNEFORGE_H
#define RUNEFORGE_H
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

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

/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

/* When compiling for windows, we specify a specific calling convention to avoid issues where we are being called from a project with a different default calling convention.  For windows you have 3 define options:

CJSON_HIDE_SYMBOLS - Define this in the case where you don't want to ever dllexport symbols
CJSON_EXPORT_SYMBOLS - Define this on library build when you want to dllexport symbols (default)
CJSON_IMPORT_SYMBOLS - Define this if you want to dllimport symbol

For *nix builds that support visibility attribute, you can define similar behavior by

setting default visibility to hidden by adding
-fvisibility=hidden (for gcc)
or
-xldscope=hidden (for sun cc)
to CFLAGS

then using the CJSON_API_VISIBILITY flag to "export" the same symbols the way CJSON_EXPORT_SYMBOLS does

*/

#define CJSON_CDECL __cdecl
#define CJSON_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(CJSON_HIDE_SYMBOLS) && !defined(CJSON_IMPORT_SYMBOLS) && !defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_EXPORT_SYMBOLS
#endif

#if defined(CJSON_HIDE_SYMBOLS)
#define CJSON_PUBLIC(type)   type CJSON_STDCALL
#elif defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllexport) type CJSON_STDCALL
#elif defined(CJSON_IMPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllimport) type CJSON_STDCALL
#endif
#else /* !__WINDOWS__ */
#define CJSON_CDECL
#define CJSON_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define CJSON_PUBLIC(type)   __attribute__((visibility("default"))) type
#else
#define CJSON_PUBLIC(type) type
#endif
#endif

/* project version */
#define CJSON_VERSION_MAJOR 1
#define CJSON_VERSION_MINOR 7
#define CJSON_VERSION_PATCH 19

#include <stddef.h>

/* cJSON Types: */
#define cJSON_Invalid (0)
#define cJSON_False  (1 << 0)
#define cJSON_True   (1 << 1)
#define cJSON_NULL   (1 << 2)
#define cJSON_Number (1 << 3)
#define cJSON_String (1 << 4)
#define cJSON_Array  (1 << 5)
#define cJSON_Object (1 << 6)
#define cJSON_Raw    (1 << 7) /* raw json */

#define cJSON_IsReference 256
#define cJSON_StringIsConst 512

/* The cJSON structure: */
typedef struct cJSON
{
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *next;
    struct cJSON *prev;
    /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    struct cJSON *child;

    /* The type of the item, as above. */
    int type;

    /* The item's string, if type==cJSON_String  and type == cJSON_Raw */
    char *valuestring;
    /* writing to valueint is DEPRECATED, use cJSON_SetNumberValue instead */
    int valueint;
    /* The item's number, if type==cJSON_Number */
    double valuedouble;

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char *string;
} cJSON;

typedef struct cJSON_Hooks
{
      /* malloc/free are CDECL on Windows regardless of the default calling convention of the compiler, so ensure the hooks allow passing those functions directly. */
      void *(CJSON_CDECL *malloc_fn)(size_t sz);
      void (CJSON_CDECL *free_fn)(void *ptr);
} cJSON_Hooks;

typedef int cJSON_bool;

/* Limits how deeply nested arrays/objects can be before cJSON rejects to parse them.
 * This is to prevent stack overflows. */
#ifndef CJSON_NESTING_LIMIT
#define CJSON_NESTING_LIMIT 1000
#endif

/* Limits the length of circular references can be before cJSON rejects to parse them.
 * This is to prevent stack overflows. */
#ifndef CJSON_CIRCULAR_LIMIT
#define CJSON_CIRCULAR_LIMIT 10000
#endif

/* returns the version of cJSON as a string */
CJSON_PUBLIC(const char*) cJSON_Version(void);

/* Supply malloc, realloc and free functions to cJSON */
CJSON_PUBLIC(void) cJSON_InitHooks(cJSON_Hooks* hooks);

/* Memory Management: the caller is always responsible to free the results from all variants of cJSON_Parse (with cJSON_Delete) and cJSON_Print (with stdlib free, cJSON_Hooks.free_fn, or cJSON_free as appropriate). The exception is cJSON_PrintPreallocated, where the caller has full responsibility of the buffer. */
/* Supply a block of JSON, and this returns a cJSON object you can interrogate. */
CJSON_PUBLIC(cJSON *) cJSON_Parse(const char *value);
CJSON_PUBLIC(cJSON *) cJSON_ParseWithLength(const char *value, size_t buffer_length);
/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error so will match cJSON_GetErrorPtr(). */
CJSON_PUBLIC(cJSON *) cJSON_ParseWithOpts(const char *value, const char **return_parse_end, cJSON_bool require_null_terminated);
CJSON_PUBLIC(cJSON *) cJSON_ParseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, cJSON_bool require_null_terminated);

/* Render a cJSON entity to text for transfer/storage. */
CJSON_PUBLIC(char *) cJSON_Print(const cJSON *item);
/* Render a cJSON entity to text for transfer/storage without any formatting. */
CJSON_PUBLIC(char *) cJSON_PrintUnformatted(const cJSON *item);
/* Render a cJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
CJSON_PUBLIC(char *) cJSON_PrintBuffered(const cJSON *item, int prebuffer, cJSON_bool fmt);
/* Render a cJSON entity to text using a buffer already allocated in memory with given length. Returns 1 on success and 0 on failure. */
/* NOTE: cJSON is not always 100% accurate in estimating how much memory it will use, so to be safe allocate 5 bytes more than you actually need */
CJSON_PUBLIC(cJSON_bool) cJSON_PrintPreallocated(cJSON *item, char *buffer, const int length, const cJSON_bool format);
/* Delete a cJSON entity and all subentities. */
CJSON_PUBLIC(void) cJSON_Delete(cJSON *item);

/* Returns the number of items in an array (or object). */
CJSON_PUBLIC(int) cJSON_GetArraySize(const cJSON *array);
/* Retrieve item number "index" from array "array". Returns NULL if unsuccessful. */
CJSON_PUBLIC(cJSON *) cJSON_GetArrayItem(const cJSON *array, int index);
/* Get item "string" from object. Case insensitive. */
CJSON_PUBLIC(cJSON *) cJSON_GetObjectItem(const cJSON * const object, const char * const string);
CJSON_PUBLIC(cJSON *) cJSON_GetObjectItemCaseSensitive(const cJSON * const object, const char * const string);
CJSON_PUBLIC(cJSON_bool) cJSON_HasObjectItem(const cJSON *object, const char *string);
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
CJSON_PUBLIC(const char *) cJSON_GetErrorPtr(void);

/* Check item type and return its value */
CJSON_PUBLIC(char *) cJSON_GetStringValue(const cJSON * const item);
CJSON_PUBLIC(double) cJSON_GetNumberValue(const cJSON * const item);

/* These functions check the type of an item */
CJSON_PUBLIC(cJSON_bool) cJSON_IsInvalid(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsFalse(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsTrue(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsBool(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsNull(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsNumber(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsString(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsArray(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsObject(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsRaw(const cJSON * const item);

/* These calls create a cJSON item of the appropriate type. */
CJSON_PUBLIC(cJSON *) cJSON_CreateNull(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateTrue(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateFalse(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateBool(cJSON_bool boolean);
CJSON_PUBLIC(cJSON *) cJSON_CreateNumber(double num);
CJSON_PUBLIC(cJSON *) cJSON_CreateString(const char *string);
/* raw json */
CJSON_PUBLIC(cJSON *) cJSON_CreateRaw(const char *raw);
CJSON_PUBLIC(cJSON *) cJSON_CreateArray(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateObject(void);

/* Create a string where valuestring references a string so
 * it will not be freed by cJSON_Delete */
CJSON_PUBLIC(cJSON *) cJSON_CreateStringReference(const char *string);
/* Create an object/array that only references it's elements so
 * they will not be freed by cJSON_Delete */
CJSON_PUBLIC(cJSON *) cJSON_CreateObjectReference(const cJSON *child);
CJSON_PUBLIC(cJSON *) cJSON_CreateArrayReference(const cJSON *child);

/* These utilities create an Array of count items.
 * The parameter count cannot be greater than the number of elements in the number array, otherwise array access will be out of bounds.*/
CJSON_PUBLIC(cJSON *) cJSON_CreateIntArray(const int *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateFloatArray(const float *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateDoubleArray(const double *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateStringArray(const char *const *strings, int count);

/* Append item to the specified array/object. */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToArray(cJSON *array, cJSON *item);
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the cJSON object.
 * WARNING: When this function was used, make sure to always check that (item->type & cJSON_StringIsConst) is zero before
 * writing to `item->string` */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObjectCS(cJSON *object, const char *string, cJSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToObject(cJSON *object, const char *string, cJSON *item);

/* Remove/Detach items from Arrays/Objects. */
CJSON_PUBLIC(cJSON *) cJSON_DetachItemViaPointer(cJSON *parent, cJSON * const item);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromArray(cJSON *array, int which);
CJSON_PUBLIC(void) cJSON_DeleteItemFromArray(cJSON *array, int which);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObject(cJSON *object, const char *string);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObjectCaseSensitive(cJSON *object, const char *string);
CJSON_PUBLIC(void) cJSON_DeleteItemFromObject(cJSON *object, const char *string);
CJSON_PUBLIC(void) cJSON_DeleteItemFromObjectCaseSensitive(cJSON *object, const char *string);

/* Update array items. */
CJSON_PUBLIC(cJSON_bool) cJSON_InsertItemInArray(cJSON *array, int which, cJSON *newitem); /* Shifts pre-existing items to the right. */
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemViaPointer(cJSON * const parent, cJSON * const item, cJSON * replacement);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObjectCaseSensitive(cJSON *object,const char *string,cJSON *newitem);

/* Duplicate a cJSON item */
CJSON_PUBLIC(cJSON *) cJSON_Duplicate(const cJSON *item, cJSON_bool recurse);
/* Duplicate will create a new, identical cJSON item to the one you pass, in new memory that will
 * need to be released. With recurse!=0, it will duplicate any children connected to the item.
 * The item->next and ->prev pointers are always zero on return from Duplicate. */
/* Recursively compare two cJSON items for equality. If either a or b is NULL or invalid, they will be considered unequal.
 * case_sensitive determines if object keys are treated case sensitive (1) or case insensitive (0) */
CJSON_PUBLIC(cJSON_bool) cJSON_Compare(const cJSON * const a, const cJSON * const b, const cJSON_bool case_sensitive);

/* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.
 * The input pointer json cannot point to a read-only address area, such as a string constant, 
 * but should point to a readable and writable address area. */
CJSON_PUBLIC(void) cJSON_Minify(char *json);

/* Helper functions for creating and adding items to an object at the same time.
 * They return the added item or NULL on failure. */
CJSON_PUBLIC(cJSON*) cJSON_AddNullToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddTrueToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddFalseToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddBoolToObject(cJSON * const object, const char * const name, const cJSON_bool boolean);
CJSON_PUBLIC(cJSON*) cJSON_AddNumberToObject(cJSON * const object, const char * const name, const double number);
CJSON_PUBLIC(cJSON*) cJSON_AddStringToObject(cJSON * const object, const char * const name, const char * const string);
CJSON_PUBLIC(cJSON*) cJSON_AddRawToObject(cJSON * const object, const char * const name, const char * const raw);
CJSON_PUBLIC(cJSON*) cJSON_AddObjectToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddArrayToObject(cJSON * const object, const char * const name);

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define cJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
/* helper for the cJSON_SetNumberValue macro */
CJSON_PUBLIC(double) cJSON_SetNumberHelper(cJSON *object, double number);
#define cJSON_SetNumberValue(object, number) ((object != NULL) ? cJSON_SetNumberHelper(object, (double)number) : (number))
/* Change the valuestring of a cJSON_String object, only takes effect when type of object is cJSON_String */
CJSON_PUBLIC(char*) cJSON_SetValuestring(cJSON *object, const char *valuestring);

/* If the object is not a boolean type this does nothing and returns cJSON_Invalid else it returns the new type*/
#define cJSON_SetBoolValue(object, boolValue) ( \
    (object != NULL && ((object)->type & (cJSON_False|cJSON_True))) ? \
    (object)->type=((object)->type &(~(cJSON_False|cJSON_True)))|((boolValue)?cJSON_True:cJSON_False) : \
    cJSON_Invalid\
)

/* Macro for iterating over an array or object */
#define cJSON_ArrayForEach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

/* malloc/free objects using the malloc/free functions that have been set with cJSON_InitHooks */
CJSON_PUBLIC(void *) cJSON_malloc(size_t size);
CJSON_PUBLIC(void) cJSON_free(void *object);

#ifdef __cplusplus
}
#endif

#endif


#ifndef LOG_H
#define LOG_H
typedef enum{
    GAVEN_COLOR_RESET,
    GAVEN_RED,
    GAVEN_GREEN,
    GAVEN_BLUE,
    GAVEN_YELLOW,
    GAVEN_MAGENTA,
    GAVEN_CYAN,
    GAVEN_WHITE
} GAVEN_COLOR;
#ifdef _WIN32
    #define GAVEN_DEBUG_BREAK() __debugbreak()
#elif defined(__linux__)
    #include <signal.h>
    #define GAVEN_DEBUG_BREAK() raise(SIGTRAP)
#endif
RUNEFORGE_API void GAVEN_PRINT_COLOR(GAVEN_COLOR color, const char* message, ...);
RUNEFORGE_API void GAVEN_PRINT_COLOR_V(GAVEN_COLOR color, const char* message, va_list args);
RUNEFORGE_API void GAVEN_INFO(const char* message, ...);
RUNEFORGE_API void GAVEN_WARN(const char* message, ...);
RUNEFORGE_API void gaven_assert_message(const char* message, const char* file, const char* function, uint32_t line,...);
RUNEFORGE_API void destroy_logging(void);
#ifndef NDEBUG
#define GAVEN_ASSERT(condition, message, ...)\
    do {\
        if (!(condition)) {\
            gaven_assert_message(message, __FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);\
            GAVEN_DEBUG_BREAK();\
        }\
    }\
    while(0)
#else
#define GAVEN_ASSERT(condition, message, ...)\
    do {\
        if (!(condition)) {\
            gaven_assert_message(message, __FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);\
        }\
    }\
    while(0)

#endif
#endif

#ifndef EVENT_H
#define EVENT_H
#ifdef EVENT_CATEGORY_64
typedef uint64_t event_category;
#define EVENT_CATEGORY_SIZE 64
#else
typedef uint32_t event_category;
#define EVENT_CATEGORY_SIZE 32
#endif
#define EVENT_CATEGORY_BIT(x) ((event_category)1 <<  (x))
/* creates an event category with the name "name".
Note: bit needs to be unique number from 0 to 31 or to 63 if EVENT_CATEGORY_64 is defined*/

#define create_event_category(name,bit)\
    enum { event_category_##name = (event_category)(EVENT_CATEGORY_BIT(bit)) }
typedef uint32_t event_type;
/* creates an event type, it is very important that you write the name of the type without ""
Note: value needs to be a unique positive integer*/
#define create_event_type(struct_event_type,value)\
    enum { event_type_##struct_event_type = (event_type)(value) }

typedef struct event{
    event_type Type;
    event_category Category_Flags;
    uint8_t Handled;
    const char* Name;
    void (*To_String)(struct event *Event, char* buffer, size_t buffer_size);
}event;
static inline uint8_t is_event_in_category(event *Event,event_category Category){ return (Event->Category_Flags & Category)!=0;}
#define EVENT_DISPATCH_V(Event_Pointer, Event_Type, Dispatch_Function,...)\
    do{\
        if((Event_Pointer)!=NULL && (Event_Pointer)->Type == event_type_##Event_Type)\
            (Event_Pointer)->Handled = Dispatch_Function((Event_Type*)(Event_Pointer),__VA_ARGS__);\
    } while(0)
#define EVENT_DISPATCH(Event_Pointer, Event_Type, Dispatch_Function)\
    do{\
        if((Event_Pointer)!=NULL && (Event_Pointer)->Type == event_type_##Event_Type)\
            (Event_Pointer)->Handled = Dispatch_Function((Event_Type*)(Event_Pointer));\
    } while(0)
#endif
#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H
typedef struct http http;
typedef struct http_connection http_connection;
typedef enum http_method {
    HTTP_GET_REQUEST,
    HTTP_POST_REQUEST
} http_method;

create_event_category(networking,0);

typedef struct networking_recieve{
    event base;
    char* Recieved_Data;
    http_connection* Connection;
    size_t Size;
}networking_recieve;
create_event_type(networking_recieve,1);
RUNEFORGE_API void networking_recieve_init(networking_recieve *Event,http_connection* Connection,char* Recieved_Data,size_t Size);

typedef struct networking_new_connection{
    event base;
    http_connection* Connection;
    char Ip[64];
    uint16_t Port;
}networking_new_connection;
create_event_type(networking_new_connection,0);
RUNEFORGE_API void networking_new_connection_init(networking_new_connection *Event,http_connection *Connection,char Ip[], uint16_t Port);

typedef struct http_response http_response;
typedef struct http_request http_request;
RUNEFORGE_API http *create_http_server(const char* address,uint16_t port);
RUNEFORGE_API http *create_http_client(void);
RUNEFORGE_API http_connection *connect_http_client(http* client,char address[],uint16_t port);
RUNEFORGE_API void poll_http(http* server);
RUNEFORGE_API void destroy_http_server(http* server);

inline static void send_http_request_json(http_connection* Connection,http_method Method,cJSON* Body_JSON, const char* Path,const char* Headers);
RUNEFORGE_API void send_http_request(http_connection* Connection,http_method Method,char* Body, const char* Path,const char* Headers);


inline static void send_http_response_json(http_connection* Connection,cJSON *Body_JSON, int Status_Code, const char* Status_Text,const char* Headers);
RUNEFORGE_API void send_http_response(http_connection* Connection,const char *Body, int Status_Code, const char* Status_Text,const char* Headers);

#endif
#ifndef LAYER_H
#define LAYER_H
typedef struct layer layer;
typedef uint32_t layer_phase;
typedef struct layer_binding{
    layer* Layer;
    layer_phase Phase;
    void (*Layer_phase_callback)(layer* self,void* phase_ctx);
}layer_binding;
typedef struct phase_bucket{
    layer_binding** Bindings;
    layer_phase Phase;
    size_t Count;
    size_t Capacity;
    void* Phase_Context;
}phase_bucket;
typedef struct layer_registry{
    layer** Layers;
    size_t Count;
    size_t Capacity;
    phase_bucket* Phase_Buckets;
    size_t Phase_Count;
    size_t Phase_Capacity;
} layer_registry;
#define create_layer_phase(phase_name,order)\
    enum { layer_phase_##phase_name = (layer_phase)(order) }

struct layer{
    const char* Name;
    layer_binding* Bindings;
    size_t Bindings_Count;
    size_t Bindings_Capacity;
    void (*OnAttach)(layer* self);
    void (*OnDettach)(layer* self);
    void (*OnEvent)(layer* self, event* Event);
    void* LayerData;
};
RUNEFORGE_API void add_layer(layer_registry* Layer_Registry, layer* Layer);
RUNEFORGE_API void bind_layer_phase(layer* Layer, layer_phase Phase, void (*Callback)(layer* self,void* phase_ctx));
RUNEFORGE_API void bind_phase_ctx(layer_registry* Registry, layer_phase Phase, void* Phase_Context);
RUNEFORGE_API void remove_layer(layer_registry* Layer_Registry, layer* Layer);
#endif
#ifndef GAVEN_H
#define GAVEN_H

typedef struct application application;

struct application{
    uint8_t Running;
    layer_registry* Layer_Registry;
};

RUNEFORGE_API void application_event_callback(event *e);
RUNEFORGE_API application* create_gaven_application(void);
RUNEFORGE_API void run_application(void);
RUNEFORGE_API void destroy_application(void);
/* Macros */
create_layer_phase(polling,0);
#endif

#ifndef RENDERER_H
#define RENDERER_H

typedef struct renderer renderer;
typedef struct sprite{
    char *Data;
    short Width;
    short Height;
} sprite;

RUNEFORGE_API void set_console_size(short Width,short Height);
RUNEFORGE_API void destroy_runewall(renderer* self);
RUNEFORGE_API void runewall_start_render_frame(renderer* self);
RUNEFORGE_API void runewall_end_render_frame(renderer* self);
RUNEFORGE_API void set_renderer_offset(renderer* self,short X,short Y);

RUNEFORGE_API sprite create_sprite(const char* Sprite, short Width, short Height);
RUNEFORGE_API void draw_sprite(renderer* self,sprite Sprite, short X, short Y,short Z);
RUNEFORGE_API void draw_overlay_sprite(renderer* self,sprite Sprite, short X, short Y,short Z);
RUNEFORGE_API void destroy_sprite(sprite *s);


#endif
#ifndef PRIMITIVES_H
#define PRIMITIVES_H

RUNEFORGE_API sprite create_text(const char* Text,short length);
create_layer_phase(render_begin,4);
create_layer_phase(render,5);
create_layer_phase(overlay_render,7);
create_layer_phase(render_end,8);
#endif
#ifndef ENTITY_H
#define ENTITY_H


typedef struct entity entity;
typedef enum {
    PROPERTY_TYPE_INT,
    PROPERTY_TYPE_FLOAT,
    PROPERTY_TYPE_STRING,
    PROPERTY_TYPE_DOUBLE,
    PROPERTY_TYPE_SIZET
}property_types;
typedef  struct property_info{
    const char* Name;
    int Type;
    int Usage;
    const char* Getter;
    const char* Setter;
    void *Default_Value;
} property_info;
typedef struct type_info{
    const char* Name;
    const char* Parent;
    size_t Size;
    void (*Create)(entity* Self);
    void (*Update)(entity* Self,double Delta_Time);
    void (*Render)(entity* Self); 
    void (*Destroy)(entity* Self);
    property_info *Properties;
    size_t Property_Count;
    unsigned Flags;
}type_info;
struct entity{
    size_t ID;
    type_info *Type;
    char *Name;
};
RUNEFORGE_API entity* create_entity(const char* Type_Name,const char* Entity_Name);
RUNEFORGE_API void TypeDB_Register(type_info* Type);
RUNEFORGE_API void Destroy_TypeDB(void);
RUNEFORGE_API type_info** Get_Entity_Types(size_t* Count);
RUNEFORGE_API void TypeDB_Clear(void);
#endif
#ifndef ENTITY_REGISTRY_H
#define ENTITY_REGISTRY_H
typedef struct entity_registry entity_registry;
struct entity_registry{
    entity **Entities;
    size_t Count;
    size_t Cap;
    const char* Path;
    char* Name;
    size_t Version;
};
RUNEFORGE_API entity_registry* create_entity_registry(void);
RUNEFORGE_API void add_entity_to_registry(entity_registry *Registry,entity *Entity);
RUNEFORGE_API void update_entities(entity_registry* Self,double deltaTime);
RUNEFORGE_API void render_entities(entity_registry* Self);
RUNEFORGE_API void free_entity(entity_registry *Registry,entity *Entity);
RUNEFORGE_API void destroy_entity_registry(entity_registry* Self);
RUNEFORGE_API void unload_entity_registry(entity_registry* Self);
RUNEFORGE_API void serialize_entity_registry(const char* path,entity_registry* Entity_Registry);
RUNEFORGE_API void deserialize_entity_registry(const char* path,entity_registry* Entity_Registry);
#endif

#ifndef ASSET_TYPES_H
#define ASSET_TYPES_H
typedef enum asset_type {
    ASSET_TYPE_SPRITE,
    ASSET_TYPE_SCENE
}asset_type;
#endif

#ifndef RUNEFORGELAYER_H
#define RUNEFORGELAYER_H
RUNEFORGE_API void set_main_scene(const char *path);
RUNEFORGE_API void draw_game_sprite(sprite Sprite,short X,short Y,short Z);
RUNEFORGE_API void draw_game_overlay_sprite(sprite Sprite,short X,short Y,short Z);
RUNEFORGE_API void set_window_size(short Width,short Height);
RUNEFORGE_API void set_panel_offset(short X,short Y);
RUNEFORGE_API sprite get_game_sprite(size_t id);
RUNEFORGE_API short get_window_width(void);
RUNEFORGE_API short get_window_height(void);
RUNEFORGE_API entity_registry* load_scene(const char* path);
RUNEFORGE_API void save_scene(const char* Path,const char* Name);
RUNEFORGE_API void unload_scene(void);
RUNEFORGE_API void change_update_state(uint8_t new_state);
RUNEFORGE_API uint8_t get_state(void);
RUNEFORGE_API void remove_asset(size_t ID);
RUNEFORGE_API application* runeforge_main(void);
RUNEFORGE_API size_t load_game_asset(const char* Path,asset_type Type);
RUNEFORGE_API void add_entity(entity* e);
RUNEFORGE_API size_t get_asset_id_from_path(const char* Path);

#endif


#ifndef KEYCODES_H
#define KEYCODES_H
/* from glfw3.h */
#define RUNEFORGE_KEY_SPACE              32
#define RUNEFORGE_KEY_APOSTROPHE         39  /* ' */
#define RUNEFORGE_KEY_COMMA              44  /* , */
#define RUNEFORGE_KEY_MINUS              45  /* - */
#define RUNEFORGE_KEY_PERIOD             46  /* . */
#define RUNEFORGE_KEY_SLASH              47  /* / */
#define RUNEFORGE_KEY_0                  48
#define RUNEFORGE_KEY_1                  49
#define RUNEFORGE_KEY_2                  50
#define RUNEFORGE_KEY_3                  51
#define RUNEFORGE_KEY_4                  52
#define RUNEFORGE_KEY_5                  53
#define RUNEFORGE_KEY_6                  54
#define RUNEFORGE_KEY_7                  55
#define RUNEFORGE_KEY_8                  56
#define RUNEFORGE_KEY_9                  57
#define RUNEFORGE_KEY_SEMICOLON          59  /* ; */
#define RUNEFORGE_KEY_EQUAL              61  /* = */
#define RUNEFORGE_KEY_A                  65
#define RUNEFORGE_KEY_B                  66
#define RUNEFORGE_KEY_C                  67
#define RUNEFORGE_KEY_D                  68
#define RUNEFORGE_KEY_E                  69
#define RUNEFORGE_KEY_F                  70
#define RUNEFORGE_KEY_G                  71
#define RUNEFORGE_KEY_H                  72
#define RUNEFORGE_KEY_I                  73
#define RUNEFORGE_KEY_J                  74
#define RUNEFORGE_KEY_K                  75
#define RUNEFORGE_KEY_L                  76
#define RUNEFORGE_KEY_M                  77
#define RUNEFORGE_KEY_N                  78
#define RUNEFORGE_KEY_O                  79
#define RUNEFORGE_KEY_P                  80
#define RUNEFORGE_KEY_Q                  81
#define RUNEFORGE_KEY_R                  82
#define RUNEFORGE_KEY_S                  83
#define RUNEFORGE_KEY_T                  84
#define RUNEFORGE_KEY_U                  85
#define RUNEFORGE_KEY_V                  86
#define RUNEFORGE_KEY_W                  87
#define RUNEFORGE_KEY_X                  88
#define RUNEFORGE_KEY_Y                  89
#define RUNEFORGE_KEY_Z                  90
#define RUNEFORGE_KEY_LEFT_BRACKET       91  /* [ */
#define RUNEFORGE_KEY_BACKSLASH          92  /* \ */
#define RUNEFORGE_KEY_RIGHT_BRACKET      93  /* ] */
#define RUNEFORGE_KEY_GRAVE_ACCENT       96  /* ` */
#define RUNEFORGE_KEY_WORLD_1            161 /* non-US #1 */
#define RUNEFORGE_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define RUNEFORGE_KEY_ESCAPE             256
#define RUNEFORGE_KEY_ENTER              257
#define RUNEFORGE_KEY_TAB                258
#define RUNEFORGE_KEY_BACKSPACE          259
#define RUNEFORGE_KEY_INSERT             260
#define RUNEFORGE_KEY_DELETE             261
#define RUNEFORGE_KEY_RIGHT              262
#define RUNEFORGE_KEY_LEFT               263
#define RUNEFORGE_KEY_DOWN               264
#define RUNEFORGE_KEY_UP                 265
#define RUNEFORGE_KEY_PAGE_UP            266
#define RUNEFORGE_KEY_PAGE_DOWN          267
#define RUNEFORGE_KEY_HOME               268
#define RUNEFORGE_KEY_END                269
#define RUNEFORGE_KEY_CAPS_LOCK          280
#define RUNEFORGE_KEY_SCROLL_LOCK        281
#define RUNEFORGE_KEY_NUM_LOCK           282
#define RUNEFORGE_KEY_PRINT_SCREEN       283
#define RUNEFORGE_KEY_PAUSE              284
#define RUNEFORGE_KEY_F1                 290
#define RUNEFORGE_KEY_F2                 291
#define RUNEFORGE_KEY_F3                 292
#define RUNEFORGE_KEY_F4                 293
#define RUNEFORGE_KEY_F5                 294
#define RUNEFORGE_KEY_F6                 295
#define RUNEFORGE_KEY_F7                 296
#define RUNEFORGE_KEY_F8                 297
#define RUNEFORGE_KEY_F9                 298
#define RUNEFORGE_KEY_F10                299
#define RUNEFORGE_KEY_F11                300
#define RUNEFORGE_KEY_F12                301
#define RUNEFORGE_KEY_F13                302
#define RUNEFORGE_KEY_F14                303
#define RUNEFORGE_KEY_F15                304
#define RUNEFORGE_KEY_F16                305
#define RUNEFORGE_KEY_F17                306
#define RUNEFORGE_KEY_F18                307
#define RUNEFORGE_KEY_F19                308
#define RUNEFORGE_KEY_F20                309
#define RUNEFORGE_KEY_F21                310
#define RUNEFORGE_KEY_F22                311
#define RUNEFORGE_KEY_F23                312
#define RUNEFORGE_KEY_F24                313
#define RUNEFORGE_KEY_F25                314
#define RUNEFORGE_KEY_KP_0               320
#define RUNEFORGE_KEY_KP_1               321
#define RUNEFORGE_KEY_KP_2               322
#define RUNEFORGE_KEY_KP_3               323
#define RUNEFORGE_KEY_KP_4               324
#define RUNEFORGE_KEY_KP_5               325
#define RUNEFORGE_KEY_KP_6               326
#define RUNEFORGE_KEY_KP_7               327
#define RUNEFORGE_KEY_KP_8               328
#define RUNEFORGE_KEY_KP_9               329
#define RUNEFORGE_KEY_KP_DECIMAL         330
#define RUNEFORGE_KEY_KP_DIVIDE          331
#define RUNEFORGE_KEY_KP_MULTIPLY        332
#define RUNEFORGE_KEY_KP_SUBTRACT        333
#define RUNEFORGE_KEY_KP_ADD             334
#define RUNEFORGE_KEY_KP_ENTER           335
#define RUNEFORGE_KEY_KP_EQUAL           336
#define RUNEFORGE_KEY_LEFT_SHIFT         340
#define RUNEFORGE_KEY_LEFT_CONTROL       341
#define RUNEFORGE_KEY_LEFT_ALT           342
#define RUNEFORGE_KEY_LEFT_SUPER         343
#define RUNEFORGE_KEY_RIGHT_SHIFT        344
#define RUNEFORGE_KEY_RIGHT_CONTROL      345
#define RUNEFORGE_KEY_RIGHT_ALT          346
#define RUNEFORGE_KEY_RIGHT_SUPER        347
#define RUNEFORGE_KEY_MENU               348


// from glfw
#define RUNEFORGE_MOUSE_BUTTON_1         1
#define RUNEFORGE_MOUSE_BUTTON_2         2
#define RUNEFORGE_MOUSE_BUTTON_3         3
#define RUNEFORGE_MOUSE_BUTTON_4         4
#define RUNEFORGE_MOUSE_BUTTON_5         5
#define RUNEFORGE_MOUSE_BUTTON_6         6
#define RUNEFORGE_MOUSE_BUTTON_7         7
#define RUNEFORGE_MOUSE_BUTTON_8         8
#define RUNEFORGE_MOUSE_BUTTON_LAST      RUNEFORGE_MOUSE_BUTTON_8
#define RUNEFORGE_MOUSE_BUTTON_LEFT      RUNEFORGE_MOUSE_BUTTON_1
#define RUNEFORGE_MOUSE_BUTTON_RIGHT     RUNEFORGE_MOUSE_BUTTON_2
#define RUNEFORGE_MOUSE_BUTTON_MIDDLE    RUNEFORGE_MOUSE_BUTTON_3

#endif
#ifndef INPUT_H
#define INPUT_H

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
RUNEFORGE_API uint8_t is_key_pressed(int Keycode);
RUNEFORGE_API uint8_t is_key_just_pressed(int Keycode);
RUNEFORGE_API uint8_t is_key_just_released(int Keycode);
RUNEFORGE_API short get_mouse_X(void);
RUNEFORGE_API short get_mouse_Y(void);
RUNEFORGE_API const char* get_text_input(size_t* len);
RUNEFORGE_API void init_input(void);
RUNEFORGE_API void input_polling(void);
#endif
create_layer_phase(Update,2);
#endif