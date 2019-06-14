/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#define TA_FTPM_UUID { 0x53BAB89C, 0xB864, 0x4D7E, \
	{0xAC, 0xBC, 0x33, 0xC0, 0x7A, 0x9C, 0x1B, 0x8D}}

#define TA_FTPM_SUBMIT_COMMAND (0)
typedef uint32_t TPM_CC;
#define TPM_CC_SelfTest						(TPM_CC)(0x00000143)

typedef uint16_t TPM_ST;
#define TPM_ST_NO_SESSIONS					(TPM_ST)(0x8001)
typedef uint8_t TPMI_YES_NO;
typedef uint16_t TPM_ST;

typedef uint32_t TPM_RC;
#define TPM_RC_SUCCESS           (TPM_RC)(0x000)

#pragma pack(1)
typedef struct {
  TPM_ST tag;
  uint32_t paramSize;
  TPM_CC commandCode;
} TPM2_COMMAND_HEADER;

typedef struct {
  TPM_ST tag;
  uint32_t paramSize;
  TPM_RC responseCode;
} TPM2_RESPONSE_HEADER;

typedef struct {
  TPM2_COMMAND_HEADER  Header;
  TPMI_YES_NO          FullTest;
} TPM2_SELF_TEST_COMMAND;

typedef struct {
  TPM2_RESPONSE_HEADER Header;
} TPM2_SELF_TEST_RESPONSE;
#pragma pack()

uint16_t
SwapBytes16 (
  uint16_t                    Value
  )
{
  return (uint16_t) ((Value<< 8) | (Value>> 8));
}

uint32_t
SwapBytes32 (
  uint32_t                    Value
  )
{
  uint32_t  LowerBytes;
  uint32_t  HigherBytes;

  LowerBytes  = (uint32_t) SwapBytes16 ((uint16_t) Value);
  HigherBytes = (uint32_t) SwapBytes16 ((uint16_t) (Value >> 16));
  return (LowerBytes << 16 | HigherBytes);
}

int main(void)
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_UUID uuid = TA_FTPM_UUID;
	uint32_t err_origin;

	uint8_t *SharedInput;
	uint8_t *SharedOutput;
	TEEC_Operation TeecOperation = {0};
	TEEC_SharedMemory TeecSharedMem = {0};

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	res = TEEC_OpenSession(&ctx, &sess, &uuid,
				   TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

	TPM2_SELF_TEST_COMMAND            Cmd;
	TPM2_SELF_TEST_RESPONSE           Res;

	Cmd.Header.tag         = SwapBytes16(TPM_ST_NO_SESSIONS);
	Cmd.Header.paramSize   = SwapBytes32(sizeof(Cmd));
	Cmd.Header.commandCode = SwapBytes32(TPM_CC_SelfTest);
	Cmd.FullTest           = 1;

	/*
	 * Run ftpm selftest
	 */
	TeecSharedMem.size = sizeof(Cmd) + sizeof(Res);
	TeecSharedMem.flags = (TEEC_MEM_INPUT | TEEC_MEM_OUTPUT);
	res = TEEC_AllocateSharedMemory(&ctx, &TeecSharedMem);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_AllocateSharedMemory failed with code 0x%x origin 0x%x",
			res, err_origin);

	SharedInput = (uint8_t *)TeecSharedMem.buffer;
	SharedOutput = SharedInput + sizeof(Cmd);

	memcpy(SharedInput, (uint8_t*)&Cmd, sizeof(Cmd));

	TeecOperation.params[0].tmpref.buffer = SharedInput;
	TeecOperation.params[0].tmpref.size = sizeof(Cmd);
	TeecOperation.params[1].tmpref.buffer = SharedOutput;
	TeecOperation.params[1].tmpref.size = sizeof(Res);
	TeecOperation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
											  TEEC_MEMREF_TEMP_INOUT,
											  TEEC_NONE,
											  TEEC_NONE);

	res = TEEC_InvokeCommand(&sess, TA_FTPM_SUBMIT_COMMAND, &TeecOperation,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);

	memcpy((uint8_t*)&Res, SharedOutput, sizeof(Res));
	printf("fTPM TA selftest returned %x\n", Res.Header.responseCode);

	if (Res.Header.responseCode != TPM_RC_SUCCESS)
		errx(1, "TPM failed with code 0x%x", Res.Header.responseCode);

	/*
	 * We're done with the TA, close the session and
	 * destroy the context.
	 *
	 * The TA will print "Goodbye!" in the log when the
	 * session is closed.
	 */

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return 0;
}
