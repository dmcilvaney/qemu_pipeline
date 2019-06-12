#include "uefi_globals.h"

GUID gOpteeAuthVarTaGuid = { 0x2d57c0f7, 0xbddf, 0x48ea, { 0x83, 0x2f, 0xd8, 0x4a, 0x1a, 0x21, 0x93, 0x01 }};
GUID gEfiGlobalVariableGuid = { 0x8BE4DF61, 0x93CA, 0x11D2, { 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C }};
GUID gEfiSecureBootEnableDisableGuid    = { 0xf0a30bc7, 0xaf08, 0x4556, { 0x99, 0xc4, 0x0, 0x10, 0x9, 0xc9, 0x3a, 0x44 }};
GUID gEfiCustomModeEnableGuid           = { 0xc076ec0c, 0x7028, 0x4399, { 0xa0, 0x72, 0x71, 0xee, 0x5c, 0x44, 0x8b, 0x9f } };
GUID gEfiImageSecurityDatabaseGuid  = { 0xd719b2cb, 0x3d3a, 0x4596, {0xa3, 0xbc, 0xda, 0xd0,  0xe, 0x67, 0x65, 0x6f }};
EFI_GUID gEfiSecureBootDbImageGuid = {0};
EFI_GUID gEfiSecureBootKekImageGuid = {0};

INT32 PcdMaxVariableSize = 0x2000;
INT32 PcdMaxHardwareErrorVariableSize = 0x8000;
BOOLEAN PcdSecureBootEnable = FALSE;

EFI_RUNTIME_SERVICES  Services = {};
EFI_RUNTIME_SERVICES  *gRT = &Services;