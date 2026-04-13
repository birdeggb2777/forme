#include <windows.h>

// 直接指示連結器連結這些函式庫
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

void drawImage2Window(HDC hdc, HWND hwnd, byte* imgData, int x, int y, int srcWidth, int srcHeight, int dstWidth, int dstHeight){
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = srcWidth;
    // biHeight 為負值表示這是一個由上到下的 DIB (top-down DIB)，
    // 記憶體中的第一行像素對應到圖像的第一行。
    bmi.bmiHeader.biHeight = -srcHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;          // 每個像素 32 位元 (對應 DWORD)
    bmi.bmiHeader.biCompression = BI_RGB;   // 未壓縮

    // 避免縮放時呈現顏色怪異、鋸齒狀等現象
    SetStretchBltMode(hdc, HALFTONE);
    SetBrushOrgEx(hdc, 0, 0, NULL);

    // 將像素陣列繪製到視窗上
    StretchDIBits(
        hdc,
        x, y,                            // 目的地左上角座標 (在視窗上的 x, y)
        dstWidth, dstHeight,             // 目的地繪製的寬與高
        0, 0,                            // 來源左上角座標 (在陣列中的 x, y)
        srcWidth, srcHeight,             // 來源的寬與高
        imgData,                         // 指向像素資料的指標
        &bmi,                            // 指向 BITMAPINFO 結構的指標
        DIB_RGB_COLORS,                  // 顏色表使用 RGB 值
        SRCCOPY                          // 直接複製像素 (不做混合)
    );
}

// 視窗訊息處理函數 (WindowProc)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SIZE: {
            // 當視窗大小改變時，強制重繪整個視窗 (設為TRUE，代表有先清除影像)
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_PAINT: {
            RECT windowRect;
            GetClientRect(hwnd, &windowRect);
            PAINTSTRUCT ps;

            //清除整個視窗
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            
            //取得視窗長寬
            RECT rect;
            int Window_Width = 1, Window_Height = 1;
            if (GetWindowRect(hwnd, &rect)) {
                Window_Width = rect.right - rect.left; Window_Height = rect.bottom - rect.top;
            }

            // 計算縮放比例，採用最小的那一個
            float scaleW = (float)Window_Width / (float)GlobalImage->width;
            float scaleH = (float)Window_Height / (float)GlobalImage->height;
            float scale = (scaleW < scaleH) ? scaleW : scaleH;
            // 繪製在螢幕上的長寬
            int dstWidth  = (int)(GlobalImage->width * scale);
            int dstHeight = (int)(GlobalImage->height * scale);
            // 於視窗中置中的位置
            int x = (Window_Width - dstWidth) / 2;
            int y = (Window_Height - dstHeight) / 2;
            // 畫上去
            drawImage2Window(hdc, hwnd, GlobalImage->data, x, y, GlobalImage->width, GlobalImage->height, dstWidth, dstHeight);

            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

typedef struct{
   WNDCLASSEXW wc;
   HWND hwnd;
} window;

window* createWindow(HINSTANCE hInstance){
    // 註冊視窗
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);      
    wc.hbrBackground = NULL; // (HBRUSH)(COLOR_WINDOW + 1); // 系統預設的視窗顏色
    wc.lpfnWndProc = WindowProc;                
    wc.hInstance = hInstance;                   
    wc.lpszClassName = L"imgview"; 

    window* window_ = (window*)malloc(sizeof(window));
    window_->wc=wc;

    if (!RegisterClassExW(&window_->wc)) return 0;
    return window_;
}

void setWindow(window* window_, HINSTANCE hInstance, wcharPtr title, int width, int height){
    // 建立視窗
    window_->hwnd = CreateWindowExW(
        1,                            // 設定為沒有icon的視窗
        L"imgview",                   // 視窗類別名稱
        title,                        // 視窗標題
        WS_OVERLAPPEDWINDOW,          // 視窗樣式
        CW_USEDEFAULT, CW_USEDEFAULT, // 起始位置
        width, height,                // 只有長寬、沒有高
        NULL,                         // 父視窗 (不需要)
        NULL,                         // 功能表 (不需要)
        hInstance,                    // 程式實體的識別碼
        NULL                          // 其他參數 (這也不需要)
    );
    if (window_->hwnd == NULL) {
        MessageBox(NULL, "視窗建立失敗！原因窩也不知道(╥﹏╥)", "錯誤", MB_ICONERROR);
        return;
    }
}

