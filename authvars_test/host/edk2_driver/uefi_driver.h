#ifndef UEFI_DRIVER_H
#define UEFI_DRIVER_H
#pragma once

#include "edk2/uefi.h"
#include <tee_client_api.h>

#include <stdio.h>
#include <string.h>
#define LOG_LEVEL 4
#define LOG_INFO(...) {if(LOG_LEVEL >= 2){wprintf(L"(%s:%d)INFO:", __FILE__,__LINE__); wprintf(L ## __VA_ARGS__);wprintf(L"\n");}}
#define LOG_VANILLA_TRACE(...) {if(LOG_LEVEL >= 3){wprintf(L"(%s:%d)VANILLA_TRACE:", __FILE__,__LINE__); wprintf(L ## __VA_ARGS__);wprintf(L"\n");}}
#define LOG_TRACE(...) {if(LOG_LEVEL >= 4){wprintf(L"(%s:%d)TRACE:", __FILE__,__LINE__); wprintf(L ## __VA_ARGS__);wprintf(L"\n");}}
#define LOG_ERROR(...) {if(LOG_LEVEL >= 1){wprintf(L"(%s:%d)ERROR:", __FILE__,__LINE__); wprintf(L ## __VA_ARGS__);wprintf(L"\n");}}
#define DEBUG_STRIP(lvl,...) LOG_INFO(__VA_ARGS__)
#define DEBUG(expr) DEBUG_STRIP expr

static
inline
BOOLEAN
EfiAtRuntime() {
  return FALSE;
}

static
inline
VOID
SecureBootHook( CHAR16 *VariableName, EFI_GUID *VendorGuid) {
  (void)VariableName;
  (void)VendorGuid;
}

EFI_STATUS
EFIAPI
TaOpenSession(
  VOID
);

EFI_STATUS
EFIAPI
OpteeRuntimeQueryVariableInfo(
  IN  UINT32                                Attributes,
  OUT UINT64                                *MaximumVariableStorageSize,
  OUT UINT64                                *RemainingVariableStorageSize,
  OUT UINT64                                *MaximumVariableSize
);

EFI_STATUS
EFIAPI
OpteeRuntimeSetVariable(
  IN CHAR16     *VariableName,
  IN EFI_GUID   *VendorGuid,
  IN UINT32     Attributes,
  IN UINTN      DataSize,
  IN VOID       *Data
);

EFI_STATUS
EFIAPI
OpteeRuntimeGetNextVariableName(
  IN OUT  UINTN                             *VariableNameSize,
  IN OUT  CHAR16                            *VariableName,
  IN OUT  EFI_GUID                          *VendorGuid
);

EFI_STATUS
EFIAPI
OpteeRuntimeGetVariable(
  IN      CHAR16                            *VariableName,
  IN      EFI_GUID                          *VendorGuid,
  OUT     UINT32                            *Attributes OPTIONAL,
  IN OUT  UINTN                             *DataSize,
  OUT     VOID                              *Data
);

EFI_STATUS
EFIAPI
OpteeRuntimeVariableInvokeCommand(
  IN VARIABLE_SERVICE_OPS  Command,
  IN UINT32                VariableParamSize,
  IN TEEC_SharedMemory     *VariableParam,
  IN UINT32                VariableResultSize,
  IN TEEC_SharedMemory     *VariableResult,
  OUT UINT32               *ResultSize,
  OUT UINT32               *AuthVarStatus
);

VOID
EFIAPI
OpteeRuntimeOnExitBootServices(
  IN  EFI_EVENT  Event,
  IN VOID       *Context
);

EFI_STATUS
EFIAPI
VariableAuthOpteeRuntimeInitialize(
  // IN EFI_HANDLE         ImageHandle,
  // IN EFI_SYSTEM_TABLE   *SystemTable
);

#endif