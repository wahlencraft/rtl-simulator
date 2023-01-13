#ifndef ENITIY_H_
#define ENITIY_H_

class Entity {
public:
    Entity(std::string name): name{name} {};
    virtual ~Entity() = default;
    virtual void reset() = 0;

protected:
    std::string name;
};

#endif  // ENITIY_H_

