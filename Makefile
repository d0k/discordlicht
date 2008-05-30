DEVICE     = atmega8
CLOCK      = 16000000
PROGRAMMER = -c USBasp
OBJECTS    = pwm.o
FUSES      = -U hfuse:w:0xc9:m -U lfuse:w:0x9f:m

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -std=gnu99 -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -mrelax -combine -fwhole-program

# symbolic targets:
all:	pwm.hex

pwm.o: gammatable.h

.c.o:
	$(COMPILE) -c $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:pwm.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

clean:
	rm -f pwm.hex pwm.elf gammatable.h $(OBJECTS)

# file targets:
gammatable.h:
	python gammatable.py > $@

pwm.elf: $(OBJECTS)
	$(COMPILE) -o pwm.elf $(OBJECTS)

pwm.hex: pwm.elf
	rm -f pwm.hex
	avr-objcopy -j .text -j .data -O ihex pwm.elf pwm.hex
