DEVICE     = atmega8
CLOCK      = 16000000
PROGRAMMER = -c USBasp
OBJECTS    = main.o pwm.o rc5.o static_scripts.o
FUSES      = -U hfuse:w:0xc9:m -U lfuse:w:0x9f:m

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -std=gnu99 -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)# -mrelax -combine

# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

clean:
	rm -f main.hex main.elf $(OBJECTS)

# file targets:

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
