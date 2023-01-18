# RTL Simulator

The goal of this project is to make a high level Register Transfer Level
simulator which can run at a high clock speed.

The simulator will take advantage of the very parallel nature of a RTL design
and compute multiple simulation paths concurrently.

## Documentation

### BitVector<N>
A type for storing values of bitlength N. This class is very important for the
implementation of other classes because it handles the storage and operation on
the values, but you don't have to understand the details to use the simulator.

### Entity
Virtual base class for most other classes.

- `reset()`: Reset the entity to before a new clock cycle. Recursively reset
    any entity which this entity points to. The main purpose of reset is to
    help keep track of what entities has already been `set()` in a particular
    clock cycle.
- `set()` or `set(T value)`: Each entity has a set method. For some entities
    the value is passed with the method, for some it's not. No Entity
    implements both methods.

### Wire<N>: Entity
- `set(BitVector<N> value)`

### InputPort<N>: Entity

### Component: Entity
Virtual base class for many different components.

Most components has a few InputPorts. To set the component set each InputPort.
The InputPorts will then call the Component::set() method which will only
execute fully once all InputPorts has called it.

### Register<N>: Clockable, Component
The `InputPort` is called `in`.

- `set(BitVec value)`: Set the value of a Register. This ends the set chain and the
    output will not change until next clock cycle.
- `clock(T value)`: Start a new clock cycle. Error if the register has not been
    set.

### Adder<N>: Component
```
     \-A-\/-B-/
------\      Cin
       \    /
        ----
          |
```

