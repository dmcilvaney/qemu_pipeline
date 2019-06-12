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

// commands:
//	selftest (default)	Runs UEFI test application
//	bin					Sets then gets the contents of a compiled in binary object (see test_compile_time_buffer() )
//	set					file_name variable_name guid attributes...
//	get					variable_name guid
//	getnext				(optional)[variable_name guid]
//	query				attributes...

//		attributes:
//			NV = EFI_VARIABLE_NON_VOLATILE
//			BS = EFI_VARIABLE_BOOTSERVICE_ACCESS
//			RT = EFI_VARIABLE_RUNTIME_ACCESS
//			HE = EFI_VARIABLE_HARDWARE_ERROR_RECORD
//			AW = EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
//			TA = EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS
//			AP = EFI_VARIABLE_APPEND_WRITE

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

typedef enum {GET, SET, GETNEXT, QUERY, SELF_TEST, BIN, UNKNOWN_COMMAND} NV_op;

void print_guid(EFI_GUID guid) {
	wprintf(L"{ 0x%08lX, 0x%04hX, 0x%04hX, { 0x%02hhX, 0x%02hhX, 0x%02hhX, 0x%02hhX, 0x%02hhX, 0x%02hhX, 0x%02hhX, 0x%02hhX } }",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
}

void print_buffer(char *data, unsigned int size) {
	int i;
	for(i = 0; i < size; i++) {
		wprintf(L"%02x ", data[i]);
		 if (!((i + 1) % 16)) {
			wprintf(L"\n");
		} else if (!((i + 1) % 8)) {
			wprintf(L"  ");
		}
	}
	wprintf(L"\n");
}

void print_bytes(char *data, unsigned int size) {
	int i;
	wprintf(L"printf \"\%b\" \'");
	for(i = 0; i < size; i++) {
		wprintf(L"\\x%02x", data[i]);
	}
	wprintf(L"\'\n");
}

char *read_file(char *file_name, unsigned int *file_size) {
	char *file_data;
	unsigned int read_size;
	FILE *file;

	if (file_name) {
		wprintf(L"Reading from file %s\n", file_name);
		file = fopen(file_name, "rb");
		if (!file) {
			errx(1, "fopen failed!");
		}
		fseek(file, 0, SEEK_END);
		*file_size = ftell(file);
		rewind(file);

		wprintf(L"file is 0x%x bytes\n", *file_size);

		file_data = malloc(*file_size);
		if (!file_data) {
			errx(1, "malloc fail");
		}

		read_size = fread(file_data, 1, *file_size, file);
		if (read_size != *file_size) {
			errx(1, "read fail");
		}
		wprintf(L"Successfully read 0x%x bytes\n", read_size);
	}
	return file_data;
}

int process_command(const char *data_in, unsigned int data_size, NV_op op, EFI_GUID *guid, CHAR16 *variable_name, unsigned int attributes) {


	int res = 0;
	unsigned int name_length = 0;
	EFI_GUID empty_guid = {0};
	

	if (variable_name) {
		name_length = wcslen(variable_name);
	}

	switch (op) {
		case GET:
			;
			{
				unsigned int required_size = 0;
				char *data;
				res = OpteeRuntimeGetVariable(variable_name,
												guid,
												&attributes,
												&required_size,
												0);

				if (res == EFI_BUFFER_TOO_SMALL) {
					data = malloc(required_size);
					res = OpteeRuntimeGetVariable(variable_name,
													guid,
													&attributes,
													&required_size,
													data);
					print_guid(*guid);
					wprintf(L"   :   \"%S\" attr:0x%x\n", variable_name, attributes);
					print_buffer(data, required_size);
					print_bytes(data, required_size);
					free(data);
				}
				return res;
			}
		case SET:
			;
			{
				char *data_temp = malloc(data_size);
				memcpy(data_temp, data_in, data_size);
				res = OpteeRuntimeSetVariable(variable_name,
										guid,
										attributes,
										data_size,
										data_temp);
				free (data_temp);
				return res;
			}
		case GETNEXT:
			;
			{
				CHAR16 *temp_name = calloc(name_length + 1, sizeof(CHAR16));
				unsigned int name_bytes = (name_length + 1) * sizeof(CHAR16);
				if (variable_name) {
				 	wcscpy(temp_name, variable_name);
				}
				if (!guid) {
					guid = &empty_guid;
				}
				do {
					temp_name = realloc(temp_name, name_bytes);
					res = OpteeRuntimeGetNextVariableName(
						&name_bytes,
						temp_name,
						guid
					);
				} while (res == EFI_BUFFER_TOO_SMALL);
				wprintf(L"Run next: \n\t./authvars_test getnext \"%S\" \"", temp_name);
				print_guid(*guid);
				wprintf(L"\"\n");
				free(temp_name);
				return res;
			}
		case QUERY:
			;
			{
				UINT64 MaximumVariableStorageSize;
				UINT64 RemainingVariableStorageSize;
				UINT64 MaximumVariableSize;
				res = OpteeRuntimeQueryVariableInfo(attributes,
												&MaximumVariableStorageSize,
												&RemainingVariableStorageSize,
												&MaximumVariableSize);
				wprintf(L" max storage size: %llu, remaining: %llu, largest variable: %llu\n",
							MaximumVariableStorageSize,
							RemainingVariableStorageSize,
							MaximumVariableSize);
				return res;
			}
		default:
			wprintf(L"Unknown op\n");
			return -1;
	}
}

unsigned int build_attributes(char *attribute_strings[] , int num)
{
	int i = 0;
	unsigned int attributes = 0;
	wprintf(L"Processing %d attributes\n", num);
	for(i = 0; i < num; i++) {
		char *attr = attribute_strings[i];
		wprintf(L"+ %s\n", attr);
		if(!strcmp(attr, "NV")) {attributes |= EFI_VARIABLE_NON_VOLATILE;}
		else if(!strcmp(attr, "BS")) {attributes |= EFI_VARIABLE_BOOTSERVICE_ACCESS;}
		else if(!strcmp(attr, "RT")) {attributes |= EFI_VARIABLE_RUNTIME_ACCESS;}
		else if(!strcmp(attr, "HE")) {attributes |= EFI_VARIABLE_HARDWARE_ERROR_RECORD;}
		else if(!strcmp(attr, "AW")) {attributes |= EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS;}
		else if(!strcmp(attr, "TA")) {attributes |= EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;}
		else if(!strcmp(attr, "AP")) {attributes |= EFI_VARIABLE_APPEND_WRITE;}
		else {
			wprintf(L"Unkown attribute %s\n", attr);
			wprintf(L"NV = EFI_VARIABLE_NON_VOLATILE\n");
			wprintf(L"BS = EFI_VARIABLE_BOOTSERVICE_ACCESS\n");
			wprintf(L"RT = EFI_VARIABLE_RUNTIME_ACCESS\n");
			wprintf(L"HE = EFI_VARIABLE_HARDWARE_ERROR_RECORD\n");
			wprintf(L"HE = EFI_VARIABLE_HARDWARE_ERROR_RECORD\n");
			wprintf(L"AW = EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS\n");
			wprintf(L"TA = EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS\n");
			wprintf(L"AP = EFI_VARIABLE_APPEND_WRITE\n");
			errx(1, "Unknown attributes!");
		}
	}
	return attributes;
}

NV_op set_op(char* operation) {
	if(!strcmp(operation, "selftest")) {return SELF_TEST;}
	else if(!strcmp(operation, "bin")) {return BIN;}
	else if(!strcmp(operation, "get")) {return GET;}
	else if(!strcmp(operation, "set")) {return SET;}
	else if(!strcmp(operation, "query")) {return QUERY;}
	else if(!strcmp(operation, "getnext")) {return GETNEXT;}
	else {return UNKNOWN_COMMAND;}
}


EFI_GUID get_guid(char *str) {
	EFI_GUID guid = {0};
	wprintf(L"Parsing GUID %s\n", str);

	//strip spaces
	char *in = str;
	char *out = str;
	while(*in != '\0') {
		if (*in != ' ') {
			*out = *in;
			out++;
		}
		in++;
	}
	*out = '\0';
	sscanf(str, "{0x%X,0x%hX,0x%hX,{0x%hhX,0x%hhX,0x%hhX,0x%hhX,0x%hhX,0x%hhX,0x%hhX,0x%hhX}}",
		&guid.Data1, &guid.Data2, &guid.Data3,
		&guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3],
		&guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7]);
	print_guid(guid);
	wprintf(L"\n");
	return guid;
}


