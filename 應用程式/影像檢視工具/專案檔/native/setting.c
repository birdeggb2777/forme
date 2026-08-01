#include <stdio.h>
#include <windows.h> // 包含 SetConsoleOutputCP
#include <io.h>      // 包含 _setmode
#include <fcntl.h>   // 包含 _O_U16TEXT
#include <locale.h>  // 包含 setlocale
#include <stdint.h>  //uint8_t
#include <time.h> 
//////////////////////////////////
#include <shellapi.h> // argv用的
#pragma comment(lib, "Shell32.lib") // argv用的

#define BOOL int  
#define TRUE 1  
#define FALSE 0

#define bool int  
#define true 1  
#define false 0
#define True 1  
#define False 0

#define and &&
#define or  ||

#define wcharPtr wchar_t*
#define wchar wchar_t
#define UInt64 DWORD

#define max1(a,b) (((a) > (b)) ? (a) : (b))
#define min1(a,b) (((a) < (b)) ? (a) : (b))

#define memory(type, size) ((type *)malloc(sizeof(type) * (size)))
#define fori(count) for(int i = 0; i < (count); i++)

int argc; wcharPtr* argv;
int setup_arguments(){
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    return (!(argv != NULL and argc > 1)) ? 1 : 0;
}

void setup_console() {
    setlocale(LC_ALL, ".UTF8");
    _setmode(_fileno(stdout), _O_U16TEXT);
}

void setup_gui(){
    // 由於Windows可以設定縮放，需使用這行防止文字大小計算錯誤
    if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) 
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
}

//系統常數、亂數種子
void setup_sys(){
    // wcharPtr commandLine = GetCommandLineW();
    srand(time(NULL));
}

HINSTANCE ProgramID;
// 識別碼
void setup_instance(HINSTANCE hInstance){
    ProgramID = hInstance;
}