#ifndef __matrix_config_h_
#define __matrix_config_h_

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Number of pixels of each INDIVIDUAL panel module.
#define PANEL_RES_X 64
#define PANEL_RES_Y 32

// Redefined according to https://github.com/NeoCat/ESP32-P3RGB64x32MatrixPanel#wiring-esp32-with-the-led-matrix-panel
#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 21
#define G2_PIN 22
#define B2_PIN 23
#define A_PIN 12
#define B_PIN 16
#define C_PIN 17
#define D_PIN 18
#define E_PIN -1 // required for 1/32 scan panels, like 64x64. Any available pin would do, i.e. IO32
#define CLK_PIN 15
#define LAT_PIN 32
#define OE_PIN 33

static HUB75_I2S_CFG::i2s_pins _pins={R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
static HUB75_I2S_CFG mxconfig(
	PANEL_RES_X, // Module width
	PANEL_RES_Y, // Module height
	1, // chain length (number of LED panels)
	_pins // pin mapping
);

#endif
