#!/bin/bash
nohup ./qemu-system-arm \
    -nographic \
    -serial pty -serial pty \
    -smp 1 \
    -machine virt -machine secure=on -cpu cortex-a15 \
    -d unimp  -semihosting-config enable,target=native \
    -m 1057 \
    -bios bl1.bin \
    -virtfs local,id=sh0,path=$QEMU_MOUNT_DIR,security_model=passthrough,readonly,mount_tag=sh0 \
    > 2>&1 | grep --max-count=1 -o "/dev/pts/[^ ]*" > pts.txt \

QEMU_PID=$!
QEMU_PTS=$(cat pts.txt)

echo QEMU started
sleep 20

echo Attempting to connect to QEMU
echo "root" > $QEMU_PTS
sleep 10

QEMU_CMD=" \
    mkdir /mnt/ci && \
    mount -t 9p -o trans=virtio sh0 /mnt/ci -oversion9p2000.L && \
    cp /mnt/ci/*.ta /lib/optee_armtz && \
    /mnt/ci/ftpm_test > test_results.txt && touch done_test || touch done_test \
"

echo "Running command"
echo $QEMU_CMD > $QEMU_PTS

while [ -f $QEMU_MOUNT_DIR/done_test ]; do
    echo "Waiting for tests to finish"
    sleep 5
done

if [ ! grep "fTPM TA selftest returned 0" $QEMU_MOUNT_DIR/test_results.txt ]; then
    echo "Test failed"
    kill $QEMU_PI
    exit 1
fi

echo "Done!"
kill $QEMU_PID