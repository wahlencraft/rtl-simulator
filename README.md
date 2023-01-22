# RTL Simulator

The goal of this project was to make a high level Register Transfer Level
simulator which can run at a high clock speed.

To do this I thought it would be usefull to take advantage of the very parallel
nature of an RTL design and compute multiple simulation paths concurrently.

It turns out that the overhead introduced by handling threads in this case is
a lot more than the gain from the concurrent calculations.

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

### Wire<N>: Entity
Passes values to one or more InputPorts.

- `set(BitVector<N> value)`

### InputPort<N>: Entity
InputPort takes a value and makes it available for its parent.

### Component: Entity
Virtual base class for many different components.

Most components has a few InputPorts. To set the component set each InputPort.
The InputPorts will then call the Component::set() method which will only
execute fully once all InputPorts has called it.

If the component only has one InputPort it is called input. If the component
has many inputs of the same width, they are probably found in an array called
input. Accessing each input is then `component.input[i]` where `i` is an
integer from 0 to the numbe of inputs.

### SimpleComponent<N, COMPONENTS>
Virtual base class for many similar components.

These components takes COMPONENTS inputs, but only one output. All inputs and
the output is of width N.

The InputPorts are stored in an array of length INPUTS called input. If there
is only one input it can be accessed without the array notation.

List of SimpleComponent derived classes:
- Inverter<N>
- ANDGate<N>
- NANDGate<N>
- ORGate<N>
- XORGATE<N>
- NORGATE<N>

### Clockable
Virtual base class for some components, for example Registers.
- `clock()`: Update the internal state of the clockable object based on it's
             current input.
- `start()`: Start the set() chain.

### Register<N>: Clockable, Component
The `InputPort` is called `input`.

- `BitVector<N> get_value()`: Get the value stored in the register.
                              Mostly for test purposes.

### Adder<N>: Component
```
     \-A-\/-B-/
------\      Cin
       \    /
        ----
          |
```

