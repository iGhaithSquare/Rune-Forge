
#include "input.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif
#define INPUT_KEYS 512
#define INPUT_TEXT_COUNT 128
typedef struct input_system{
    uint8_t keys[INPUT_KEYS];
    uint8_t last_frame_keys[INPUT_KEYS];
    short mouse_X;
    short mouse_Y;

    char text_buffer[INPUT_TEXT_COUNT];
    int text_count;
}input_system;
static input_system Input_System;
#ifdef _WIN32
int translate_vk(int code)
{
    switch (code)
    {
        /* =========================
           LETTERS
        ========================= */
        case 'A': return RUNEFORGE_KEY_A;
        case 'B': return RUNEFORGE_KEY_B;
        case 'C': return RUNEFORGE_KEY_C;
        case 'D': return RUNEFORGE_KEY_D;
        case 'E': return RUNEFORGE_KEY_E;
        case 'F': return RUNEFORGE_KEY_F;
        case 'G': return RUNEFORGE_KEY_G;
        case 'H': return RUNEFORGE_KEY_H;
        case 'I': return RUNEFORGE_KEY_I;
        case 'J': return RUNEFORGE_KEY_J;
        case 'K': return RUNEFORGE_KEY_K;
        case 'L': return RUNEFORGE_KEY_L;
        case 'M': return RUNEFORGE_KEY_M;
        case 'N': return RUNEFORGE_KEY_N;
        case 'O': return RUNEFORGE_KEY_O;
        case 'P': return RUNEFORGE_KEY_P;
        case 'Q': return RUNEFORGE_KEY_Q;
        case 'R': return RUNEFORGE_KEY_R;
        case 'S': return RUNEFORGE_KEY_S;
        case 'T': return RUNEFORGE_KEY_T;
        case 'U': return RUNEFORGE_KEY_U;
        case 'V': return RUNEFORGE_KEY_V;
        case 'W': return RUNEFORGE_KEY_W;
        case 'X': return RUNEFORGE_KEY_X;
        case 'Y': return RUNEFORGE_KEY_Y;
        case 'Z': return RUNEFORGE_KEY_Z;

        /* =========================
           NUMBERS
        ========================= */
        case '0': return RUNEFORGE_KEY_0;
        case '1': return RUNEFORGE_KEY_1;
        case '2': return RUNEFORGE_KEY_2;
        case '3': return RUNEFORGE_KEY_3;
        case '4': return RUNEFORGE_KEY_4;
        case '5': return RUNEFORGE_KEY_5;
        case '6': return RUNEFORGE_KEY_6;
        case '7': return RUNEFORGE_KEY_7;
        case '8': return RUNEFORGE_KEY_8;
        case '9': return RUNEFORGE_KEY_9;

        /* =========================
           SYMBOLS
        ========================= */
        case ' ':  return RUNEFORGE_KEY_SPACE;
        case '\n': return RUNEFORGE_KEY_ENTER;
        case '\r': return RUNEFORGE_KEY_ENTER;
        case '\t': return RUNEFORGE_KEY_TAB;
        case 27:   return RUNEFORGE_KEY_ESCAPE;
        case 8:    return RUNEFORGE_KEY_BACKSPACE;

        case ';': return RUNEFORGE_KEY_SEMICOLON;
        case '=': return RUNEFORGE_KEY_EQUAL;
        case ',': return RUNEFORGE_KEY_COMMA;
        case '-': return RUNEFORGE_KEY_MINUS;
        case '.': return RUNEFORGE_KEY_PERIOD;
        case '/': return RUNEFORGE_KEY_SLASH;
        case '`': return RUNEFORGE_KEY_GRAVE_ACCENT;
        case '[': return RUNEFORGE_KEY_LEFT_BRACKET;
        case ']': return RUNEFORGE_KEY_RIGHT_BRACKET;
        case '\\': return RUNEFORGE_KEY_BACKSLASH;
        case '\'': return RUNEFORGE_KEY_APOSTROPHE;

        /* =========================
           FUNCTION KEYS (ESCAPES / VK)
        ========================= */
        case 256: return RUNEFORGE_KEY_ESCAPE;
        case 257: return RUNEFORGE_KEY_ENTER;
        case 258: return RUNEFORGE_KEY_TAB;
        case 259: return RUNEFORGE_KEY_BACKSPACE;
        case 260: return RUNEFORGE_KEY_INSERT;
        case 261: return RUNEFORGE_KEY_DELETE;
        case 262: return RUNEFORGE_KEY_RIGHT;
        case 263: return RUNEFORGE_KEY_LEFT;
        case 264: return RUNEFORGE_KEY_DOWN;
        case 265: return RUNEFORGE_KEY_UP;
        case 266: return RUNEFORGE_KEY_PAGE_UP;
        case 267: return RUNEFORGE_KEY_PAGE_DOWN;
        case 268: return RUNEFORGE_KEY_HOME;
        case 269: return RUNEFORGE_KEY_END;

        /* =========================
           FUNCTION KEYS F1-F25
        ========================= */
        case 290: return RUNEFORGE_KEY_F1;
        case 291: return RUNEFORGE_KEY_F2;
        case 292: return RUNEFORGE_KEY_F3;
        case 293: return RUNEFORGE_KEY_F4;
        case 294: return RUNEFORGE_KEY_F5;
        case 295: return RUNEFORGE_KEY_F6;
        case 296: return RUNEFORGE_KEY_F7;
        case 297: return RUNEFORGE_KEY_F8;
        case 298: return RUNEFORGE_KEY_F9;
        case 299: return RUNEFORGE_KEY_F10;
        case 300: return RUNEFORGE_KEY_F11;
        case 301: return RUNEFORGE_KEY_F12;
        case 302: return RUNEFORGE_KEY_F13;
        case 303: return RUNEFORGE_KEY_F14;
        case 304: return RUNEFORGE_KEY_F15;
        case 305: return RUNEFORGE_KEY_F16;
        case 306: return RUNEFORGE_KEY_F17;
        case 307: return RUNEFORGE_KEY_F18;
        case 308: return RUNEFORGE_KEY_F19;
        case 309: return RUNEFORGE_KEY_F20;
        case 310: return RUNEFORGE_KEY_F21;
        case 311: return RUNEFORGE_KEY_F22;
        case 312: return RUNEFORGE_KEY_F23;
        case 313: return RUNEFORGE_KEY_F24;
        case 314: return RUNEFORGE_KEY_F25;

        /* =========================
           KEYPAD
        ========================= */
        case 320: return RUNEFORGE_KEY_KP_0;
        case 321: return RUNEFORGE_KEY_KP_1;
        case 322: return RUNEFORGE_KEY_KP_2;
        case 323: return RUNEFORGE_KEY_KP_3;
        case 324: return RUNEFORGE_KEY_KP_4;
        case 325: return RUNEFORGE_KEY_KP_5;
        case 326: return RUNEFORGE_KEY_KP_6;
        case 327: return RUNEFORGE_KEY_KP_7;
        case 328: return RUNEFORGE_KEY_KP_8;
        case 329: return RUNEFORGE_KEY_KP_9;

        case 330: return RUNEFORGE_KEY_KP_DECIMAL;
        case 331: return RUNEFORGE_KEY_KP_DIVIDE;
        case 332: return RUNEFORGE_KEY_KP_MULTIPLY;
        case 333: return RUNEFORGE_KEY_KP_SUBTRACT;
        case 334: return RUNEFORGE_KEY_KP_ADD;
        case 335: return RUNEFORGE_KEY_KP_ENTER;
        case 336: return RUNEFORGE_KEY_KP_EQUAL;

        /* =========================
           MODIFIERS
        ========================= */
        case 340: return RUNEFORGE_KEY_LEFT_SHIFT;
        case 341: return RUNEFORGE_KEY_LEFT_CONTROL;
        case 342: return RUNEFORGE_KEY_LEFT_ALT;
        case 343: return RUNEFORGE_KEY_LEFT_SUPER;
        case 344: return RUNEFORGE_KEY_RIGHT_SHIFT;
        case 345: return RUNEFORGE_KEY_RIGHT_CONTROL;
        case 346: return RUNEFORGE_KEY_RIGHT_ALT;
        case 347: return RUNEFORGE_KEY_RIGHT_SUPER;
        case 348: return RUNEFORGE_KEY_MENU;

        default:
            return 0;
    }
}
#else

