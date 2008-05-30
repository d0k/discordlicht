#include <avr/io.h>
#include <util/delay.h>
#include "gammatable.h"

#define PWMFREQ 39		// 1s/100Hz/255 states == 39 us

int main(void)
{
	uint8_t pulspause1 = 128, pulspause2 = 0, pulspause3 = 255;
	DDRD = 0b11110000;
	PORTD = 0xFF;

	while (1) {
		PORTD = 0b00000011;
		for (uint8_t i = 0; i < 0xFF; i++) {
			if (i > gammatable[pulspause1])
				PORTD |= 0b10000000;
			if (i > gammatable[pulspause2])
				PORTD |= 0b01000000;
			if (i > gammatable[pulspause3])
				PORTD |= 0b00100000;
			_delay_us(PWMFREQ);
		}
		if ((PIND & 0b00000001) == 0) { // PIND 0 low
			pulspause1--;
		} else if ((PIND & 0b00000010) == 0) {	// PIND 1 low
			pulspause1++;
		} else if ((PIND & 0b00000100) == 0) { // PIND 3 low
			pulspause2--;
			pulspause3++;
		} else if ((PIND & 0b00001000) == 0) { // PIND 4 low
			pulspause3--;
			pulspause2++;
		}
	}
}
