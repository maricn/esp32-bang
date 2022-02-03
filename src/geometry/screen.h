#ifndef __screen_h_
#define __screen_h_

#include "component.h"
#include "util.h"

class Screen : virtual public Component {
 private:
  P3RGB64x32MatrixPanel *matrix;
  uint8_t brightness = 0;
  float decayLength = 0.6;
  float decayFactor = pow(decayLength, 0.016);

 public:
  Screen(P3RGB64x32MatrixPanel *matrix) { this->matrix = matrix; }
  ~Screen() {}

  void setBrightness(uint8_t brightness) { this->brightness = brightness; }

  void setDecayFactor(float decayFactor) { this->decayFactor = decayFactor; }

  bool tick() {
    this->brightness = fadeUint(this->brightness, this->decayFactor);
    return true;
  }

  void render() {
    uint16_t matrix_color = this->matrix->colorHSV(0, 0, this->brightness);
    this->matrix->fillScreen(matrix_color);
  }
};

#endif
