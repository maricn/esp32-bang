#ifndef __util_h_
#define __util_h_

// Number of pixels of each INDIVIDUAL panel module.
#define PANEL_RES_X 64
#define PANEL_RES_Y 32

uint8_t fadeUint(uint8_t val, float factor) {
  return (uint8_t)(floorf(((float)val) * factor));
}

#endif
