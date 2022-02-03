#ifndef __line_h_
#define __line_h_

#include <P3RGB64x32MatrixPanel.h>
#include <math.h>

#include "point.h"
#include "util.h"

class Line : virtual public Component {
 protected:
  P3RGB64x32MatrixPanel *matrix;
  Point *center;
  uint8_t radius;
  float angle;

 public:
  Line(P3RGB64x32MatrixPanel *matrix, Point *center, uint8_t radius,
       float angle) {
    this->matrix = matrix;
    this->center = center;
    this->radius = radius;
    this->angle = angle;
  };

  ~Line() { delete center; }

  bool tick() {
    this->angle += 0.1;
    this->radius -= 1;
    if (this->radius <= 0) return false;
    return true;
  }

  void render() {
    float x0, y0, x1, y1;
    x0 = x1 = floorf((float)this->radius * cos(this->angle));
    y0 = y1 = floorf((float)this->radius * sin(this->angle));
    x0 -= this->center->x;
    x1 += this->center->x;
    y0 -= this->center->y;
    y1 += this->center->y;
    this->matrix->drawLine(
        (uint8_t)(x0 + PANEL_RES_X / 2), (uint8_t)(y0 + PANEL_RES_Y / 2),
        (uint8_t)(x1 + PANEL_RES_X / 2), (uint8_t)(y1 + PANEL_RES_Y / 2),
        (uint8_t)esp_random());
  }
};

#endif
