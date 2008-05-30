#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "main.h"
#include "pwm.h"

#if RC5_DECODER
#include "rc5.h"
#endif

#if STATIC_SCRIPTS

/* include static scripts */
#include "static_scripts.h"
#include "testscript.h"

#endif

volatile struct global_t global = {{0, 0}};

int main(void)
{
	DDRD = 0xFF;
	PORTD = 0;
	init_pwm();
	
	global_pwm.channels[0].brightness = 50;
    global_pwm.channels[0].target_brightness = 0;
	global_pwm.channels[0].speed = 0x100;
	
	#if STATIC_SCRIPTS
	    init_script_threads();

	    #if RS485_CTRL == 0
	    /* start the example scripts */
	    script_threads[0].handler.execute = &memory_handler_flash;
	    script_threads[0].handler.position = (uint16_t) &colorchange_red;
	    script_threads[0].flags.disabled = 0;

	    //script_threads[1].handler.execute = &memory_handler_flash;
	    //script_threads[1].handler.position = (uint16_t) &testscript_flash2;
	    //script_threads[1].flags.disabled = 0;
	    //
	    //script_threads[2].handler.execute = &memory_handler_eeprom;
	    //script_threads[2].handler.position = (uint16_t) &testscript_eeprom;
	    //script_threads[2].flags.disabled = 0;

	    //script_threads[0].handler.execute = &memory_handler_flash;
	    //script_threads[0].handler.position = (uint16_t) &blinken;
	    //script_threads[0].flags.disabled = 0;
	    #endif

	#endif
	
	/* enable interrupts globally */
    sei();

    while (1) {
        /* after the last pwm timeslot, rebuild the timeslot table */
        if (global.flags.last_pulse) {
            global.flags.last_pulse = 0;

            update_pwm_timeslots();
        }

        /* at the beginning of each pwm cycle, call the fading engine and
         * execute all script threads */
        if (global.flags.new_cycle) {
            global.flags.new_cycle = 0;

            update_brightness();
#if STATIC_SCRIPTS
            execute_script_threads();
#endif

            continue;
        }
		if (global_pwm.channels[0].brightness == 0) {
			global_pwm.channels[0].target_brightness = 0xFF;
		}
		if (global_pwm.channels[0].brightness == 255) {
			global_pwm.channels[0].target_brightness = 0;
		}
	}
}
