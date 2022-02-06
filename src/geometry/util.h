#ifndef __util_h_
#define __util_h_

// Number of pixels of each INDIVIDUAL panel module.
#define PANEL_RES_X 64
#define PANEL_RES_Y 32

uint8_t fadeUint(uint8_t val, float factor) {
  return (uint8_t)(floorf(((float)val) * factor));
}

int8_t clip(int8_t val, int8_t min, int8_t max) {
  return val < 0 ? 0 : val > PANEL_RES_X ? PANEL_RES_X : val;
}

#endif
