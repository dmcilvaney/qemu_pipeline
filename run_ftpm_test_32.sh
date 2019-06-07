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
TEST_COMMAND="cp /mnt/ci/*.ta /lib/optee_armtz && /mnt/ci/ftpm_test"
GOOD_RESULT="fTPM TA selftest returned 0"

REE_PIPE=/tmp/ree_32
TEE_PIPE=/tmp/tee_32
rm -f $REE_PIPE.in $REE_PIPE.out $TEE_PIPE.in $TEE_PIPE.out
mkfifo $REE_PIPE.in $REE_PIPE.out $TEE_PIPE.in $TEE_PIPE.out

mkdir -p logs
echo "REE LOG:" > ./logs/ree.log
echo "TEE LOG:" > ./logs/tee.log
cat $REE_PIPE.out >> ./logs/ree.log 2>&1 &
PIDS="${PIDS} $!"
cat $TEE_PIPE.out >> ./logs/tee.log 2>&1 &
PIDS="${PIDS} $!"

echo "QEMU LOG:" > ./logs/qemu.log

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
echo "Command:  $QEMU_CMD" >> ./logs/qemu.log
$QEMU_CMD >> ./logs/qemu.log 2>&1 &
PIDS="${PIDS} $!"

# Wait for QEMU to boot
echo QEMU started
while ! grep "buildroot login:" ./logs/ree.log > /dev/null; do
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

echo "RESULTS LOG:" > ./logs/results.log
tail -f -n 0 ./logs/ree.log >> ./logs/results.log &
PIDS="${PIDS} $!"

echo "Waiting for tests to finish"
while ! grep "$QEMU_TEST_FLAG" ./logs/results.log > /dev/null; do
    sleep 1
    # tail won't update sometimes, force the file to update?
    cat ./logs/ree.log > /dev/null
done

if ! grep "$GOOD_RESULT" ./logs/results.log > /dev/null; then
    echo "Test failed"
    echo ""
    echo "REE:"
    echo ""
    cat ./logs/ree.log
    echo ""
    echo "TEE:"
    echo ""
    cat ./logs/tee.log
    echo ""
    echo "Results:"
    echo ""
    cat ./logs/results.log
    
    rm -f $REE_PIPE.in $REE_PIPE.out $TEE_PIPE.in $TEE_PIPE.out
    exit 1
fi

rm -f $REE_PIPE.in $REE_PIPE.out $TEE_PIPE.in $TEE_PIPE.out
echo "Done!"
cat ./logs/results.log