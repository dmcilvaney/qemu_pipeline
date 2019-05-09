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

/* To the the UUID (found the the TA's h-file(s)) */
#define TA_FTPM_UUID { 0x53BAB89C, 0xB864, 0x4D7E, \
	{0xAC, 0xBC, 0x33, 0xC0, 0x7A, 0x9C, 0x1B, 0x8D}}

#define TA_FTPM_SUBMIT_COMMAND (0)
typedef uint32_t TPM_CC;
#define TPM_CC_SelfTest						(TPM_CC)(0x00000143)

typedef uint16_t TPM_ST;
#define TPM_ST_NO_SESSIONS					(TPM_ST)(0x8001)
typedef uint8_t TPMI_YES_NO;

#pragma pack(1)
typedef struct {
  TPM2_COMMAND_HEADER  Header;
  TPMI_YES_NO          FullTest;
} TPM2_SELF_TEST_COMMAND;

typedef struct {
  TPM2_RESPONSE_HEADER Header;
} TPM2_SELF_TEST_RESPONSE;
#pragma pack()

int main(void)
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_UUID uuid = TA_FTPM_UUID;
	uint32_t err_origin;

	UINT8 *SharedInput;
	UINT8 *SharedOutput;
	TEEC_Operation TeecOperation = {0};
	TEEC_Result TeecResult;
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

	uint32_t                          Status;
	TPM2_SELF_TEST_COMMAND            Cmd;
	TPM2_SELF_TEST_RESPONSE           Res;

	Cmd.Header.tag         = SwapBytes16(TPM_ST_NO_SESSIONS);
	Cmd.Header.paramSize   = SwapBytes32(sizeof(Cmd));
	Cmd.Header.commandCode = SwapBytes32(TPM_CC_SelfTest);
	Cmd.FullTest           = FullTest;

	ResultBufSize = sizeof(Res);
	Status = Tpm2SubmitCommand (sizeof(Cmd), (uint8_t *)&Cmd, &ResultBufSize, (uint8_t *)&Res);

	/*
	 * Run ftpm selftest
	 */
	TeecSharedMem.size = sizeof(Cmd) + sizeof(Res);
	res = TEEC_AllocateSharedMemory(&ctx, &TeecSharedMem);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_AllocateSharedMemory failed with code 0x%x origin 0x%x",
			res, err_origin);

	SharedInput = (UINT8 *)TeecSharedMem.buffer;
	SharedOutput = SharedInput + sizeof(Cmd);

	CopyMem(SharedInput, InputParameterBlock, InputParameterBlockSize);

	TeecOperation.params[0].tmpref.buffer = SharedInput;
	TeecOperation.params[0].tmpref.size = InputParameterBlockSize;
	TeecOperation.params[1].tmpref.buffer = SharedOutput;
	TeecOperation.params[1].tmpref.size = *OutputParameterBlockSize;
	TeecOperation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
											  TEEC_MEMREF_TEMP_INOUT,
											  TEEC_NONE,
											  TEEC_NONE);

	res = TEEC_InvokeCommand(&sess, TA_FTPM_SUBMIT_COMMAND, &TeecOperation,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
	printf("TA incremented value to %d\n", op.params[0].value.a);

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
