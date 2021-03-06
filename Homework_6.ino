/**
 * RBE 2001 Homework 6
 * 
 * This program was written to count frequencies of an
 * incoming signal using an Arduino Uno. It accepts signals
 * on digital pin 2 and counts the frequency using 
 * interrupt service routines.
 * 
 * D2  : Signal
 * D3  : Integration Period Select Jumper
 * D7  : LCD D7
 * D8  : LCD D6
 * D9  : LCD D5
 * D10 : LCD D4
 * D11 : LCD E
 * D12 : LCD RS
 * 
 * @author   Arthur Lockman
 * @created  10/4/2014
 * @modified 10/6/2014 
 */

/**
 * BONUS:
 * The lowest possible non-zero frequency for the low range is 2Hz. This is because
 * the smallest (non-zero) number of pulses it can measure in the low range is 1. 1
 * pulse over 500ms is equal to 2Hz, as far as the Arduino can tell. In the high setting,
 * the lowest frequency it can measure is 10Hz. This is because the smallest number 
 * of pulses it can measure over the 100ms interval is 1, and 1 pulse over 100ms
 * is equal to 10Hz. 
 */

#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <stdio.h>

LiquidCrystal m_lcd(12, 11, 10, 9, 8, 7); //Instantiate LCD

static const int kIntegrationPeriodSelect = 3; //Pin to select integration period.
static const int kSignal = 0; //Signal interrupt pin
static const unsigned long kTimerPeriod = 100000; //Timer period

volatile int counter = 0; //number of interrupts since last reset
volatile int integrationPeriod = 1; //2 for low, 1 for high
volatile float lastFrequency = 0.0; //last read frequency
volatile unsigned long numberOfPulses = 0; //number of pulses since last integration

void setup()
{
	m_lcd.begin(16, 2); //begin LCD
	Timer1.initialize(kTimerPeriod); //initialze timer interrupt
	Timer1.attachInterrupt(integrateSignal); //attach timer interrupt
	pinMode(kIntegrationPeriodSelect, INPUT_PULLUP); //setup integration period select pin
	attachInterrupt(kSignal, countPulse, FALLING); //setup signal interrupt.
}

void loop()
{
	(digitalRead(kIntegrationPeriodSelect) == 1)? integrationPeriod = 1 : integrationPeriod = 5;
	switch(integrationPeriod) //Change LCD display to reflect selected integration period.
	{
		case 1:
			m_lcd.setCursor(0, 0);
			m_lcd.print("Range: high");
			break;
		case 5:
			m_lcd.setCursor(0, 0);
			m_lcd.print("Range: low ");
			break;
	}
	float frequency = lastFrequency; //Set local frequency to last calculated frequency.
	m_lcd.setCursor(0, 1); //Set cursor to proper position.
	char speedOutput[16]; //Create character buffer to hold speed.
	unsigned long decimals = (frequency - (unsigned long)frequency) * 100; //decimals in frequency
	snprintf(speedOutput, 16, "%d.%d Hz   ", (unsigned long)frequency, decimals); //create output string
	m_lcd.print(speedOutput); //write output string to LCD.
	delay(100);
}

/**
 * @brief Determine signal speed.
 * @details Integrates the number of pulses over
 * the given time period.
 */
void integrateSignal()
{
	noInterrupts(); //disable interrupts
	/**
	 * What this does is determine if the code should be run based on the selected
	 * integration period. The timer interrupt runs every 100 milliseconds. If the high period
	 * is selected then integrationPeriod == 1, so the code runs every interrupt. If 
	 * low is selected then integrationPeriod == 5 so the code runs every 5 interrupts,
	 * or 500ms.
	 */
	int period = counter % integrationPeriod;
	if (period == 0)
	{
		counter = 0; //Reset interrupt counter.
		lastFrequency = (float)numberOfPulses / (0.1 * (float)integrationPeriod); //Determine frequency.
		numberOfPulses = 0; //Reset pulse counter.
	}
	counter++; //increment interrupt counter.
	interrupts(); //re-enable interrupts.
}

/**
 * @brief Counts a pulse.
 * @details All this method does is increment
 * the pulse counter, indicating that a pulse has
 * occurred on the signal line.
 */
void countPulse()
{
	numberOfPulses++;
}
