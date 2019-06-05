#ifndef UEFI_DRIVER_H
#define UEFI_DRIVER_H
#pragma once
#pragma message "HELLO!"

#include "uefi_types.h"
#include <tee_client_api.h>

extern EFI_RUNTIME_SERVICES  *gRT;

#include <stdio.h>
#include <string.h>
#define LOG_INFO(...) {printf("INFO:"); printf(__VA_ARGS__);}
#define LOG_VANILLA_TRACE(...) {printf("VANILLA_TRACE:"); printf(__VA_ARGS__);}
#define LOG_TRACE(...) {printf("TRACE:"); printf(__VA_ARGS__);}
#define LOG_ERROR(...) {printf("ERROR:"); printf(__VA_ARGS__);}
#define DEBUG_STRIP(lvl,...) LOG_ERROR(__VA_ARGS__)
#define DEBUG(expr) DEBUG_STRIP expr

extern GUID gOpteeAuthVarTaGuid;
extern GUID gEfiGlobalVariableGuid;
extern GUID gEfiSecureBootEnableDisableGuid;
extern GUID gEfiCustomModeEnableGuid;
///
/// Whether the system is operating in setup mode (1) or not (0).
/// All other values are reserved. Should be treated as read-only.
/// Its attribute is BS+RT.
///
#define EFI_SETUP_MODE_NAME                         L"SetupMode"
///
/// The Key Exchange Key Signature Database.
/// Its attribute is NV+BS+RT+AT.
///
#define EFI_KEY_EXCHANGE_KEY_NAME                   L"KEK"
///
/// The public Platform Key.
/// Its attribute is NV+BS+RT+AT.
///
#define EFI_PLATFORM_KEY_NAME                       L"PK"
///
/// Whether the platform firmware is operating in Secure boot mode (1) or not (0).
/// All other values are reserved. Should be treated as read-only.
/// Its attribute is BS+RT.
///
#define EFI_SECURE_BOOT_MODE_NAME                   L"SecureBoot"

#define SETUP_MODE                        1
#define USER_MODE                         0
#define SECURE_BOOT_MODE_ENABLE           1
#define SECURE_BOOT_MODE_DISABLE          0
#define EFI_SECURE_BOOT_ENABLE_NAME      L"SecureBootEnable"
#define SECURE_BOOT_ENABLE               1
#define SECURE_BOOT_DISABLE              0
#define EFI_CUSTOM_MODE_NAME          L"CustomMode"
#define CUSTOM_SECURE_BOOT_MODE       1
#define STANDARD_SECURE_BOOT_MODE     0

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