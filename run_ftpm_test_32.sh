#!/bin/bash

set -e

# Make sure to clean up background jobs
PIDS=""
function cleanup {
    echo "Killing pids: $PIDS"
    kill $PIDS
}
trap cleanup EXIT

# Local files are available in /mnt/ci/
TEST_COMMAND="cp /mnt/ci/*.ta /lib/optee_armtz && /mnt/ci/ftpm_test_32/ftpm_test"
GOOD_RESULT="fTPM TA selftest returned 0"

REE_PIPE=/tmp/ree_$RANDOM
TEE_PIPE=/tmp/tee_$RANDOM
echo Pipes: $REE_PIPE, $TEE_PIPE
rm -f $REE_PIPE.in $REE_PIPE.out $TEE_PIPE.in $TEE_PIPE.out
mkfifo $REE_PIPE.in $REE_PIPE.out $TEE_PIPE.in $TEE_PIPE.out

REE_LOG=./logs/ftpm_32_ree.log
TEE_LOG=./logs/ftpm_32_tee.log
QEMU_LOG=./logs/ftpm_32_qemu.log
RESULTS_LOG=./logs/ftpm_32_results.log

mkdir -p logs
echo "REE LOG:" > $REE_LOG
echo "TEE LOG:" > $TEE_LOG
cat $REE_PIPE.out >> $REE_LOG 2>&1 &
PIDS="${PIDS} $!"
cat $TEE_PIPE.out >> $TEE_LOG 2>&1 &
PIDS="${PIDS} $!"

echo "QEMU LOG:" > $QEMU_LOG

echo "Mounting directory at $QEMU_MOUNT_DIR"
QEMU_CMD="./qemu-system-arm \
    -nographic \
    -serial pipe:$REE_PIPE -serial pipe:$TEE_PIPE \
    -smp 1 \
    -machine virt -machine secure=on -cpu cortex-a15 \
    -d unimp -semihosting-config enable,target=native \
    -m 1057 \
    -bios bl1.bin \
    -fsdev local,id=fsdev0,path=$QEMU_MOUNT_DIR,security_model=none -device virtio-9p-device,fsdev=fsdev0,mount_tag=host"
echo "Command:  $QEMU_CMD" >> $QEMU_LOG
$QEMU_CMD >> $QEMU_LOG 2>&1 &
PIDS="${PIDS} $!"

# Wait for QEMU to boot
echo QEMU started
while ! grep "buildroot login:" $REE_LOG > /dev/null; do
    sleep 1
done

# Log into root account
echo Attempting to connect to QEMU at $REE_PIPE.in
echo "root" > $REE_PIPE.in
sleep 5

QEMU_MOUNT_CMD="mkdir /mnt/ci && \
    mount -t 9p -o trans=virtio host /mnt/ci"

echo "Mounting host files"
echo "$QEMU_MOUNT_CMD" > $REE_PIPE.in

QEMU_TEST_FLAG="QEMU TEST COMPLETE"
QEMU_TEST_CMD=" sleep 5 && \
    $TEST_COMMAND; \
    echo $QEMU_TEST_FLAG"

echo "Running test command:"
echo ""
echo $QEMU_TEST_CMD
echo ""
echo "$QEMU_TEST_CMD" > $REE_PIPE.in
echo "Command sent"
#Seperate the results from the login and command logs
sleep 1

echo "RESULTS LOG:" > $RESULTS_LOG
tail -f -n 0 $REE_LOG >> $RESULTS_LOG &
PIDS="${PIDS} $!"

echo "Waiting for tests to finish"
while ! grep "$QEMU_TEST_FLAG" $RESULTS_LOG > /dev/null; do
    sleep 1
    # tail won't update sometimes, force the file to update?
    cat $REE_LOG > /dev/null
done

if ! grep "$GOOD_RESULT" $RESULTS_LOG > /dev/null; then
    echo "Test failed"
    echo ""
    echo "REE:"
    echo ""
    cat $REE_LOG
    echo ""
    echo "TEE:"
    echo ""
    cat $TEE_LOG
    echo ""
    echo "Results:"
    echo ""
    cat $QEMU_LOG

    rm -f $REE_PIPE.in $REE_PIPE.out $TEE_PIPE.in $TEE_PIPE.out
    exit 1
fi

rm -f $REE_PIPE.in $REE_PIPE.out $TEE_PIPE.in $TEE_PIPE.out
echo "Done!"
cat $RESULTS_LOG