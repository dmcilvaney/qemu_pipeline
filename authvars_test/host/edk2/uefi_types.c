/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#include "uefi.h"

BOOLEAN
CompareGuid (
  IN CONST GUID  *Guid1,
  IN CONST GUID  *Guid2
  )
{
  return (!memcmp((void*)Guid1,(void*)Guid2, sizeof(GUID)));
}