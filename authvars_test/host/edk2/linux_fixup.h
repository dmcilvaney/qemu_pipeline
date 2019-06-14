/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#pragma once

#include <stdlib.h>
#include <setjmp.h>
typedef jmp_buf BASE_LIBRARY_JUMP_BUFFER;
#define SetJump(env) setjmp(*env)
#define LongJump(env,val) longjmp(*env,val)

#include <time.h>
#define GetPerformanceCounter clock
#define GetPerformanceCounterProperties(a,b) ((UINT64)CLOCKS_PER_SEC)

#include <stdio.h>
#define Print wprintf
#define UnicodeSPrint swprintf

#include "uefi_types.h"
void FixWideString(wchar_t *In, INT16 *Out);
void RestoreWideString(INT16 *In, wchar_t *Out);