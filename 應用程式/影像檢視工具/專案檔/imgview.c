
#include "native/setting.c"

#include "BASE/image.c"
#include "BASE/string.c"

#include "native/window.c"
#include "native/gdiplus.c"

Image* imread(string path){
    Image* img = (Image*) malloc(sizeof(Image) * 1);
    imread_native(path.val, &img->data, &img->width, &img->height, &img->channels);
    img->pixelFormat = BGRA;
    return img;
}

// 取得檔案名稱
wcharPtr getFileName(char* fullPath){
    
    char drive[_MAX_DRIVE]; char dir[_MAX_DIR];
    char FName[_MAX_FNAME]; char ext[_MAX_EXT];

    // 進行拆分
    _splitpath_s(fullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, FName, _MAX_FNAME, ext, _MAX_EXT);

    // 取得檔案名稱
    int file_len = strlen(FName) + strlen(ext) + 1;
    char fileName[_MAX_FNAME + _MAX_EXT];
    snprintf(fileName, sizeof(fileName), "%s%s", FName, ext);
    return char_to_wchar(fileName);
}

// 程式進入點 (main function)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    if (!(__argc > 1)) {MessageBox(NULL, "圖片讀取失敗！原因窩也不知道(╥﹏╥)", "錯誤", MB_ICONERROR);return 0;} 
    setup_console();
    setup_gui();
    setup_sys();
    initGdiPlus();

    string imagePath;
    apply(&imagePath, char_to_wchar(__argv[1]));
    GlobalImage = imread(imagePath); //initImage(1200,1300,4,RGBA);
    if(GlobalImage->width == 0) {
        MessageBox(NULL, "圖片讀取失敗！原因窩也不知道(╥﹏╥)", "錯誤", MB_ICONERROR);
        return 0;
    }

    string title;
    apply(&title, getFileName(__argv[1]));

    // 建立視窗
    window* window_ = createWindow(hInstance);
    setWindow(window_,hInstance, title.val, 500, 300);

    // 顯示視窗
    ShowWindow(window_->hwnd, SW_MAXIMIZE);
    UpdateWindow(window_->hwnd);
    SetCursor(LoadCursor(NULL, IDC_ARROW));

    // 訊息迴圈
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // 翻譯虛擬按鍵訊息
        DispatchMessage(&msg);  // 分發訊息到視窗處理函數
    }

    closeGdiPlus();
    return (int)msg.wParam;
}