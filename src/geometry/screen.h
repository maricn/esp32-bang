#ifndef __screen_h_
#define __screen_h_

#include "component.h"
#include "util.h"

class Screen : virtual public Component {
 private:
  MatrixPanel_I2S_DMA *matrix;
  uint8_t brightness = 0;
  uint8_t hue = 0;
  float decayLength = 0.6;
  float decayFactor = pow(decayLength, 0.016);

 public:
  Screen(MatrixPanel_I2S_DMA *matrix) { this->matrix = matrix; }
  ~Screen() {}

  void setBrightness(uint8_t brightness) { this->brightness = brightness; }

  void setDecayFactor(float decayFactor) { this->decayFactor = decayFactor; }

  void setDecayLength(float decayLength) {
    this->decayLength = decayLength;
    this->decayFactor = pow(decayLength, 0.016);
  }

  void setHue(uint8_t hue) { this->hue = hue; }

  bool tick() {
    this->brightness = fadeUint(this->brightness, this->decayFactor);
    return true;
  }

  void render() {
    uint16_t matrix_color = colorHSV(this->hue, this->brightness, this->brightness);
    this->matrix->fillScreen(matrix_color);
  }
};

#endif