static int decode_escape_sequence(char seq2, char seq3) {
    if (seq2 == '[') {
        switch (seq3) {
            case 'A': return RUNEFORGE_KEY_UP;
            case 'B': return RUNEFORGE_KEY_DOWN;
            case 'C': return RUNEFORGE_KEY_RIGHT;
            case 'D': return RUNEFORGE_KEY_LEFT;
        }
    }
    return 0;
}
static int read_key_linux(void) {
    char c;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    if (n <= 0) return 0;
    if (c != 27) {
        if (c == '\r' || c == '\n') return RUNEFORGE_KEY_ENTER;
        if (c == 127) return RUNEFORGE_KEY_BACKSPACE;
        if (c >= 'a' && c <= 'z') return c - 32;
        if (c >= 'A' && c <= 'Z') return c;
        if (c >= '0' && c <= '9') return c;
        return (uint8_t)c;
    }
    char seq1, seq2;
    if (read(STDIN_FILENO, &seq1, 1) <= 0) return RUNEFORGE_KEY_ESCAPE;
    if (read(STDIN_FILENO, &seq2, 1) <= 0) return RUNEFORGE_KEY_ESCAPE;
    return decode_escape_sequence(seq1, seq2);
}

#endif
void init_input(void){
    #ifdef _WIN32
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode=0;
    GetConsoleMode(h,&mode);
    mode &= ~(ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT);
    mode|=ENABLE_PROCESSED_INPUT;
    mode|=ENABLE_MOUSE_INPUT;
    GAVEN_ASSERT(SetConsoleMode(h,mode),"Set console mode failed: %lu", GetLastError());
    #else
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    #endif
    memset(Input_System.keys,0,INPUT_KEYS);
}

