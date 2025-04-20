
# Neural Network framework in C 


A lightweight neural network framework implemented in C. This single-header library provides a simple way to create, train, and use neural networks with configurable architectures.


## Features

- Single-header library - Easy to integrate into any C project
- Matrix operations - Basic linear algebra implementation
- Configurable network architecture - Create networks with custom layer sizes
- No dependencies - Pure C implementation with only standard libraries


## Installation

This is a single-header library. Simply include `nn.h` in your project:

```C
#define NN_IMPLEMENTATION
#include "nn.h"
```
    
## Customization
You can customize the memory allocation and assertion functions by defining these macros before including the header:
```C
#define NN_MALLOC your_malloc_function
#define NN_ASSERT your_assert_function
```
