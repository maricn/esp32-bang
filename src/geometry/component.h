#ifndef __component_h_
#define __component_h_

class Component {
  public:
    virtual ~Component() {};
    virtual void render() = 0;
    // if component should be deleted, return false
    virtual bool tick() = 0;
};

#endif
