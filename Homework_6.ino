#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <stdio.h>

LiquidCrystal m_lcd(12, 11, 10, 9, 8, 7);

static const int kIntegrationPeriodSelect = 3;
static const int kSignal = 0;
static const unsigned long kTimerPeriod = 500000;

volatile int counter = 0; //number of 
volatile int integrationPeriod = 1; //2 for low, 1 for high
volatile float lastFrequency = 0.0;
volatile unsigned long numberOfPulses = 0;

void setup()
{
	Serial.begin(115200);
	m_lcd.begin(16, 2);
	Timer1.initialize(kTimerPeriod);
	Timer1.attachInterrupt(integrateSignal);
	pinMode(kIntegrationPeriodSelect, INPUT_PULLUP);
	pinMode(2, INPUT_PULLUP);
	attachInterrupt(kSignal, countPulse, FALLING);
}

void loop()
{
	(digitalRead(kIntegrationPeriodSelect) == 1)? integrationPeriod = 1 : integrationPeriod = 2;
	switch(integrationPeriod)
	{
		case 1:
			m_lcd.setCursor(0, 0);
			m_lcd.print("Range: high");
			break;
		case 2:
			m_lcd.setCursor(0, 0);
			m_lcd.print("Range: low ");
			break;
	}
	float frequency = lastFrequency;
	m_lcd.setCursor(0, 1);
	char speedOutput[16];
	unsigned long temp = (frequency - (unsigned long)frequency) * 100;
	sprintf(speedOutput, "%d.%d Hz   ", (unsigned long)frequency, temp);
	m_lcd.print(speedOutput);
}

void integrateSignal()
{
	noInterrupts();
	int period = counter % integrationPeriod;
	if (period == 0)
	{
		counter = 0;
		lastFrequency = (float)numberOfPulses / (0.5 * (float)integrationPeriod);
		numberOfPulses = 0;
	}
	counter++;
	interrupts();
}

void countPulse()
{
	numberOfPulses++;
}

/**
 * @brief Get number of digits in number.
 * @details Get the number of digits in an 
 * integer. This is needed to add padding 
 * to the numbers in the clock.
 * 
 * @param number The number to size.
 * @return The number of digits in number.
 */
int digits(int number)
{
	if (number == 0) return 1; //0 has one digit
	else
	{
		int digits = 0;
		while (number) //divide by 10 until number is 0
		{
			number /= 10;
			digits++;
		}
		return digits; //return number of digits
	}
}
