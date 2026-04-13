
#pragma comment (lib,"gdiplus.lib")

#ifndef GDIPVER
#define GDIPVER 0x0110
#endif
#define PixelFormat32bppARGB 0x0026200A

typedef struct GdiplusStartupInput {
    UINT32 GdiplusVersion;
    void* DebugEventCallback;
    BOOL SuppressBackgroundThread;
    BOOL SuppressExternalCodecs;
} GdiplusStartupInput;

typedef enum GpStatus { Ok = 0, GenericError = 1, InvalidParameter = 2, OutOfMemory = 3,} GpStatus;
typedef enum ImageLockMode { ImageLockModeRead = 1, ImageLockModeWrite = 2, ImageLockModeUserInputBuf = 4} ImageLockMode;

// GDI+ 物件指標
typedef struct GpBitmap GpBitmap;
typedef struct GpImage GpImage;

// GDI+ 的結構
typedef struct Rect {
    INT X; INT Width;
    INT Y; INT Height;
} Rect;

typedef struct BitmapData {
    UINT Width;
    UINT Height;
    INT Stride;
    INT PixelFormat;
    void* Scan0;
    UINT_PTR Reserved;
} BitmapData;

// --- 函式原型 (Function Prototypes) ---
GpStatus WINAPI GdiplusStartup(ULONG_PTR* token, const GdiplusStartupInput* input, void* output);
void     WINAPI GdiplusShutdown(ULONG_PTR token);
GpStatus WINAPI GdipCreateBitmapFromFile(const WCHAR* filename, GpBitmap** bitmap);
GpStatus WINAPI GdipDisposeImage(GpBitmap* image);
GpStatus WINAPI GdipGetImageWidth(GpBitmap* image, UINT* width);
GpStatus WINAPI GdipGetImageHeight(GpBitmap* image, UINT* height);
GpStatus WINAPI GdipBitmapLockBits(GpBitmap* bitmap, const Rect* rect, UINT flags, INT format, BitmapData* lockedBitmapData);
GpStatus WINAPI GdipBitmapUnlockBits(GpBitmap* bitmap, BitmapData* lockedBitmapData);

// ========= 手動宣告結束 ========= //

ULONG_PTR gdiplusToken;
void initGdiPlus(){
    GdiplusStartupInput gdiplusStartupInput;
    memset(&gdiplusStartupInput, 0, sizeof(gdiplusStartupInput));
    gdiplusStartupInput.GdiplusVersion = 1;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

void closeGdiPlus(){
    GdiplusShutdown(gdiplusToken);
}

void imread_native(wcharPtr path, byte** imgData, int* width_, int* height_, int* chans_) {
    
    GpBitmap* bitmap = NULL;
    BitmapData bitmapData;
    GpStatus status = GdipCreateBitmapFromFile(path, &bitmap);

    if (status != Ok || bitmap == NULL) return;
    
    UINT width, height;
    GdipGetImageWidth(bitmap, &width);
    GdipGetImageHeight(bitmap, &height);
    Rect rect = {0, 0, width, height};

    // 鎖定點陣圖的記憶體區域以供讀取，PixelFormat32bppARGB的順序是A、R、G、B
    GdipBitmapLockBits(bitmap, &rect, ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
    
    // 建立一個緩衝區來複製像素資料
    int buffer_size = bitmapData.Stride * height;
    *imgData = (byte*) malloc(sizeof(byte) * buffer_size);
    memcpy(*imgData, bitmapData.Scan0, buffer_size);

    *width_ = width; *height_ = height; *chans_ = 4;

    // 解除點陣圖記憶體的鎖定
    GdipBitmapUnlockBits(bitmap, &bitmapData);
    // 點陣圖可以釋放了
    GdipDisposeImage(bitmap);
}