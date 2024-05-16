#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/barebones.kernel isodir/boot/barebones.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "barebones" {
	multiboot /boot/barebones.kernel
}
EOF
grub2-mkrescue -o barebones.iso isodir
