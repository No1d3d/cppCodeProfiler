# cppCodeProfiler

A lightweight and high-performance C++ profiler for measuring function execution time. Supports pause/resume, scoped profiling, and exporting results to CSV.

## Features

- Measure execution time of functions with minimal overhead
- Scoped profiling for automatic function tracking
- Pause and resume functionality to exclude user input time
- Export profiling results to CSV for further analysis
- Optional memory allocation tracking (if enabled)

## Installation

Clone the repository and include `cppCodeProfiler.hpp` in your project:

```sh
git clone git@github.com:No1d3d/cppCodeProfiler.git
cd cppCodeProfiler
```

Then, include the header file in your project:

```cpp
#include "cppCodeProfiler.hpp"
```
