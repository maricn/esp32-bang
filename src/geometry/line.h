#ifndef __line_h_
#define __line_h_

#include <P3RGB64x32MatrixPanel.h>
#include <math.h>

#include "point.h"
#include "util.h"

class Line : virtual public Component {
 protected:
  static float deg2rad;
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
    this->radius -= 1;
    if (this->radius <= 0) return false;

    // shake
    /* this->angle += esp_random() % 10 - 5; */
    // shakey slide
    this->angle += esp_random() % 5 - 1;
    // slide
    /* this->angle += 2; */
    return true;
  }

  void render() {
    float width =
        floorf((float)this->radius * cos(this->angle * Line::deg2rad));
    float height =
        floorf((float)this->radius * sin(this->angle * Line::deg2rad));
    uint8_t x0, y0, x1, y1;
    x0 = (uint8_t)(this->center->x - width);
    x1 = (uint8_t)(this->center->x + width);
    y0 = (uint8_t)(this->center->y - height);
    y1 = (uint8_t)(this->center->y + height);

    this->matrix->drawLine(x0 < 0             ? 0
                           : x0 > PANEL_RES_X ? PANEL_RES_X
                                              : x0,
                           y0 < 0             ? 0
                           : y0 > PANEL_RES_Y ? PANEL_RES_Y
                                              : y0,
                           x1 < 0             ? 0
                           : x1 > PANEL_RES_X ? PANEL_RES_X
                                              : x1,
                           y1 < 0             ? 0
                           : y1 > PANEL_RES_Y ? PANEL_RES_Y
                                              : y1,
                           (uint8_t)esp_random());
  }
};

float Line::deg2rad = PI / 180.f;

#endif
