/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

#include "edk2_driver/UEFIVarServices.h"
#include "edk2_driver/uefi_driver.h"

/* To the the UUID (found the the TA's h-file(s)) */
#define TA_AUTHVARS_UUID { 0x2d57c0f7, 0xbddf, 0x48ea, \
    {0x83, 0x2f, 0xd8, 0x4a, 0x1a, 0x21, 0x93, 0x01}}

TEEC_Context ctx;

// int runCommand(
// 	VARIABLE_SERVICE_OPS cmd,
// 	uint8_t *bufferTo,
// 	uint32_t bufferToSize,
// 	uint8_t *bufferFrom,
// 	uint32_t bufferFromSize,
// 	uint32_t *actualResultSize
// ) {
// 	TEEC_Result res;
// 	TEEC_Operation TeecOperation = {0};
// 	TEEC_SharedMemory TeecSharedMemTo = {0};
// 	TEEC_SharedMemory TeecSharedMemFrom = {0};
// 	uint8_t *SharedInput;
// 	uint8_t *SharedOutput;
// 	uint32_t err_origin;

// 	*actualResultSize = 0;

// 	// Allocate shared memory
// 	TeecSharedMemTo.size = bufferToSize;
// 	TeecSharedMemTo.flags = TEEC_MEM_INPUT;
// 	res = TEEC_AllocateSharedMemory(&ctx, &TeecSharedMemTo);
// 	if (res != TEEC_SUCCESS)
// 		errx(1, "TEEC_AllocateSharedMemory failed with code 0x%x origin 0x%x",
// 			res, err_origin);

// 	TeecSharedMemFrom.size = bufferFromSize;
// 	TeecSharedMemFrom.flags = TEEC_MEM_OUTPUT;
// 	res = TEEC_AllocateSharedMemory(&ctx, &TeecSharedMemFrom);
// 	if (res != TEEC_SUCCESS)
// 		errx(1, "TEEC_AllocateSharedMemory failed with code 0x%x origin 0x%x",
// 			res, err_origin);

// 	// Fill out parameters
// 	TeecOperation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
//         TEEC_MEMREF_TEMP_OUTPUT,
//         TEEC_VALUE_OUTPUT,
//         TEEC_NONE);

//     if (VariableParam != NULL) {
//         TeecOperation.params[0].tmpref.buffer = TeecSharedMemTo.buffer;
//         TeecOperation.params[0].tmpref.size = bufferToSize;
// 		memcpy(TeecOperation.params[0].tmpref.buffer, bufferTo, bufferToSize);
//     }

//     if (VariableResult != NULL) {
//         TeecOperation.params[1].tmpref.buffer = TeecSharedMemFrom.buffer;
//         TeecOperation.params[1].tmpref.size = bufferFromSize;
//     }

// 	// Invoke command
// 	res = TEEC_InvokeCommand(&sess, cmd, &TeecOperation,
// 				 &err_origin);
// 	if (res != TEEC_SUCCESS)

// 	*actualResultSize = (UINT32)TeecOperation.params[2].value.a;
// 	memcpy(bufferFrom, TeecSharedMemFrom.buffer, *actualResultSize);

// 	// Translate Errors
//     if (res != TEEC_SUCCESS) {
//         switch (res) {
//         case TEEC_ERROR_ITEM_NOT_FOUND:
// 			printf("TEEC not found");
// 			Status = EFI_NOT_FOUND;
// 			break;

// 		case TEEC_ERROR_SHORT_BUFFER:
// 			printf("TEEC short buffer");
// 			Status = EFI_BUFFER_TOO_SMALL;
// 			break;

// 		case TEEC_ERROR_OUT_OF_MEMORY:
// 			printf("TEEC out of memory");
// 			Status = EFI_OUT_OF_RESOURCES;
// 			break;

// 		case TEEC_ERROR_ACCESS_DENIED:
// 			printf("TEEC acccess denied");
// 			Status = EFI_SECURITY_VIOLATION;
// 			break;

// 		default:
// 			errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
// 				res, err_origin);
//         }
//     }
// }

// EFI_STATUS
// OpteeRuntimeGetVariable(
//   CHAR16 *VariableName,
//   EFI_GUID *VendorGuid,
//   UINT32 *Attributes OPTIONAL,
//   UINTN *DataSize,
//   VOID *Data
// )
// {
// 	UINT32 NameSize = (wcslen(VariableName) + 1) * sizeof(CHAR16);
// 	UINT32 ParamSize = sizeof(VARIABLE_GET_PARAM) + LocalVariableNameSize;
// 	UINT32 ResultSize = sizeof(VARIABLE_GET_RESULT) + *DataSize;
// 	VARIABLE_GET_PARAM *Param = malloc(ParamSize);
// 	VARIABLE_GET_RESULT *Result = malloc(ResultSize);
// 	UINT32 ActualSize = 0;
// 	EFI_STATUS Status;

// 	Param->Size = sizeof(VARIABLE_GET_PARAM);
// 	Param->VariableNameSize = (NameLength + 1) * sizeof(CHAR16);
// 	memcpy(Param->VendorGuid, VendorGuid, sizeof(Param->VendorGuid));
// 	memcpy(Param->VariableName, VariableName, sizeof(Param->VendorGuid));

// 	Status = runCommand( VSGetOp,
// 				(uint8_t*)Param,
// 				ParamSize,
// 				(uint8_t *)Result,
// 				uint32_t ResultSize,
// 				&ActualSize );

// 	if (Status == EFI_BUFFER_TOO_SMALL) {
// 		*DataSize = ActualSize;
// 	} else {
// 		*DataSize = VariableResult->GetResult.DataSize;
// 	}

// 	memcpy(Result->Data, Data, *DataSize);
// }

// EFI_STATUS
// OpteeRuntimeGetNextVariableName(
//   UINTN                             *VariableNameSize,
//   CHAR16                            *VariableName,
//   EFI_GUID                          *VendorGuid
// )
// {
// 	UINT32 NameSize = (wcslen(VariableName) + 1) * sizeof(CHAR16);
// 	UINT32 ParamSize = sizeof(VARIABLE_GET_PARAM) + LocalVariableNameSize;
// 	UINT32 ResultSize = sizeof(VARIABLE_GET_RESULT) + *DataSize;
// 	VARIABLE_GET_PARAM *Param = malloc(ParamSize);
// 	VARIABLE_GET_RESULT *Result = malloc(ResultSize);
// 	UINT32 ActualSize = 0;
// 	EFI_STATUS Status;

// 	Param->Size = sizeof(VARIABLE_GET_PARAM);
// 	Param->VariableNameSize = (NameLength + 1) * sizeof(CHAR16);
// 	memcpy(Param->VendorGuid, VendorGuid, sizeof(Param->VendorGuid));
// 	memcpy(Param->VariableName, VariableName, sizeof(Param->VendorGuid));

// 	Status = runCommand( VSGetOp,
// 				(uint8_t*)Param,
// 				ParamSize,
// 				(uint8_t *)Result,
// 				uint32_t ResultSize,
// 				&ActualSize );

// 	if (Status == EFI_BUFFER_TOO_SMALL) {
// 		*DataSize = ActualSize;
// 	} else {
// 		*DataSize = VariableResult->GetResult.DataSize;
// 	}

// 	memcpy(Result->Data, Data, *DataSize);
// }

int main(void)
{
	if(VariableAuthOpteeRuntimeInitialize()) {
		
	}
	OpteeRuntimeOnExitBootServices(NULL,NULL);
}
