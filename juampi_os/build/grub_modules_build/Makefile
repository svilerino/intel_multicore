LDFLAGS=-static -m elf_i386 --oformat binary -b elf32-i386
%.o: %.asm
	nasm -felf32 -o $@ $^

ap_startup_code: ap_startup_code.o
	ld $(LDFLAGS) -e mp_ap_start -Ttext 0x2000 -o $@ $^
	cp $@ ../grub_modules/$@

clean:
	rm -rf ap_startup_code ap_startup_code.o
