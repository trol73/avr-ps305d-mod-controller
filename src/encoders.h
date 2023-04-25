#define EVENT_UP		1
#define EVENT_DOWN	2
#define EVENT_PRESS	3

#define ENC_IF			0
#define ENC_IC			1
#define ENC_VF			2
#define ENC_VC			3

// * 			Fine					Coarse
// * V		C1.C2.C3				C4.C5.D2
// * I		D5.D6.D7				B0.B1.C0

uint8_t encodersPressed[4];

void onEncoderEvent(uint8_t enc, uint8_t event);
void onButtonPressed(uint8_t enc, bool longPress);


static uint8_t encoderReadIF() {
	return (PINC >> 1) & 0b11;
}

static uint8_t encoderReadIC() {
	return (PINC >> 4) & 0b11;
}

static uint8_t encoderReadVF() {
	return (PIND >> 5) & 0b11;
}

static uint8_t encoderReadVC() {
	return PINB & 0b11;
}


static uint8_t encoderGetCodeIF() {
	static uint8_t prev;
	uint8_t val = encoderReadIF();
	uint8_t code = (prev << 2) | val;
	prev = val;
	return code;
}

static uint8_t encoderGetCodeIC() {
	static uint8_t prev;
	uint8_t val = encoderReadIC();
	uint8_t code = (prev << 2) | val;
	prev = val;
	return code;
}

static uint8_t encoderGetCodeVF() {
	static uint8_t prev;
	uint8_t val = encoderReadVF();
	uint8_t code = (prev << 2) | val;
	prev = val;
	return code;
}

static uint8_t encoderGetCodeVC() {
	static uint8_t prev;
	uint8_t val = encoderReadVC();
	uint8_t code = (prev << 2) | val;
	prev = val;
	return code;
}


inline static void encodersInit() {
	encoderGetCodeIF();
	encoderGetCodeIC();
	encoderGetCodeVF();
	encoderGetCodeVC();

	encodersPressed[0] = 0;
	encodersPressed[1] = 0;
	encodersPressed[2] = 0;
	encodersPressed[3] = 0;
}
    
  
inline static void encoderCheck(uint8_t enc, uint8_t code) {
	if (code == 1 || code == 7 || code == 8 || code == 14) {
		onEncoderEvent(enc, EVENT_UP);
	} else if (code == 2 || code == 4 || code == 11 || code == 13) {
		onEncoderEvent(enc, EVENT_DOWN);
	}
}


inline static void encodersCheck() {
	encoderCheck(ENC_IF, encoderGetCodeIF());
	encoderCheck(ENC_IC, encoderGetCodeIC());
	encoderCheck(ENC_VF, encoderGetCodeVF());
	encoderCheck(ENC_VC, encoderGetCodeVC());
}


static void onRelease(uint8_t enc) {
	if (encodersPressed[enc] > 200) {
		onButtonPressed(enc, true);
	} else if (encodersPressed[enc] > 3) {
		onButtonPressed(enc, false);
	}
	encodersPressed[enc] = 0;
}


inline static void encodersCheckButtons() {
	if (PINC & _BV(3)) {
		onRelease(ENC_VF);
	} else {
		if (encodersPressed[ENC_VF] < 0xff) {
			encodersPressed[ENC_VF]++;
		}
	}
	if (PIND & _BV(2)) {
		onRelease(ENC_VC);
	} else {
		if (encodersPressed[ENC_VC] < 0xff) {
			encodersPressed[ENC_VC]++;
		}
	}
	
	if (PIND & _BV(7)) {
		onRelease(ENC_IF);
	} else {
		if (encodersPressed[ENC_IF] < 0xff) {
			encodersPressed[ENC_IF]++;
		}
	}
	if (PINC & _BV(0)) {
		onRelease(ENC_IC);
	} else {
		if (encodersPressed[ENC_IC] < 0xff) {
			encodersPressed[ENC_IC]++;
		}
	}	
}
