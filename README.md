Implementation of [Electrostatic Halftoning](https://www.mia.uni-saarland.de/Research/Electrostatic_Halftoning/images.shtml) in C++/OpenCL.

![preview](./EH1.gif)

## Features
* GPU acceleration electrostatic halftoning.
* SVG output.

## Dependencies
* Boost.Compute
* Qt 6.x

## Build
```
cmake .
make
```
## Limitation
* force field generation is slow; therefore, small input images are recommended.
* floating-point issues, specially with large number of particles.
* color image not supported.