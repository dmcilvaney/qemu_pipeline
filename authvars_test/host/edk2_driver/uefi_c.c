#include "uefi_types.h"

UINTN
StrSize (
  CONST CHAR16              *String
  )
{
  return (wcslen (String) + 1) * sizeof (*String);
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