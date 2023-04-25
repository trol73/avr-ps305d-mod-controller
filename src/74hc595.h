// RST 10 -> +5V
// IN    B3
// STCP  B2
// SCK   B5
// RST   D4


// устанавливает переданные значения на выходах регистров
static void dacSave() {
	PORTB &= ~_BV(2);
	_delay_us(15);
	PORTB |= _BV(2);
}



inline static void dacSendByte(uint8_t val) {
	SPDR = val;
	while (!(SPSR & _BV(SPIF)));
}


