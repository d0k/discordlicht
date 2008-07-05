/* vim:fdm=marker ts=4 et ai
 * {{{
 *         fnordlicht firmware next generation
 *
 *    for additional information please
 *    see http://koeln.ccc.de/prozesse/running/fnordlicht
 *
 * (c) by Alexander Neumann <alexander@bumpern.de>
 *     Lars Noschinski <lars@public.noschinski.de>
 *     Benjamin Kramer <benny.kra@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 }}} */

/* includes */
#include "config.h"

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

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

/* structs */
volatile struct global_t global = {{0, 0}};

/* prototypes */
static inline void init_output(void);

/** init output channels */
void init_output(void) { /* {{{ */
    /* set all channels low -> leds off */
    PORTB = 0;
    /* configure PB0-PB2 as outputs */
    DDRB = _BV(PB0) | _BV(PB1) | _BV(PB2);
}

/* }}} */

/** main function
 */
int main(void) {
    init_output();
    init_pwm();

#if RC5_DECODER
    init_rc5();
#endif

#if STATIC_SCRIPTS
    init_script_threads();

    /* start the example scripts */
    script_threads[0].handler.execute = &memory_handler_flash;
    script_threads[0].handler.position = (uint16_t) &colorchange_red;
    script_threads[0].flags.disabled = 1;

    script_threads[1].handler.execute = &memory_handler_flash;
    script_threads[1].handler.position = (uint16_t) &green_flash;
    script_threads[1].flags.disabled = 1;

    //script_threads[1].handler.execute = &memory_handler_flash;
    //script_threads[1].handler.position = (uint16_t) &testscript_flash2;
    //script_threads[1].flags.disabled = 0;

    //script_threads[2].handler.execute = &memory_handler_eeprom;
    //script_threads[2].handler.position = (uint16_t) &testscript_eeprom;
    //script_threads[2].flags.disabled = 0;

    script_threads[2].handler.execute = &memory_handler_flash;
    script_threads[2].handler.position = (uint16_t) &blinken;
    script_threads[2].flags.disabled = 1;

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


#if RC5_DECODER
        /* check if we received something via ir */
        if (global_rc5.new_data) {
            static uint8_t toggle_bit = 2;

            /* if key has been pressed again */
            if (global_rc5.received_command.toggle_bit != toggle_bit) {

                /* if code is 0x01 (key '1' on a default remote) */
                //if (global_rc5.received_command.code == 0x01) {

                    /* install script into thread 1 */
                    //script_threads[1].handler.execute = &memory_handler_flash;
                    //script_threads[1].handler.position = (uint16_t) &green_flash;
                    //script_threads[1].flags.disabled = 0;
                    //script_threads[1].handler_stack_offset = 0;

                //}

                for (uint8_t i = 0; i < PWM_CHANNELS; i++)
                    global_pwm.channels[i].speed = 0x0100;

                switch (global_rc5.received_command.code) {
                case 0x01:
                    global_pwm.channels[0].target_brightness += 5;
                    break;
                case 0x02:
                    global_pwm.channels[0].target_brightness -= 5;
                    break;
                case 0x04:
                    global_pwm.channels[1].target_brightness += 5;
                    break;
                case 0x05:
                    global_pwm.channels[1].target_brightness -= 5;
                    break;
                case 0x07:
                    global_pwm.channels[2].target_brightness += 5;
                    break;
                case 0x08:
                    global_pwm.channels[2].target_brightness -= 5;
                    break;
                case 0x03:
                    script_threads[0].handler.execute = &memory_handler_flash;
                    script_threads[0].handler.position = (uint16_t) &colorchange_red;
                    script_threads[0].flags.disabled = !script_threads[0].flags.disabled;
                    script_threads[0].handler_stack_offset = 0;
                    break;
                case 0x06:
                    script_threads[1].handler.execute = &memory_handler_flash;
                    script_threads[1].handler.position = (uint16_t) &colorchange_red_blue    ;
                    script_threads[1].flags.disabled = !script_threads[1].flags.disabled;
                    script_threads[1].handler_stack_offset = 0;
                    break;
                case 0x09:
                    script_threads[2].handler.execute = &memory_handler_flash;
                    script_threads[2].handler.position = (uint16_t) &blinken;
                    script_threads[2].flags.disabled = !script_threads[2].flags.disabled;
                    script_threads[2].handler_stack_offset = 0;
                    break;
                case 0x00:
                    for (uint8_t i = 0; i < PWM_CHANNELS; i++) {
                        global_pwm.channels[i].target_brightness = 0;
                        script_threads[i].flags.disabled = 1;
                    }
                    break;
                }

                /* store new toggle bit state */
                toggle_bit = global_rc5.received_command.toggle_bit;

            }

            /* reset the new_data flag, so that new commands can be received */
            global_rc5.new_data = 0;

            continue;
        }
#endif
    }
}
