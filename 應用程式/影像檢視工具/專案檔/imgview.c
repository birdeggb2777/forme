
#include "native/setting.c"

#include "BASE/image.c"
#include "BASE/string.c"

#include "native/gdiplus.c"

Image* imread(wcharPtr path){
    Image* img = (Image*) calloc(1, sizeof(Image));
    imread_native(path, &img->data, &img->width, &img->height, &img->channels);
    img->pixelFormat = BGRA;
    return img;
}

void mixAlpha2Image(Image* img){
    const int length = img->width * img->height * img->channels, strides = img->width * img->channels;
    const byte* data = img->data;
    bool includeAlpha = false;
    // 偵測的最小單位是100，沒有就當作沒有透明了
    for(int y = 0; y < img->height; y += 100){
        for(int x = 0; x < img->width; x += 100){
            if(img->data[y * strides + x + 3] < 255) includeAlpha = true;
        }
    }
    if(includeAlpha == false) return;
    
    // 如果有透明，就做出動作
    DWORD bgColor = GetSysColor(COLOR_WINDOW);
    byte bgR = GetRValue(bgColor), bgG = GetGValue(bgColor), bgB = GetBValue(bgColor);
    for(int i = 0; i < length; i += 4)
        if(img->data[i + 3] < 255) img->data[i + 0] = bgB,img->data[i + 1] = bgG,img->data[i + 2] = bgR;
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

// 判斷是否為圖片副檔名 (忽略大小寫)
int IsImageExtension(const wchar* ext) {
    if (ext == NULL) return 0;
    return (_wcsicmp(ext, L".jpg") == 0 ||  _wcsicmp(ext, L".jpeg") == 0 ||  _wcsicmp(ext, L".png") == 0 || 
     _wcsicmp(ext, L".bmp") == 0 || _wcsicmp(ext, L".gif") == 0 || _wcsicmp(ext, L".tif") == 0);
}

// 傳入檔案路徑，回傳圖片路徑字串陣列，並透過 outCount 輸出數量
// 注意！！！，FindFirstFileW回傳的順序不可信任
wchar** GetImageFilesInDirectory(const wchar* inputFilePath, int* outCount) {
    *outCount = 0;
    if (inputFilePath == NULL) return NULL;

    // 所在資料夾路徑
    wchar dirPath[MAX_PATH];
    wcscpy_s(dirPath, MAX_PATH, inputFilePath);
    
    // 尋找最後一個斜線的位置
    wchar* lastSlash = wcsrchr(dirPath, L'\\');
    if (lastSlash == NULL) lastSlash = wcsrchr(dirPath, L'/');
    
    // 將斜線後面的字元截斷，保留斜線 (例如 C:\images\a.jpg -> C:\images\)
    if (lastSlash != NULL) *(lastSlash + 1) = L'\0'; 
    // 如果沒有斜線，假設在當前目錄
    else wcscpy_s(dirPath, MAX_PATH, L".\\"); 

    // 準備搜尋字串 (資料夾路徑 + "*")
    wchar searchPath[MAX_PATH];
    swprintf_s(searchPath, MAX_PATH, L"%s*", dirPath);

    // 準備動態記憶體來存放結果 (預設先給10個)
    int capacity = 10, count = 0;
    wchar** fileList = (wchar**)malloc(capacity * sizeof(wchar*));
    if (fileList == NULL) return NULL;

    // 呼叫 Windows API 遍歷檔案
    WIN32_FIND_DATAW ffd;
    HANDLE hFind = FindFirstFileW(searchPath, &ffd);
    if (hFind == INVALID_HANDLE_VALUE) goto error;

    do {
        // 確保不是資料夾，而是實體檔案
        if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
            
        // 取得該檔案的副檔名 (找最後一個點)，不是圖片就跳過
        wchar* ext = wcsrchr(ffd.cFileName, L'.');
        if (!IsImageExtension(ext)) continue;
        
        // 如果陣列滿了，就擴充兩倍空間 (realloc)
        if (count >= capacity) {
            capacity *= 2;
            wchar** temp = (wchar**)realloc(fileList, capacity * sizeof(wchar*));
            if (temp == NULL) goto error; // 記憶體不足時跳出
            fileList = temp;
        }

        // 組合完整的絕對路徑 (資料夾路徑 + 檔名)
        wchar fullPath[MAX_PATH];
        swprintf_s(fullPath, MAX_PATH, L"%s%s", dirPath, ffd.cFileName);

        // 分配獨立記憶體
        size_t len = wcslen(fullPath) + 1;
        fileList[count] = (wchar*)malloc(len * sizeof(wchar));
        wcscpy_s(fileList[count], len, fullPath);

        count++;
    } while (FindNextFileW(hFind, &ffd)); // 繼續找下一個

    FindClose(hFind); // 搜尋完畢，關閉 Handle

    *outCount = count;
    return fileList;
error:
    free(fileList);
    return NULL;
}

#include "native/window.c"

// 程式進入點 (main function)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wcharPtr lpCmdLine, int nCmdShow) {
    // 處理輸入的參數
    int argc;
    wcharPtr* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!(argv != NULL && argc > 1)) goto Error;

    setup_console();
    setup_gui();
    setup_sys();
    initGdiPlus();
    
    // 載入整個資料夾的影像清單
    playlistCount = 0;
    playlist = GetImageFilesInDirectory(argv[1], &playlistCount);
    for(int i = 0;i < playlistCount; i++)
        if (wcscmp(argv[1], playlist[i]) == 0) currentImgIndex = i;

    // 載入目標圖片
    GlobalImage = imread(argv[1]);
    mixAlpha2Image(GlobalImage);
    if(GlobalImage->width == 0) goto Error;

    // 建立視窗
    window* window_ = createWindow(hInstance);
    setWindow(window_,hInstance, argv[1], 1920, 1080);

    // 顯示視窗
    ShowWindow(window_->hwnd, SW_MAXIMIZE);
    UpdateWindow(window_->hwnd);

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