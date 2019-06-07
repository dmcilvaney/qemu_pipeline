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
#include "edk2_variable_test/AuthVarTest.h"

/* To the the UUID (found the the TA's h-file(s)) */
#define TA_AUTHVARS_UUID { 0x2d57c0f7, 0xbddf, 0x48ea, \
    {0x83, 0x2f, 0xd8, 0x4a, 0x1a, 0x21, 0x93, 0x01}}

#include "edk2_variable_test/binaries.h"
int main(void)
{
	EFI_STATUS res;
	res = VariableAuthOpteeRuntimeInitialize();

	if (res != EFI_SUCCESS) {
		errx(1, "VariableAuthOpteeRuntimeInitialize failed with code 0x%x", res);
	}

	UefiMain();

	// Will stop future tests from running!
	//OpteeRuntimeOnExitBootServices(NULL,NULL);
}
