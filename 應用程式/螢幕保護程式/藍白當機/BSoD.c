#include <windows.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if(uMsg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);
        RECT textRc = { 0, 0, rect.right, rect.bottom };
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);
        HFONT hFont = CreateFontW(-28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Lucida Console");
        SelectObject(hdc, hFont);
        wchar_t* text = L"Collecting data for crash dump ...\nInitializing disk for crash dump ...\nBeginning dump of physical memory.\nDumping physical memory to disk:  0";
        DrawTextW(hdc, text, -1, &textRc, DT_CALCRECT | DT_CENTER | DT_VCENTER | DT_WORDBREAK);
        int x = (rect.right - (textRc.right - textRc.left)) / 2, y = (rect.bottom - (textRc.bottom - textRc.top)) / 2;
        RECT drawRc = {x, y, x + (textRc.right - textRc.left), y + (textRc.bottom - textRc.top)};
        DrawTextW(hdc, text, -1, &drawRc, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
        EndPaint(hwnd, &ps);
    }
    if(uMsg != WM_DESTROY) return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    PostQuitMessage(0);
    return 0;
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* lpCmdLine, int nCmdShow) {
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);      
    wc.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(0, 0, 255)));
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