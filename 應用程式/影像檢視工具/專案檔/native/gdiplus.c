
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

// --- 旋轉用 ---
typedef struct PropertyItem {
    PROPID  id;
    ULONG   length;
    WORD    type;
    VOID    *value;
} PropertyItem;

typedef enum RotateFlipType {
    RotateNoneFlipNone = 0,
    Rotate90FlipNone   = 1,
    Rotate180FlipNone  = 2,
    Rotate270FlipNone  = 3,
    RotateNoneFlipX    = 4,
    Rotate90FlipX      = 5,
    Rotate180FlipX     = 6,
    Rotate270FlipX     = 7
} RotateFlipType;

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

void setOrientationWithBitmap(GpBitmap* bitmap) {

    // PropertyTagOrientation 的標籤 ID
    const PROPID propId = 0x0112; 
    UINT size = 0;

    // 取得 EXIF 屬性的大小 (需要轉型成 GpImage*)
    GpStatus status = GdipGetPropertyItemSize((GpImage*)bitmap, propId, &size);
    if (status != Ok || size == 0) return;

    // 分配記憶體
    PropertyItem* propItem = memory(PropertyItem, size);
    if (propItem == NULL) return;

    // 讀取 EXIF 屬性內容
    status = GdipGetPropertyItem((GpImage*)bitmap, propId, size, propItem);
    if (status != Ok) goto exitOrientation;

    // 處理旋轉
    short orientation = *((short*)propItem->value);
    RotateFlipType rotateFlipType = RotateNoneFlipNone;
    switch (orientation) {
        case 2: rotateFlipType = RotateNoneFlipX;   break;
        case 3: rotateFlipType = Rotate180FlipNone; break;
        case 4: rotateFlipType = Rotate180FlipX;    break;
        case 5: rotateFlipType = Rotate90FlipX;     break;
        case 6: rotateFlipType = Rotate90FlipNone;  break;
        case 7: rotateFlipType = Rotate270FlipX;    break;
        case 8: rotateFlipType = Rotate270FlipNone; break;
    }
    if (rotateFlipType == RotateNoneFlipNone)  goto exitOrientation;

    GdipImageRotateFlip((GpImage*)bitmap, rotateFlipType); // 套用旋轉
    GdipRemovePropertyItem((GpImage*)bitmap, propId);      // 移除旋轉屬性
        
exitOrientation:
    free(propItem);
}

void imread_native(wcharPtr path, byte** imgData, int* width_, int* height_, int* chans_) {

    //宣告
    GpBitmap* bitmap = NULL; BitmapData bitmapData;
    // 出錯就返回
    if (GdipCreateBitmapFromFile(path, &bitmap) != Ok || bitmap == NULL) return;

    // 處理旋轉
    setOrientationWithBitmap(bitmap);

    // 處理長寬
    UINT width, height;
    GdipGetImageWidth(bitmap, &width);
    GdipGetImageHeight(bitmap, &height);

    // 鎖定點陣圖的記憶體區域以供讀取，PixelFormat32bppARGB的順序是A、R、G、B
    GdipBitmapLockBits(bitmap, &(Rect){0, 0, width, height}, ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
    
    // 建立一個緩衝區(緩衝區大小為 bitmapData.Stride * height)，並複製像素資料到緩衝區
    *imgData = memory(byte, bitmapData.Stride * height);
    memcpy(*imgData, bitmapData.Scan0, (bitmapData.Stride * height));
    
    // 長、寬
    *width_ = width; *height_ = height; *chans_ = 4;
    // 解除點陣圖記憶體的鎖定
    GdipBitmapUnlockBits(bitmap, &bitmapData);
    // 點陣圖可以釋放了
    GdipDisposeImage(bitmap);
}