QEMU testing infrastructure for security firmware TAs (fTPM, Authvars)
===

Builds QEMU (or extracts the requried binaries from an archive if present), and automates the testing of TAs. The QEMU build process is quite time consuming, but should only be required once (if an archive is not already present).

## Basic instructions
All commands will automatically build/extract the required QEMU binaries before running.

#### make
    Defaults to running test_32

#### make test_32
#### make test_64
    Run automated tests against TAs as defined by the *_test_<32/64>.sh files.
    The *.ta files must be pressent in ./run/qemu_<32/64>_mnt to run successfully.

    Logs are generated in qemu_build/run_<32/64>/logs.

#### make run_32
#### make run_64
    Start an interactive QEMU session. QEMU can be accessed through pseudo terminals. The specific terminals will be printed by QEMU when it starts. The first terminal connects to the normal world, while the second connects to secure world.

    Instructions for accessing the emulator are printed by this command just before starting QEMU.

# Included applications

## Building the test applications
#### make -f build_tests.mk
    This will build the tests and palce them into ./tests

    The scripts require the TEEC exports to build, this can be set by TEEC_EXPORT_PATH=..., or use the default QEMU TEEC (requires checking out all of QEMU, which is slow).

## authvars_test
This application defaults to running the EDK2 authenticated variable tests.

It can also be run in interactive mode with the commands `./authvar_test <get,set,getnext,query,bin,selftest>`

#### ./authvar_test get   variable_name guid
    The get command outputs a binary representation of the data, followed by a formatted print command (The QEMU mount is read-only). Pasting the print command into a bash terminal will generate a file matching the contents of the variable.
#### ./authvar_test set   file_name variable_name guid attributes...
#### ./authvar_test getnext   (optional)[variable_name guid]

    Pass no additional parameters for the frist call to getnext, the command will show what the next command should be to continue iterating through the varaibles.
#### ./authvar_test query attributes <...>

#### ./authvar_test bin
    Runs a short automated test based on pre-compiled binaries (see test_compile_time_buffer() in main.c)
#### ./authvar_test selftest (default)
    Run the UEFI test infrastructure

## ftpm_test
    Simply runs the TPM self test command.