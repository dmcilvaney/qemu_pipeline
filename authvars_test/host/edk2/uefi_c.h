/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#pragma once
#include "uefi_types.h"

UINTN
StrSize (
  CONST CHAR16              *String
  );

INTN
StrCmp (
  CONST CHAR16              *FirstString,
  CONST CHAR16              *SecondString
  );

VOID
ZeroMem (
    IN VOID   *Buffer,
    IN UINTN  Size
    );

VOID
CopyMem (
  IN VOID   *Destination,
  IN VOID   *Source,
  IN UINTN  Length
  );

VOID *
AllocatePool (
  IN UINTN  AllocationSize
  );

VOID *
AllocateZeroPool (
  IN UINTN  AllocationSize
  );

VOID *
ReallocatePool (
  IN UINTN  OldSize,
  IN UINTN  NewSize,
  IN VOID   *OldBuffer  OPTIONAL
  );

VOID
FreePool (
  IN VOID   *Buffer
  );

#define MAX_ADDRESS   ((UINTN)(~0ULL >> (64 - sizeof (INTN) * 8)))
VOID *
SetMem (
  OUT VOID  *Buffer,
  IN UINTN  Length,
  IN UINT8  Value
  );

UINT16
SwapBytes16 (
    UINT16                    Value
    );

UINT32
SwapBytes32 (
    UINT32                    Value
    );