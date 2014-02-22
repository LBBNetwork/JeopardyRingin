/* Code for Jeopardy Ring-in Device, Mk.II

   Hardware configuration:
   * Raspberry Pi (Please let me know if the GPIO pins
     differ from each revision - I checked with bcm2835.h
     and tried as best I could but I only have a Rev2
     Model B)
   * SPST switch on pin 18
   * Three momentary contact switches on pins 23, 17 and 24
   * One LED (plus appropriate resistor) on pin 4
   * Three LEDs (plus appropriate resistors) on pins 22, 25
     and 21 (for Rev1 units) or 27 (for Rev2 units)

   Software configuration:
   * Requires the bcm2835 library for GPIO. Get it from:
     http://www.airspayce.com/mikem/bcm2835/

   Compiling:
   * Ensure you have gcc and the bcm2835 library installed.
     To compile, simply issue this command:

     gcc -o gpio_c gpio.c -l bcm2835

   Running:
   * Since this program uses GPIO pins, you must run the
     program as root.

   Have fun!
*/

/* Filename: gpio.c
   Author: neko2k (neko2k@beige-box)
   Website: http://www.beige-box.com
   Description: See above

   The following source code is (C) 2014 The Little Beige Box
   and is released as open-source software under the terms
   contained in LICENSE.txt. In the event you did not
   recieve a copy of the license, please contact the email
   address below for a free digital copy.

   support@beige-box.com
*/

#include <bcm2835.h>
#include <stdio.h>

#define INPUT1 RPI_GPIO_P1_16                   //Pin 23
#define INPUT2 RPI_GPIO_P1_11                   //Pin 17
#define INPUT3 RPI_GPIO_P1_18                   //Pin 24
#define ENABLER RPI_GPIO_P1_12                  //Pin 18
#define ENABLER_LED RPI_GPIO_P1_07              //Pin 4
#define P1_LED RPI_GPIO_P1_15                   //Pin 22
#define P2_LED RPI_GPIO_P1_22                   //Pin 25
#define P3_LED RPI_V2_GPIO_P1_13                //Pin 27 (Rev2)
/*#define P3_LED RPI_GPIO_P1_13*/               //Pin 21 (Rev1)
#define OPERATOR_INTERRUPT RPI_V2_GPIO_P1_05    //Pin SCL (GPIO 3, Rev2)
/*#define OPERATOR_INTERRUPT RPI_GPIO_P1_05*/   //Pin SCL? (GPIO 1, Rev1)

int GetPlayerRingin(int PlayerInput);
void InterruptDelay(int milliseconds);

int main()
{
        uint8_t lockout, value1, value2, value3;
        int P1Lockout, P2Lockout, P3Lockout = 0;

        if(!bcm2835_init())
                return 1;

        /* Set up the GPIO pins for input */
        bcm2835_gpio_fsel(INPUT1, BCM2835_GPIO_FSEL_INPT);
        bcm2835_gpio_set_pud(INPUT1, BCM2835_GPIO_PUD_UP);

        bcm2835_gpio_fsel(INPUT2, BCM2835_GPIO_FSEL_INPT);
        bcm2835_gpio_set_pud(INPUT2, BCM2835_GPIO_PUD_UP);

        bcm2835_gpio_fsel(INPUT3, BCM2835_GPIO_FSEL_INPT);
        bcm2835_gpio_set_pud(INPUT3, BCM2835_GPIO_PUD_UP);

        bcm2835_gpio_fsel(ENABLER, BCM2835_GPIO_FSEL_INPT);
        bcm2835_gpio_set_pud(ENABLER, BCM2835_GPIO_PUD_UP);

        bcm2835_gpio_fsel(OPERATOR_INTERRUPT, BCM2835_GPIO_FSEL_INPT);
        bcm2835_gpio_set_pud(OPERATOR_INTERRUPT, BCM2835_GPIO_PUD_UP);

        /* Set up the GPIO pins for LED output */
        bcm2835_gpio_fsel(ENABLER_LED, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(P1_LED, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(P2_LED, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(P3_LED, BCM2835_GPIO_FSEL_OUTP);

        while(1)
        {
                /* Set these variables to 0 so we start fresh
                   every time the Enabler switch is turned on */
                P1Lockout = 0;
                P2Lockout = 0;
                P3Lockout = 0;

                lockout = bcm2835_gpio_lev(ENABLER);
                bcm2835_gpio_write(ENABLER_LED, LOW);

                /* bcm2835 returns 0 when a digital input is triggered,
                   so we only enter the loop when the DI is 0 */
                while(!lockout)
                {
                        value1 = bcm2835_gpio_lev(INPUT1);
                        value2 = bcm2835_gpio_lev(INPUT2);
                        value3 = bcm2835_gpio_lev(INPUT3);
                        lockout = bcm2835_gpio_lev(ENABLER);

                        /* Toggle the Enabler LED on so the operator knows
                           when inputs are being recieved; we can run
                           headless this way */
                        bcm2835_gpio_write(ENABLER_LED, HIGH);

                        if(value1 == 0)
                        {
                                if(P1Lockout == 0)
                                {
                                        P1Lockout = GetPlayerRingin(1);
                                }
                                else
                                {
                                        printf("P1 already rung in\n");
                                }
                        }

                        if(value2 == 0)
                        {
                                if(P2Lockout == 0)
                                {
                                        P2Lockout = GetPlayerRingin(2);
                                }
                                else
                                {
                                        printf("P2 already rung in\n");
                                }
                        }

                        if(value3 == 0)
                        {
                                if(P3Lockout == 0)
                                {
                                        P3Lockout = GetPlayerRingin(3);
                                }
                                else
                                {
                                        printf("P3 already rung in\n");
                                }
                        }
                }
        }

        bcm2835_close();
        return 0;
}

int GetPlayerRingin(int PlayerInput)
{
        /* Each if statement will cause this function to return
           1. That value is stored in the lockout ints so we can
           lock a player out until the Enabler is turned on again */
        if(PlayerInput == 1)
        {
                printf("Player %d rung in\n", PlayerInput);
                bcm2835_gpio_write(P1_LED, HIGH);
                InterruptDelay(5000);
                bcm2835_gpio_write(P1_LED, LOW);
                return 1;
        }
        else if(PlayerInput == 2)
        {
                printf("Player %d rung in\n", PlayerInput);
                bcm2835_gpio_write(P2_LED, HIGH);
                InterruptDelay(5000);
                bcm2835_gpio_write(P2_LED, LOW);
                return 1;
        }
        else if(PlayerInput == 3)
        {
                printf("Player %d rung in\n", PlayerInput);
                bcm2835_gpio_write(P3_LED, HIGH);
                InterruptDelay(5000);
                bcm2835_gpio_write(P3_LED, LOW);
                return 1;
        }

        return 0;
}

void InterruptDelay(int milliseconds)
{
        /* This function exists so the operator can cancel a player's
           input without having to wait for the timer to expire. This
           keeps things running fast. */
        uint8_t oi;
        int IDelay;

        for(IDelay = 0; IDelay < milliseconds; IDelay = IDelay + 1)
        {
                delay(1);
                oi = bcm2835_gpio_lev(OPERATOR_INTERRUPT);

                if(oi == 0)
                {
                        printf("Ending countdown - OI switch was pressed\n");
                        break;
                }
        }
}
