/* vim:fdm=marker:ts=4:et:ai:
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

#ifndef CONFIG_H
#define CONFIG_H

#include <avr/version.h>

/* check for avr-libc version */
#if __AVR_LIBC_VERSION__ < 10402UL
#error newer libc version (>= 1.4.2) needed!
#endif

/* check if cpu speed is defined */
#ifndef F_CPU
#error please define F_CPU!
#endif

#define PWM_CHANNELS 3
#define RC5_DECODER 1
#define STATIC_SCRIPTS 1

/* color <-> channel assignment */
#define CHANNEL_RED     0
#define CHANNEL_GREEN   1
#define CHANNEL_BLUE    2

#define _TIMSK_TIMER1 TIMSK

#endif