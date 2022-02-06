#ifndef __line_h_
#define __line_h_

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <math.h>

#include "matrix_config.h"
#include "point.h"
#include "util.h"

class Line : virtual public Component {
 protected:
  static float deg2rad;
  MatrixPanel_I2S_DMA *matrix;
  Point center;
  uint8_t radius;
  uint8_t angle;

 public:
  Line(MatrixPanel_I2S_DMA *matrix, Point center, uint8_t radius,
       uint8_t angle) {
    this->matrix = matrix;
    this->center = center;
    this->radius = radius;
    this->angle = angle;
  };

  bool tick() {
    this->radius -= 1;
    if (this->radius <= 0) return false;

    // shakey slide
    /* this->angle += esp_random() % 5 - 1; */
    // slide
    this->angle = this->angle + 2 % 360;
    return true;
  }

  void render() {
#if DEBUG_GFX
    Serial.printf("[DEBUG] GFX.Line[%3u].render: (%u, %u), %u, %u\n", this->id,
                  this->center.x, this->center.y, this->radius, this->angle);
#endif
    float width =
        floorf((float)this->radius * cos(this->angle * Line::deg2rad));
    float height =
        floorf((float)this->radius * sin(this->angle * Line::deg2rad));

    int8_t x0 = (int8_t)(this->center.x) - width;
    int8_t x1 = (int8_t)(this->center.x) + width;
    int8_t y0 = (int8_t)(this->center.y) - height;
    int8_t y1 = (int8_t)(this->center.y) + height;
#if DEBUG_GFX
    Serial.printf("[DEBUG] GFX.Line[%3u].render: {(%d, %d), (%d, %d)}\n",
                  this->id, x0, y0, x1, y1);
#endif

    x0 = clip(x0, 0, PANEL_RES_X);
    y0 = clip(y0, 0, PANEL_RES_Y);
    x1 = clip(x1, 0, PANEL_RES_X);
    y1 = clip(y1, 0, PANEL_RES_Y);
    this->matrix->drawLine(x0, y0, x1, y1, (uint8_t)esp_random());
  }
};

float Line::deg2rad = PI / 180.f;

class LineThatRotates : public Line {
 public:
  LineThatRotates(MatrixPanel_I2S_DMA *matrix, Point center, uint8_t radius,
                  uint8_t angle)
      : Line(matrix, center, radius, angle) {}

  bool tick() {
    this->radius -= 1;
    if (this->radius <= 0) return false;
    this->angle = this->angle + 2 % 360;
    return true;
  }
};

class LineThatShakes : public Line {
 public:
  LineThatShakes(MatrixPanel_I2S_DMA *matrix, Point center, uint8_t radius,
                 uint8_t angle)
      : Line(matrix, center, radius, angle) {}

  bool tick() {
    this->radius -= 1;
    if (this->radius <= 0) return false;
    this->angle = (this->angle + esp_random() % 10 - 5) % 360;
    return true;
  }
};

#endif
