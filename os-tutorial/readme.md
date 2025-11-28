
brew install qemu nasm

Don't use the Xcode developer tools nasm if you have them installed, they won't work for the most cases. Always use /usr/local/bin/nasm


nasm -f bin boot_sect_hello.asm -o boot_sect_hello.bin
qemu-system-i386 boot_sect_hello.bin

