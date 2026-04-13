
typedef enum {
    RGB,Gray,RGBA,BGR,BGRA,noFormat
}PixelFormat;

typedef struct {
    byte* data;         // 指向原始記憶體
    int width;
    int height;
    int channels;
    PixelFormat pixelFormat;
} Image;

typedef struct {
    int** data; // EX: RGB是data[3]、Gray是data[1]
    Image* img;
    int channels;
    PixelFormat pixelFormat;
} Histagram;

Image* initImage(int w, int h, int chans, PixelFormat format){
    Image* img = (Image*) malloc(sizeof(Image) * 1);
    img->data = (byte*) malloc(sizeof(byte) * w * h * chans);
    img->width = w;
    img->height = h;
    img->channels = chans;
    img->pixelFormat = format;
    return img;
}

void DisposeImage(Image* self){
    if(self == NULL) return;
    self->width = self->height = self->channels = 0;
    self->pixelFormat = noFormat;
    free(self->data);
}

Image* GlobalImage = NULL;