run:
	@qemu-system-i386 -hda hd.img -serial null -parallel stdio 

build:
	@nasm -g -f elf32 mbr_backup.asm -o mbr_backup.o
	@ld -o mbr_backup.symbol -melf_i386 -N mbr_backup.o -Ttext 0x7c00
	@ld -o mbr_backup.bin -melf_i386 -N mbr_backup.o -Ttext 0x7c00 --oformat binary
	@nasm -g -f elf32 bootloader.asm -o bootloader.o
	@ld -o bootloader.symbol -melf_i386 -N bootloader.o -Ttext 0x7e00
	@ld -o bootloader.bin -melf_i386 -N bootloader.o -Ttext 0x7e00 --oformat binary
	@dd if=mbr_backup.bin of=hd.img bs=512 count=1 seek=0 conv=notrunc
	@dd if=bootloader.bin of=hd.img bs=512 count=5 seek=1 conv=notrunc
clean:
	@rm -fr *.bin *.o *.symbol
debug:
	@qemu-system-i386 -hda hd.img -serial null -parallel stdio -s -S &
	@sleep 1
	@gnome-terminal -e "gdb -q -x gdbinit"
