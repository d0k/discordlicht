#include <avr/io.h>
#include <util/delay.h>

#define PWMFREQ 40		// 1s/100Hz/255 states == 40 us

int main(void)
{
	uint8_t pulspause = 128;
	DDRD = 0b11111100;
	PORTD = 0xFF;

	while (1) {
		PORTD = 0b00000011;
		for (uint8_t i = 0; i < pulspause; i++)
			_delay_us(PWMFREQ);
		PORTD = 0xFF;
		for (uint8_t i = 0; i < 255 - pulspause; i++)
			_delay_us(PWMFREQ);
		if ((PIND & 0b00000001) == 0)	// PIND 0 low
			pulspause++;	// will be optimized to a shift
		else if ((PIND & 0b00000010) == 0)	// PIND 1 low
			pulspause--;
	}
}