void input_polling(void){
    memcpy(Input_System.last_frame_keys,Input_System.keys,INPUT_KEYS);
    Input_System.text_count=0;
    #ifdef _WIN32
    HANDLE hIN = GetStdHandle(STD_INPUT_HANDLE);
    DWORD events = 0;
    GetNumberOfConsoleInputEvents(hIN,&events);
    while(events>0){
        INPUT_RECORD record;
        DWORD read;
        ReadConsoleInput(hIN,&record,1,&read);
        if(record.EventType==MOUSE_EVENT){
            MOUSE_EVENT_RECORD m = record.Event.MouseEvent;
            Input_System.mouse_X= m.dwMousePosition.X;
            Input_System.mouse_Y= m.dwMousePosition.Y;
            Input_System.keys[RUNEFORGE_MOUSE_BUTTON_LEFT]= (m.dwButtonState&FROM_LEFT_1ST_BUTTON_PRESSED)!=0;
            Input_System.keys[RUNEFORGE_MOUSE_BUTTON_MIDDLE]= (m.dwButtonState&FROM_LEFT_2ND_BUTTON_PRESSED)!=0;
            Input_System.keys[RUNEFORGE_MOUSE_BUTTON_RIGHT]= (m.dwButtonState&RIGHTMOST_BUTTON_PRESSED)!=0;
            //todo implement scrolling
        }
        if(record.EventType==KEY_EVENT){
            KEY_EVENT_RECORD k = record.Event.KeyEvent;
            int key = k.wVirtualKeyCode;
            Input_System.keys[translate_vk(key)]=k.bKeyDown;
            if(k.bKeyDown&&k.uChar.AsciiChar!=0&&Input_System.text_count+1<INPUT_TEXT_COUNT)
                Input_System.text_buffer[Input_System.text_count++]=(char)k.uChar.AsciiChar;
        }
        GetNumberOfConsoleInputEvents(hIN,&events);
    }
    
    #else
    //todo implement mouse
    //todo also implement getting text
    int key;
    while ((key=read_key_linux())!=0){
        if (key>=0 && key<INPUT_KEYS)
            Input_System.keys[key]=1;
    }
    #endif
}

uint8_t is_key_pressed(int Keycode){
    return Input_System.keys[Keycode];
}
uint8_t is_key_just_released(int Keycode){
    return (!Input_System.keys[Keycode]&&Input_System.last_frame_keys[Keycode]);
}
uint8_t is_key_just_pressed(int Keycode){
    return (Input_System.keys[Keycode]&&!Input_System.last_frame_keys[Keycode]);
}
const char* get_text_input(size_t* len){
    *len=(size_t)Input_System.text_count;
    return Input_System.text_buffer;
}
short get_mouse_X(void){
    return Input_System.mouse_X;
}
short get_mouse_Y(void){
    return Input_System.mouse_Y;
}