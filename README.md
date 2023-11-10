# quasi-lang

A simple calculator (potential to be turned into a programming language)

# How to Build

```
mkdir build/
cd build/
cmake ..
make
```

# Known Bugs

- crashes when accessing a variable that doesn't exist
- does not free any memory
- crashes when setting a variable equal to itself (eg. `x = x + 1`)

