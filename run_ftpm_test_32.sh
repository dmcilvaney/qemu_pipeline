#!/bin/bash

set -e

mkdir -p logs

rm -f /tmp/ree.in /tmp/ree.out /tmp/tee.in /tmp/tee.out
mkfifo /tmp/ree.in /tmp/ree.out /tmp/tee.in /tmp/tee.out

echo "REE LOG:" > ./logs/ree.log
echo "TEE LOG:" > ./logs/tee.log
nohup cat /tmp/ree.out >> ./logs/ree.log 2>&1 &
nohup cat /tmp/tee.out >> ./logs/tee.log 2>&1 &

echo "QEMU LOG:" > ./logs/qemu.log

echo "Mounting directory at $QEMU_MOUNT_DIR"
QEMU_CMD="./qemu-system-arm \
    -nographic \
    -serial pipe:/tmp/ree -serial pipe:/tmp/tee \
    -smp 1 \
    -machine virt -machine secure=on -cpu cortex-a15 \
    -d unimp  -semihosting-config enable,target=native \
    -m 1057 \
    -bios bl1.bin \
    -fsdev local,id=fsdev0,path=$QEMU_MOUNT_DIR,security_model=none -device virtio-9p-device,fsdev=fsdev0,mount_tag=host"
echo "Command:  $QEMU_CMD" >> ./logs/qemu.log
nohup $QEMU_CMD >> ./logs/qemu.log 2>&1 &

# Wait for QEMU to boot
echo QEMU started
while ! grep "buildroot login:" ./logs/ree.log > /dev/null; do
    sleep 1
done

# Log into root account
echo Attempting to connect to QEMU at /tmp/ree.in
echo "root" > /tmp/ree.in
sleep 5

QEMU_TEST_FLAG="QEMU TEST COMPLETE"
QEMU_CMD=" sleep 5 && \
    mkdir /mnt/ci && \
    mount -t 9p -o trans=virtio sh0 /mnt/ci -oversion9p2000.L && \
    cp /mnt/ci/*.ta /lib/optee_armtz && \
    /mnt/ci/ftpm_test; echo $QEMU_TEST_FLAG"

echo "Running command:"
echo ""
echo $QEMU_CMD
echo ""
echo "$QEMU_CMD" > /tmp/ree.in
echo "Command sent"
#Seperate the results from the login and command logs
sleep 1

echo "RESULTS LOG:" > ./logs/tee.log
nohup tail -f -n 0 /tmp/ree.in >> ./logs/results.log 2>&1 &

echo "Waiting for tests to finish"
while ! grep "$QEMU_TEST_FLAG" ./logs/results.log > /dev/null; do
    sleep 5
done

if ! grep "fTPM TA selftest returned 0" ./logs/results.log > /dev/null; then
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
    
    rm -f /tmp/ree.in /tmp/ree.out /tmp/tee.in /tmp/tee.out
    exit 1
fi

rm -f /tmp/ree.in /tmp/ree.out /tmp/tee.in /tmp/tee.out
echo "Done!"
cat ./logs/results.log