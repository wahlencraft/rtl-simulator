#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <string>

#include "entity.h"

class Component : public Entity {
public:
    Component(std::string const &name="Component"): Entity(name) {}
    virtual void set() = 0;
};

#endif  // COMPONENT_H_

