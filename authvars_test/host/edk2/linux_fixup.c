#include "uefi_types.h"


void FixWideString(wchar_t *In, INT16 *Out) {
    wchar_t *wc = In;
    INT16 *sc = Out;
    int length = 0;
    do {
        *sc = (INT16)(*wc);
        //wprintf(L"W:%x->S:%x\n", (INTN)*wc, (INTN)*sc);
        sc++;
        length++;
    } while (*(wc++));
    //wprintf(L"fix wide, shrunk %d characters\n", length);
}

#define DELTA(a,b) ((INT32)((INT32)a - (INT32)b))

void RestoreWideString(INT16 *In, wchar_t *Out) {
    wchar_t *wc = 0;
    INT16 *sc = (INT16*)In;
    INT32 length = 0;
    while(*sc) {
        length++;
        sc++;
    }
    //wprintf(L"len:%d\n", length);
    wc = &(Out[length]);

    //wprintf(L"0x%x, 0x%x\n", (INTN)Out, (INTN)wc);

    while(sc >= In) {
        //wprintf(L"%d -- ",DELTA(wc,Out));
        *wc = *sc;
        //wprintf(L"S:%x->W:%x\n", (INTN)*sc, (INTN) *wc);
        wc--;
        sc--;
    }

    //wprintf(L"0x%x, 0x%x\n", (INTN)Out, (INTN)(wc+1));
}