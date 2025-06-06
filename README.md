Implementation of [Electrostatic Halftoning](https://www.mia.uni-saarland.de/Research/Electrostatic_Halftoning/images.shtml) in C++/OpenCL.

![preview](./EH1.gif)

## Features
* GPU accelerated electrostatic halftoning.
* SVG output.

## Dependencies
* Boost.Compute
* Qt 6.x

## Build
```
cmake .
make
```
## Limitations
* force field generation is slow; therefore, small input images are recommended.
* floating-point arithmetic on large number of points eventually causes issues.
* greyscale output only.