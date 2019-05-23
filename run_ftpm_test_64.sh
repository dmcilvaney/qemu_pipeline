#!/bin/bash
mkdir -p logs
nohup ./qemu-system-aarch64 \
    -nographic \
    -serial pty -serial pty \
    -smp 1 \
    -machine virt,secure=on -cpu cortex-a57 \
    -d unimp -semihosting-config enable,target=native \
    -m 1057 \
    -bios bl1.bin \
    -initrd rootfs.cpio.gz -kernel Image -no-acpi \
    -append 'console=ttyAMA0,38400 keep_bootcon root=/dev/vda2' \
    -virtfs local,id=sh0,path=$QEMU_MOUNT_DIR,security_model=passthrough,readonly,mount_tag=sh0 > ./logs/qemu.log 2>&1 &
    
#-netdev user,id=vmnic -device virtio-net-device,netdev=vmnic


sleep 1
QEMU_PTS=$(grep --max-count=2 -o "/dev/pts/[^ ]*" ./logs/qemu.log | sed -n 1p)
TEE_PTS=$(grep --max-count=2 -o "/dev/pts/[^ ]*" ./logs/qemu.log | sed -n 2p)

nohup cat $QEMU_PTS > ./logs/ree.log 2>&1 &
CAT_PID=$!
nohup cat $TEE_PTS > ./logs/tee.log 2>&1 &

# Wait for QEMU to boot
echo QEMU started
sleep 20

# Log into root account
echo Attempting to connect to QEMU at $QEMU_PTS
echo "root" > $QEMU_PTS
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
echo "$QEMU_CMD" > $QEMU_PTS

#Seperate the results from the login and command logs
sleep 1
kill $CAT_PID
wait $CAT_PID 2> /dev/null
nohup cat $QEMU_PTS > ./logs/results.log 2>&1 &

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
    exit 1
fi

echo "Done!"
cat ./logs/results.log