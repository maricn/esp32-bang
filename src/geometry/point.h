#ifndef __point_h_
#define __point_h_

class Point : virtual public Component {
  public:
    uint8_t x;
    uint8_t y;
    Point(uint8_t x, uint8_t y) {
      this->x = x;
      this->y = y;
    }
    ~Point() {}
    bool tick() {
      return true;
    }
    void render() {
    }
};

#endif
