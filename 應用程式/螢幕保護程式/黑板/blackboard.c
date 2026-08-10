#include <windows.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if(uMsg != WM_DESTROY) return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    PostQuitMessage(0);
    return 0;
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* lpCmdLine, int nCmdShow) {
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);      
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpfnWndProc = WindowProc;                
    wc.hInstance = hInstance;                   
    wc.lpszClassName = L"black"; 
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassExW(&wc);
    HWND hwnd = CreateWindowExW(0, L"black", L"black", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, SW_MAXIMIZE);
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}