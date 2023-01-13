#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "entity.h"

class Component : public Entity {
public:
    Component(std::string const &name="Component"): Entity(name) {}
    virtual ~Component() = default;
    virtual void reset() override = 0;
    virtual void set() = 0;

    Component(const Component &) = delete;

private:
};

#endif  // COMPONENT_H_

