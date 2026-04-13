
wcharPtr char_to_wchar(const char* str) {
    int size_needed = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
    wcharPtr wstr = (wcharPtr)malloc(size_needed * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, size_needed);
    return wstr;
}

typedef struct{
    wcharPtr val; // 實際上涵蓋\0
    int length;   // 不計入\0
} string;

int len(wcharPtr str_){
    int k = 0;
    while(str_[k++]!=L'\0');
    return k;
}

void apply(string* self, wcharPtr str_){
    int length = len(str_);
    self->val = (wcharPtr) malloc(sizeof(wchar_t) * length);
    for (int i=0;i<length;i++) self->val[i] = str_[i];
    self->length = length - 1;
}
