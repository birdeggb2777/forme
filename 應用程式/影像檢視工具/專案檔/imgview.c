
#include "native/setting.c"
#include "BASE/image.c"
#include "native/gdiplus.c"
#include "native/window.c"
#include "BASE/base.c"

// 程式進入點 (main function)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wcharPtr lpCmdLine, int nCmdShow) {
    
    if(setup_arguments()) goto Error; // 處理輸入的參數
    setup_console();                  // 編碼
    setup_instance(hInstance);        // 識別碼
    setup_gui();                      // 避免縮放模糊
    setup_sys();                      // 亂數種子
    
    // 載入整個資料夾的影像清單，並將index指向影像清單的位置
    filesList = GetImageFilesInDirectory(argv[1], &filesListCount);
    fori(filesListCount) if (wcscmp(argv[1], filesList[i]) == 0) currentImgIndex = i;

    // 載入目標圖片
    GlobalImage = imread(argv[1], true);
    if(GlobalImage->width == 0) goto Error;

    //顯示視窗
    displayWindow();
    //離開程式
    closeGdiPlus();
    return 0;

Error:
    MessageBox(NULL, "圖片讀取失敗！原因窩也不知道(╥﹏╥)", "錯誤", MB_ICONERROR);
    closeGdiPlus();
    return 1;
}