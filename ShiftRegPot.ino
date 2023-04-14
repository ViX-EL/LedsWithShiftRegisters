#include "Potentiometer.h"

#define POT_MODE 0
#define POT_SPEED 1
#define SHIFT_REG_COUNT 4
#define LEDS_COUNT_IN_SHIFT_REG 8

#define DATA_PIN 9    // к выводу 14 регистра
#define CLOCK_PIN 11  // к выводу 11 регистра (SH_CP)
#define LATCH_PIN 12  // к выводу 12 регистра (ST_CP)

#define MODS_COUNT 9

Potentiometer potMode(POT_MODE, 1, MODS_COUNT);
Potentiometer potSpeed(POT_SPEED, 10, 200);

void setup() 
{
	pinMode(LATCH_PIN, OUTPUT);
	pinMode(CLOCK_PIN, OUTPUT);
	pinMode(DATA_PIN, OUTPUT);
}

void loop() 
{
	int potModeVal = potMode.read();
	int potSpeedVal =  potSpeed.read();
  
	static bool alreadyDone = false;
	if (potModeVal == 1)
	{
        mode1(potSpeedVal);
	}
	else if (potModeVal == 2)
	{
        mode2(potSpeedVal);
	}
	else if (potModeVal == 3)
	{
		mode3(potSpeedVal);
	}
	else if (potModeVal == 4)
	{
		mode4(potSpeedVal);
		if (potMode.getPrevState() > MODS_COUNT - 6)
		{
			alreadyDone = false;
		}
	}
	else
	{
		if(!alreadyDone || potMode.getPrevState() != potModeVal)  //выполняется один раз
		{
			turnOnEachNLED(potModeVal - MODS_COUNT + 4, 5);
			alreadyDone = true;
		}
	}
}

void mode1(const int changePeriod)
{
    byte byteToSend = 0; //Создаем пустой байт B00000000
	for (int bitPos = 0; bitPos < LEDS_COUNT_IN_SHIFT_REG; bitPos++) // В переменной хранится позиция изменяемого бита
	{	
		byteToSend = 0; // Обнуляем байт при каждом проходе
		bitWrite(byteToSend, bitPos, HIGH); // При bitPos=0 получим B00000001, при bitPos=1 - B00000010, при bitPos=2 - B00000100 и т.д.

 		turnOnLeds(byteToSend);
		delay(changePeriod);
	}
}

void mode2(const int changePeriod)
{
    byte byteToSend = 0; //Создаем пустой байт B00000000
	for (int bitPos = 0; bitPos < LEDS_COUNT_IN_SHIFT_REG; bitPos++) // В переменной хранится позиция изменяемого бита
	{	
		byteToSend = 0; // Обнуляем байт при каждом проходе
		bitWrite(byteToSend, bitPos, HIGH); // При bitPos=0 получим B00000001, при bitPos=1 - B00000010, при bitPos=2 - B00000100 и т.д.

 		turnOnLedsReverse(byteToSend);
		delay(changePeriod);
	}
}

void mode3(const int changePeriod)
{
	for (int startLed = 0; startLed < 5; startLed++)
	{	
 		turnOnEachNLED(startLed, 5);
		delay(changePeriod);
	}
	for (int startLed = 5; startLed > 0; startLed--)
	{	
 		turnOnEachNLED(startLed, 5);
		delay(changePeriod);
	}
}

void mode4(const int changePeriod)
{
	    const byte path[4] = {
        B10000001,
        B01000010,
	    B00100100,
        B00011000
    };

	for (int currPhase = 0; currPhase < 4; currPhase++)
	{
		for (int currShiftReg = 0; currShiftReg < SHIFT_REG_COUNT; currShiftReg++) 
		{
			turnOnLeds(path[currPhase]);
			delay(changePeriod);
		}
	}

	for (int currPhase = 0; currPhase < 4; currPhase++)
	{
		for (int currShiftReg = 0; currShiftReg < SHIFT_REG_COUNT; currShiftReg++) 
		{
			turnOnLeds(path[3 - currPhase]);
			delay(changePeriod);
		}
	}
}

void flashing(const int pin, const int period)
{
	static long previousMillis = 0;
	long currentMillis = millis();
	static bool pinState = LOW;
	if (currentMillis - previousMillis >= period)
	{
		previousMillis = currentMillis;
		pinState == LOW ? HIGH : LOW;
		digitalWrite(pin, pinState);
  	}
}

void turnOnLeds(const byte bitMap)
{
	digitalWrite(LATCH_PIN, LOW);
	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, bitMap); // Инвертируем сигнал при помощи MSBFIRST, грузим с первого бита
	digitalWrite(LATCH_PIN, HIGH);
}

void turnOnLedsReverse(const byte bitMap)
{
	digitalWrite(LATCH_PIN, LOW);
	shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, bitMap); // LSBFIRST, грузим с последнего
	digitalWrite(LATCH_PIN, HIGH);
}

void turnOnEachNLED(const int startLed, const int periodicity)  
{
	byte bitMaps[4] = {0};

	int currLed = startLed;
	int currShiftReg = 1;
	while (currLed < SHIFT_REG_COUNT * LEDS_COUNT_IN_SHIFT_REG)
	{
		bitWrite(bitMaps[currShiftReg - 1], currLed - LEDS_COUNT_IN_SHIFT_REG * (currShiftReg - 1), HIGH);
		currLed += periodicity;
		if ((currLed >= LEDS_COUNT_IN_SHIFT_REG * currShiftReg) && (currLed < SHIFT_REG_COUNT * LEDS_COUNT_IN_SHIFT_REG))
			currShiftReg++;
	}

	for (int i = 3; i >= 0; i--)
		turnOnLeds(bitMaps[i]);
}
