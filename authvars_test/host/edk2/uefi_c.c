#include "uefi_c.h"
#include <stdlib.h>

UINTN
StrSize (
  CONST CHAR16              *String
  )
{
  return (wcslen (String) + 1) * sizeof (*String);
}

INTN
StrCmp (
  CONST CHAR16              *FirstString,
  CONST CHAR16              *SecondString
  )
{
  return wcscmp(FirstString,SecondString);
}

VOID
ZeroMem (
    IN VOID   *Buffer,
    IN UINTN  Size
    )
{
    memset(Buffer, 0, Size);
}

VOID
CopyMem (
  IN VOID   *Destination,
  IN VOID   *Source,
  IN UINTN  Length
  )
{
  memcpy (Destination, Source, Length);
}

VOID *
SetMem (
  OUT VOID  *Buffer,
  IN UINTN  Length,
  IN UINT8  Value
  )
{
  if (Length == 0) {
    return Buffer;
  }

  ASSERT ((Length - 1) <= (MAX_ADDRESS - (UINTN)Buffer));

  return memset (Buffer, Length, Value);
}

VOID *
AllocatePool (
  IN UINTN  AllocationSize
  )
{
  return (VOID*) malloc (AllocationSize);
}

VOID *
AllocateZeroPool (
  IN UINTN  AllocationSize
  )
{
  return calloc(AllocationSize, sizeof(BYTE));
}

VOID *
EFIAPI
ReallocatePool (
  IN UINTN  OldSize,
  IN UINTN  NewSize,
  IN VOID   *OldBuffer  OPTIONAL
  )
{
  if(OldBuffer) {
    return realloc(OldBuffer, NewSize);
  } else {
    return AllocateZeroPool(NewSize);
  }
}

VOID
FreePool (
  IN VOID   *Buffer
  )
{
  free ((void *) Buffer);
}

UINT16
SwapBytes16 (
    UINT16                    Value
    )
{
    return (UINT16) ((Value<< 8) | (Value>> 8));
}

UINT32
SwapBytes32 (
    UINT32                    Value
    )
{
    UINT32  LowerBytes;
    UINT32  HigherBytes;

    LowerBytes  = (UINT32) SwapBytes16 ((UINT16) Value);
    HigherBytes = (UINT32) SwapBytes16 ((UINT16) (Value >> 16));
    return (LowerBytes << 16 | HigherBytes);
}