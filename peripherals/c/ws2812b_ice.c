#include ws2812.h

void ws2812b_rotate (WS2812B_t *base, uint8_t num_leds) {
	for (int i = 0; i < num_leds-1; i++) {
		base[i] = base[i-1];
	}
	base[0] = base[num_leds-1];
}

void ws2812b_pulse(WS2812B_t *base, uint8_t num_leds) {
	static int direction;
	// find if red is inc or dec
	
	if (direction == 1 && base[0].red < 0xFF) {
		for (int i = 0; i < num_leds; i++) {
			base[i].red = base[i].red + 1;
		}
	}
	
	if (direction == 1 && base[0].red == 0xFF) {
		direction = 0;
		for (int i = 0; i < num_leds; i++) {
			base[i].red = base[i].red - 1;
		}
	}
	
	if (direction == 1 && base[0].red > 0x00) {
		for (int i = 0; i < num_leds; i++) {
			base[i].red = base[i].red - 1;
		}
	
	if (direction == 1 && base[0].red == 0x00) {
		direction = 1;
		for (int i = 0; i < num_leds; i++) {
			base[i].red = base[i].red + 1;
		}
	}
	
}