// Pulls hello_world.bin from the build directory at compile time, sets it, then gets it.
// Add "hello_world.blob" to the makefile variable $(BLOBS) to create a linkable object from
// hello_world.bin.
//		i.e. hello_world.bin -> hello_world.blob -> ./authvars_test -> char *hello_world; unsigned int hello_world_size;
EXTERNAL_BIN(hello_world);
#define TEST_BIN_VAR_NAME L"test_bin_var"
#define TEST_BIN_GUID { 0x0d1a33d0, 0x0c94, 0x4ba5, { 0xaf, 0x22, 0xe1, 0xdc, 0xcb, 0x64, 0x70, 0xd3}}
EFI_STATUS test_compile_time_buffer() {
	EFI_GUID guid = TEST_BIN_GUID;
	EFI_STATUS ret = EFI_SUCCESS;
	ret = process_command(hello_world, hello_world_size, SET, &guid,TEST_BIN_VAR_NAME,(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS));
	if (ret) {
		return ret;
	}
	ret = process_command(0, 0,GETNEXT,0,0,0);
	if (ret) {
		return ret;
	}
	ret = process_command(0,0,GET,&guid,TEST_BIN_VAR_NAME,0);
	return ret;
}

int main(int argc, char *argv[])
{
	EFI_STATUS res;
	res = VariableAuthOpteeRuntimeInitialize();
	if (res != EFI_SUCCESS) {
		errx(1, "VariableAuthOpteeRuntimeInitialize failed with code 0x%x", res);
	}

	int argnum = 1;
	NV_op operation = SELF_TEST;
	if (argc > argnum) {
		// Processing commands, otherwise proceed with default self_test
		char *command = argv[argnum++];
		operation = set_op(command);
	}

	switch(operation) {
		case SELF_TEST:
			return UefiMain();
		case BIN:
			return test_compile_time_buffer();
		case GET:
			{
				wprintf(L"GET\n");
				if(argc > argnum) {
					char *variable_name = argv[argnum++];
					unsigned int name_length = strlen(variable_name);
					unsigned int w_size = (name_length + 1) * sizeof(CHAR16);
					CHAR16 *w_variable_name = malloc(w_size);
					swprintf(w_variable_name, name_length + 1, L"%s", variable_name);

					if (argc > argnum) {
						EFI_GUID guid = get_guid(argv[argnum++]);

						res = process_command(0,0,GET,&guid,w_variable_name,0);
						free(w_variable_name);
						return res;
					}
					free(w_variable_name);
				}
				errx(1, "\n\t example useage: authvars_test get var_name {0x0d1a33d0,0x0c94,0x4ba5,{0xaf,0x22,0xe1,0xdc,0xcb,0x64,0x70,0xd3}}");
			}
		case SET:
			{
				wprintf(L"SET\n");
				if(argc > argnum) {
					char *file_name = argv[argnum++];
					unsigned int data_size;
					char *data = read_file(file_name, &data_size);

					if(argc > argnum) {

						char *variable_name = argv[argnum++];
						unsigned int name_length = strlen(variable_name);
						unsigned int w_size = (name_length + 1) * sizeof(CHAR16);
						CHAR16 *w_variable_name = malloc(w_size);
						swprintf(w_variable_name, name_length + 1, L"%s", variable_name);
						
						if (argc > argnum) {
							EFI_GUID guid = get_guid(argv[argnum++]);
							unsigned int attributes = 0;
							attributes = build_attributes(&(argv[argnum]), (argc - argnum) );

							res = process_command(data,data_size,SET,&guid,w_variable_name,attributes);
							return res;
							free(w_variable_name);
						}
						free(w_variable_name);
					}
					free(data);
				}
				errx(1, "\n\t example useage: authvars_test set hello_world.bin var_name {0x0d1a33d0,0x0c94,0x4ba5,{0xaf,0x22,0xe1,0xdc,0xcb,0x64,0x70,0xd3}} NV RT BS");
			}
		case QUERY:
			{
				wprintf(L"QUERY\n");
				if(argc > argnum) {
					unsigned int attributes = 0;
					attributes = build_attributes(&(argv[argnum]), (argc - argnum) );
					res = process_command(0,0,QUERY,0,0,attributes);
					return res;
				}
				errx(1, "\n\t example useage: authvars_test query NV RT BS");
			}
		case GETNEXT:
			{
				wprintf(L"GETNEXT\n");
				// need to pull two off the top
				if(argc == argnum + 2) {
					char *variable_name = argv[argnum++];
					unsigned int name_length = strlen(variable_name);
					unsigned int w_size = (name_length + 1) * sizeof(CHAR16);
					CHAR16 *w_variable_name = malloc(w_size);
					swprintf(w_variable_name, name_length + 1, L"%s", variable_name);
					
					if (argc > argnum) {
						EFI_GUID guid = get_guid(argv[argnum++]);

						res = process_command(0,0,GETNEXT,&guid,w_variable_name,0);
						return res;
						free(w_variable_name);
					}
					free(w_variable_name);
				} else if (argc == argnum) {
					res = process_command(0,0,GETNEXT,0,0,0);
					return res;
				} else {
					errx(1, "\n\t example useage: authvars_test query"
						"\n\t\t or "
						"\n\t\t\tauthvars_test query var_name {0x0d1a33d0,0x0c94,0x4ba5,{0xaf,0x22,0xe1,0xdc,0xcb,0x64,0x70,0xd3}}\n");
				}

			}
		default:
			errx(1, "\n\tUnable to parse input, valid commands are \"\", \"selftest\", \"bin\", \"get\" \"set\", \"getnext\", \"query\"");
	}

	OpteeRuntimeOnExitBootServices(NULL,NULL);
}
