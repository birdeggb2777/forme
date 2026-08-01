#include <windows.h>

// 直接指示連結器連結這些函式庫
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

Image* imread(wcharPtr path, bool mixAlpha);

///////////////////
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

void CopyImageToClipboard(HWND hwnd, Image* image) {

    const byte* pixels = image->data;
    int width = image->width, height = image->height;

    if (pixels == NULL || width <= 0 || height <= 0) return;

    // 僅限BGRA(bytesPerPixel=4)
    size_t rowBytes = width * 4, imageSize = rowBytes * height;

    // 剪貼簿需要的 CF_DIB 格式，記憶體結構是：[BITMAPINFOHEADER] + [像素陣列]
    // 剪貼簿的記憶體必須用 GlobalAlloc 配合 GMEM_MOVEABLE 配置，不能使用 malloc
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(BITMAPINFOHEADER) + imageSize);
    if (hGlobal == NULL) return;

    // 鎖定記憶體以取得指標
    byte* pMem = (byte*)GlobalLock(hGlobal);
    if (pMem == NULL) goto Error;

    // 1. 填寫檔案標頭 (BITMAPINFOHEADER)
    BITMAPINFOHEADER* bmi = (BITMAPINFOHEADER*)pMem;
    bmi->biSize = sizeof(BITMAPINFOHEADER);
    bmi->biWidth = width;
    bmi->biHeight = -height; // Windows 的點陣圖預設是「由下往上 (Bottom-Up)」存的，所以高度是負的
    bmi->biPlanes = 1;
    bmi->biBitCount = 32; // 32 位元色彩
    bmi->biCompression = BI_RGB;
    bmi->biSizeImage = (DWORD)imageSize;

    // 複製像素資料到標頭的正後方
    byte* dstPixels = pMem + sizeof(BITMAPINFOHEADER);
    memcpy(dstPixels, pixels, imageSize);

    // 解除記憶體鎖定
    GlobalUnlock(hGlobal);

    // 放入剪貼簿
    if (OpenClipboard(hwnd)) {
        EmptyClipboard();
        SetClipboardData(CF_DIB, hGlobal); // 格式改用 CF_DIB，並傳入剛剛分配的HGLOBAL
        CloseClipboard();
        return;
    } 
Error:
    GlobalFree(hGlobal); // 剪貼簿開啟失敗時，回收記憶體，但如果成功，就不可回收，因為剪貼簿已經接管了這塊記憶體
}

// 視窗訊息處理函數 (WindowProc)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {

        case WM_SIZE:
            // 當視窗大小改變時，強制重繪整個視窗 (設為TRUE，代表有先清除影像)
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_ERASEBKGND:
            return 1; // 防止閃爍

        case WM_PAINT: 
            PAINTSTRUCT ps;
                        
            //取得視窗長寬
            RECT rect;
            int Window_Width = 1, Window_Height = 1;
            if (GetClientRect(hwnd, &rect)) 
                Window_Width = rect.right - rect.left, Window_Height = rect.bottom - rect.top;

            // 清除整個視窗
            HDC hdc = BeginPaint(hwnd, &ps);
            HDC memDC = CreateCompatibleDC(hdc); // 建立與螢幕相容的記憶體管線
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, Window_Width, Window_Height); // 建立一塊空白畫布
            SelectObject(memDC, memBitmap); // 把畫布掛上管線
            FillRect(memDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

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
            drawImage2Window(memDC, hwnd, GlobalImage->data, x, y, GlobalImage->width, GlobalImage->height, dstWidth, dstHeight);
            // 將雙緩衝的影像寫回
            BitBlt(hdc, 0, 0, Window_Width, Window_Height, memDC, 0, 0, SRCCOPY);
            // 回收記憶體
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            return 0;

        case WM_KEYDOWN:
            switch (wParam)  {
                case VK_DOWN:
                case VK_RIGHT:
                    currentImgIndex -= 1;
                    if (currentImgIndex < 0) currentImgIndex = filesListCount - 1;
                    // 載入新影像並釋放原影像的記憶體
                    DisposeImage(GlobalImage);
                    GlobalImage = imread(filesList[currentImgIndex], true);
                    SetWindowTextW(hwnd, filesList[currentImgIndex]);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case VK_UP:
                case VK_LEFT:
                    currentImgIndex += 1;
                    if (currentImgIndex >= filesListCount) currentImgIndex = 0;
                    // 載入新影像並釋放原影像的記憶體
                    DisposeImage(GlobalImage);
                    GlobalImage = imread(filesList[currentImgIndex], true);
                    SetWindowTextW(hwnd, filesList[currentImgIndex]);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
            }
            // ctrl + C ，若高位元(0x8000)為 1 代表Ctrl正在被按著
            if (wParam == 'C' and (GetKeyState(VK_CONTROL) & 0x8000)){
                Image* image = imread(filesList[currentImgIndex], false);
                CopyImageToClipboard(hwnd, image);
                DisposeImage(image);
            }
            return 0;
            
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
} Window;

Window* createWindow(){
    // 註冊視窗
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);      
    wc.hbrBackground = NULL; // (HBRUSH)(COLOR_WINDOW + 1); // 系統預設的視窗顏色
    wc.lpfnWndProc = WindowProc;                
    wc.hInstance = ProgramID;                   
    wc.lpszClassName = L"imgview"; 
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    Window* window = memory(Window, 1);
    window->wc=wc;
    return RegisterClassExW(&window->wc) ? window : 0;
}

void setWindow(Window* window_, wcharPtr title, int width, int height){
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
        ProgramID,                    // 程式實體的識別碼
        NULL                          // 其他參數 (這也不需要)
    );
    if (window_->hwnd == NULL) MessageBox(NULL, "視窗建立失敗！原因窩也不知道(╥﹏╥)", "錯誤", MB_ICONERROR);
}
