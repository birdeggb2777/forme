
#include "native/setting.c"

#include "BASE/image.c"
#include "BASE/string.c"

#include "native/window.c"
#include "native/gdiplus.c"

Image* imread(string path){
    Image* img = (Image*) calloc(1, sizeof(Image));
    imread_native(path.val, &img->data, &img->width, &img->height, &img->channels);
    img->pixelFormat = BGRA;
    return img;
}

// 取得檔案名稱
wcharPtr getFileName(wcharPtr fullPath){
    
    // 寬字元陣列
    wchar drive[_MAX_DRIVE]; wchar dir[_MAX_DIR];
    wchar FName[_MAX_FNAME]; wchar ext[_MAX_EXT];

    // 進行拆分
    _wsplitpath_s(fullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, FName, _MAX_FNAME, ext, _MAX_EXT);
    
    // 取得檔案名稱
    int file_len = wcslen(FName) + wcslen(ext) + 1;
    wcharPtr fileName = (wcharPtr)calloc(file_len, sizeof(wchar));
    swprintf_s(fileName, file_len, L"%s%s", FName, ext);

    return fileName;
}

// 程式進入點 (main function)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wcharPtr lpCmdLine, int nCmdShow) {
    // 處理輸入的參數
    int argc;
    wcharPtr* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!(argc > 1)) goto Error;

    setup_console();
    setup_gui();
    setup_sys();
    initGdiPlus();

    string imagePath;
    apply(&imagePath, argv[1]);
    GlobalImage = imread(imagePath); //initImage(1200,1300,4,RGBA);
    if(GlobalImage->width == 0) goto Error;
    
    string title;
    apply(&title, getFileName(argv[1]));

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

    Error:
        MessageBox(NULL, "圖片讀取失敗！原因窩也不知道(╥﹏╥)", "錯誤", MB_ICONERROR);
        return 1;
}