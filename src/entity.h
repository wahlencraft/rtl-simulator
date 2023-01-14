#ifndef ENITIY_H_
#define ENITIY_H_

#include <string>

class Entity {
public:
    Entity(std::string name): name{name} {};
    virtual ~Entity() = default;
    virtual void reset() = 0;
    std::string get_name() { return name; }

protected:
    std::string name;
};

#endif  // ENITIY_H_

