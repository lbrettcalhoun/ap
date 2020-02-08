#
# This is the compiler that we'll use
#
CC = xtensa-lx106-elf-gcc

#
# And these are the flags passed to the compiler ... they include the location of include files (.) and the
# -mlongcalls switch.
# Note:  I added an additional custom include for all the non-standard stuff (like uart.h) ... these files are copied
# their respective locations in the SDK into my custom include directory
#
CFLAGS = -I. -I include -mlongcalls -g

# And these are all the options passed to the linker ... mainly which libraries to link (main, net80211, etc).
# All these libraries live in $HOME/esp-open-sdk/sdk/lib and are prefixed with "lib".  Also note that 
# these libraries are nothing more than object files (.o) compressed into an ar archive.
# Note:  I added -ldriver to link in the uart.c required for uart_init
#
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -ldriver -Wl,--end-group -lgcc
LDFLAGS = -Teagle.app.v6.ld

# So here is how this works ... when you execute "make" it compiles and assembles (BUT IT DOESN'T LINK) the 
# print_funcs.c and user_main.c files.  Then the linker links both of those files together with all the libraries to
# form our executable (user_main).  Finally, it uses esptool.py to transform the executable into our 2 binaries which
# make up the firmware.  These actions huser_mainpen automatically with just "make" based on the chaining:
# print_funcs.c --> print_funcs.o and then user_main.c --> user_main.o and then user_main.o and print_funcs.o --> user_main (via linker)
# and then user_main --> user_main-0x00000.bin (and user_main-0x10000.bin). 
user_main-0x00000.bin: user_main
	esptool.py elf2image $^

user_main: user_main.o print_funcs.o

user_main.o: user_main.c

# I added print_funcs.c to hold all the various print and output functions that I use for the ESP8266
print_funcs.o: print_funcs.c

# This one doesn't get called automatically.  Use "make flash" to actually flash the firmware to the ESP8266
flash: user_main-0x00000.bin
	esptool.py write_flash 0 user_main-0x00000.bin 0x10000 user_main-0x10000.bin

# Use make clean to get rid of the firmware and the executables and the object fles
clean:
	rm -f print_funcs.o user_main user_main.o user_main-0x00000.bin user_main-0x10000.bin
