#pragma once
#include "uefi_types.h"

extern GUID gOpteeAuthVarTaGuid;
extern GUID gEfiGlobalVariableGuid;
extern GUID gEfiSecureBootEnableDisableGuid;
extern GUID gEfiCustomModeEnableGuid;
extern GUID gEfiImageSecurityDatabaseGuid;
extern EFI_GUID gEfiSecureBootDbImageGuid;
extern EFI_GUID gEfiSecureBootKekImageGuid;

extern INT32 PcdMaxVariableSize;
extern INT32 PcdMaxHardwareErrorVariableSize;
extern BOOLEAN PcdSecureBootEnable;

typedef
EFI_STATUS
(EFIAPI *EFI_GET_VARIABLE)(
  IN     CHAR16                      *VariableName,
  IN     EFI_GUID                    *VendorGuid,
  OUT    UINT32                      *Attributes,    OPTIONAL
  IN OUT UINTN                       *DataSize,
  OUT    VOID                        *Data           OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_NEXT_VARIABLE_NAME)(
  IN OUT UINTN                    *VariableNameSize,
  IN OUT CHAR16                   *VariableName,
  IN OUT EFI_GUID                 *VendorGuid
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_VARIABLE)(
  IN  CHAR16                       *VariableName,
  IN  EFI_GUID                     *VendorGuid,
  IN  UINT32                       Attributes,
  IN  UINTN                        DataSize,
  IN  VOID                         *Data
  );

typedef
EFI_STATUS
(EFIAPI *EFI_QUERY_VARIABLE_INFO)(
  IN  UINT32            Attributes,
  OUT UINT64            *MaximumVariableStorageSize,
  OUT UINT64            *RemainingVariableStorageSize,
  OUT UINT64            *MaximumVariableSize
  );

typedef struct {
  //
  // Variable Services
  //
  EFI_GET_VARIABLE                GetVariable;
  EFI_GET_NEXT_VARIABLE_NAME      GetNextVariableName;
  EFI_SET_VARIABLE                SetVariable;
  //
  // Miscellaneous UEFI 2.0 Service
  //
  EFI_QUERY_VARIABLE_INFO         QueryVariableInfo;
} EFI_RUNTIME_SERVICES;

extern EFI_RUNTIME_SERVICES  *gRT;

#define  SIZE_1KB    0x00000400
#define  SIZE_2KB    0x00000800
#define  SIZE_4KB    0x00001000
#define  SIZE_8KB    0x00002000
#define  SIZE_16KB   0x00004000

#define PcdGet32(var) ((INT32)var)
#define PcdGetBool(var) ((BOOLEAN)var)

#define EFI_IMAGE_SECURITY_DATABASE_GUID \
  { \
    0xd719b2cb, 0x3d3a, 0x4596, { 0xa3, 0xbc, 0xda, 0xd0, 0xe, 0x67, 0x65, 0x6f } \
  }

///
/// Varialbe name with guid EFI_IMAGE_SECURITY_DATABASE_GUID
/// for the authorized signature database.
///
#define EFI_IMAGE_SECURITY_DATABASE       L"db"
///
/// Varialbe name with guid EFI_IMAGE_SECURITY_DATABASE_GUID
/// for the forbidden signature database.
///
#define EFI_IMAGE_SECURITY_DATABASE1      L"dbx"
///
/// Variable name with guid EFI_IMAGE_SECURITY_DATABASE_GUID
/// for the timestamp signature database.
///
#define EFI_IMAGE_SECURITY_DATABASE2      L"dbt"

#define SECURE_BOOT_MODE_ENABLE           1
#define SECURE_BOOT_MODE_DISABLE          0

#define SETUP_MODE                        1
#define USER_MODE                         0

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

#define EFI_SECURE_BOOT_ENABLE_NAME      L"SecureBootEnable"
#define SECURE_BOOT_ENABLE               1
#define SECURE_BOOT_DISABLE              0
#define EFI_CUSTOM_MODE_NAME          L"CustomMode"
#define CUSTOM_SECURE_BOOT_MODE       1
#define STANDARD_SECURE_BOOT_MODE     0