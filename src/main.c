#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include <util/delay.h>

#include "74hc595.h"
#include "encoders.h"

/*
 * 			Fine				Coarse
 * V		C1.C2.C3				C4.C5.D2
 * I		D5.D6.D7				B0.B1.C0
 * 
 * 
 * LED: D3
 * DAC
 * 	1. MOSI	B3
 * 	2. SS: 	B2
 * 	3. SCK:	B5
 * 	4. RST: 	D4
 */
 
 
#define EEPROM_OFFSET_VH1		(uint8_t*)10
#define EEPROM_OFFSET_VL1		(uint8_t*)11
#define EEPROM_OFFSET_VH2		(uint8_t*)12
#define EEPROM_OFFSET_VL2		(uint8_t*)13
#define EEPROM_OFFSET_IH1		(uint8_t*)14
#define EEPROM_OFFSET_IL1		(uint8_t*)15
#define EEPROM_OFFSET_IH2		(uint8_t*)16
#define EEPROM_OFFSET_IL2		(uint8_t*)17

 
uint8_t values[4];
 
inline static void init() {
	DDRB = _BV(2) | _BV(3) | _BV(4) | _BV(5);
	DDRC = 0;
	DDRD = 0;
	
	PORTB = _BV(0) | _BV(1) | _BV(2);
	PORTC = _BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5);
	PORTD = _BV(2) | _BV(4) | _BV(5) | _BV(6) | _BV(7);
	
	SPCR = _BV(SPE) | _BV(MSTR);
	SPDR = 0;
	
	encodersInit();
}


static void outputDac() {
	dacSendByte(values[ENC_VC]);
	dacSendByte(values[ENC_VF]);
	dacSendByte(values[ENC_IC]);
	dacSendByte(values[ENC_IF]);
	dacSave();
}

void onEncoderEvent(uint8_t enc, uint8_t event) {
	bool isVoltage = enc == ENC_VF || enc == ENC_VC;
	uint16_t v = isVoltage ? 
				(values[ENC_VC] << 8) + values[ENC_VF] :
				(values[ENC_IC] << 8) + values[ENC_IF];

	uint16_t delta = (enc == ENC_IF || enc == ENC_VF) ? 5 : 570;
	
	if (event == EVENT_UP) {
		if (v < 0xffff - delta) {
			v += delta;
		} else {
			v = 0xffff;
		}
	} else {
		if (v > delta) {
			v -= delta;
		} else {
			v = 0;
		}
	}
	if (isVoltage) {
		values[ENC_VC] = v >> 8;
		values[ENC_VF] = v & 0xff;
	} else {
		values[ENC_IC] = v >> 8;
		values[ENC_IF] = v & 0xff;
	}
	

	outputDac();
}


static void saveCurrent1() {
	eeprom_update_byte(EEPROM_OFFSET_IH1, values[ENC_IC]);
	eeprom_update_byte(EEPROM_OFFSET_IL1, values[ENC_IF]);
}

static void saveCurrent2() {
	eeprom_update_byte(EEPROM_OFFSET_IH2, values[ENC_IC]);
	eeprom_update_byte(EEPROM_OFFSET_IL2, values[ENC_IF]);
}

static void saveVoltage1() {
	eeprom_update_byte(EEPROM_OFFSET_VH1, values[ENC_VC]);
	eeprom_update_byte(EEPROM_OFFSET_VL1, values[ENC_VF]);
}

static void saveVoltage2() {
	eeprom_update_byte(EEPROM_OFFSET_VH2, values[ENC_VC]);
	eeprom_update_byte(EEPROM_OFFSET_VL2, values[ENC_VF]);
}



static void loadCurrent1() {
	values[ENC_IC] = eeprom_read_byte(EEPROM_OFFSET_IH1);
	values[ENC_IF] = eeprom_read_byte(EEPROM_OFFSET_IL1);
	outputDac();
}

static void loadCurrent2() {
	values[ENC_IC] = eeprom_read_byte(EEPROM_OFFSET_IH2);
	values[ENC_IF] = eeprom_read_byte(EEPROM_OFFSET_IL2);
	outputDac();	
}

static void loadVoltage1() {
	values[ENC_VC] = eeprom_read_byte(EEPROM_OFFSET_VH1);
	values[ENC_VF] = eeprom_read_byte(EEPROM_OFFSET_VL1);
	outputDac();
}

static void loadVoltage2() {
	values[ENC_VC] = eeprom_read_byte(EEPROM_OFFSET_VH2);
	values[ENC_VF] = eeprom_read_byte(EEPROM_OFFSET_VL2);
	outputDac();
}


void onButtonPressed(uint8_t enc, bool longPress) {
	switch (enc) {
		case ENC_VF:
			if (longPress) saveCurrent1(); else loadCurrent1();
			break;
		case ENC_VC:
			if (longPress) saveCurrent2(); else loadCurrent2();
			break;
		case ENC_IF:
			if (longPress) saveVoltage1(); else loadVoltage1();
			break;
		case ENC_IC:
			if (longPress) saveVoltage2(); else loadVoltage2();
			break;
	}
}


void main() {
	init();
	
	values[ENC_IF] = 50;
	values[ENC_IC] = 0;
	values[ENC_VF] = 0;
	values[ENC_VC] = 0;

	for (int i = 0; i < 5; i++) {
		outputDac();
	}

	
	uint16_t cnt = 0;
	while (true) {
		encodersCheck();
		if (++cnt == 500) {
			encodersCheckButtons();
			cnt = 0;
		}
	}
}

