/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

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
    INT32 length = 1; // At least a null char
    while(*sc) {
        length++;
        sc++;
    }
    wc = &(Out[length - 1]);

    while(sc >= In) {
        //wprintf(L"%d -- ",DELTA(wc,Out));
        *wc = *sc;
        wc--;
        sc--;
    }

    //wprintf(L"0x%x, 0x%x\n", (INTN)Out, (INTN)(wc+1));
}