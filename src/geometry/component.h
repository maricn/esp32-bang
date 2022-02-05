#ifndef __component_h_
#define __component_h_

class Component {
  private:
    static uint8_t currentId;
  protected:
    uint8_t id = Component::currentId++;
  public:
    virtual ~Component() {};
    virtual void render() = 0;
    // if component should be deleted, return false
    virtual bool tick() = 0;
};

uint8_t Component::currentId = 0;

#endif
