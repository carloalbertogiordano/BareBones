#!/bin/bash

# Define variables
BUILD_DIR="build_dir"
OS_NAME="myos"
BOOT_FILE="boot.s"
KERNEL_FILE="kernel.c"
LINKER_FILE="linker.ld"

# Create build directory
mkdir -p "${BUILD_DIR}"

# Compile boot loader
echo "Compiling boot loader..."
i686-elf-as "${BOOT_FILE}" -o "${BUILD_DIR}/boot.o"

# Compile kernel
echo "Compiling kernel..."
i686-elf-gcc -c "${KERNEL_FILE}" -o "${BUILD_DIR}/kernel.o" -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# Linking
echo "Linking..."
i686-elf-gcc -T "${LINKER_FILE}" -o "${BUILD_DIR}/${OS_NAME}.bin" -ffreestanding -O2 -nostdlib "${BUILD_DIR}/boot.o" "${BUILD_DIR}/kernel.o" -lgcc

# Verify multiboot
if grub2-file --is-x86-multiboot "${BUILD_DIR}/${OS_NAME}.bin"; then
  echo "Multiboot confirmed"
else
  echo "The file is not multiboot"
fi

# Generate grub configuration
echo "Generating grub2 configuration..."
cat << EOF > grub.cfg
menuentry "${OS_NAME}" {
  multiboot /boot/${OS_NAME}.bin
}
EOF

# Copy files to ISO directory
echo "Copying files to ISO directory..."
mkdir -p "${BUILD_DIR}/isodir/boot/grub"
cp "${BUILD_DIR}/${OS_NAME}.bin" "${BUILD_DIR}/isodir/boot/${OS_NAME}.bin"
cp grub.cfg "${BUILD_DIR}/isodir/boot/grub/grub.cfg"

# Create ISO image
echo "Creating ISO image..."
grub2-mkrescue -o "${OS_NAME}.iso" "${BUILD_DIR}/isodir"
