# Dynamic-Long-Numbers
This library implements two robust C++ classes: `BInteger` and `Rational`. These classes enable arithmetic operations and other utilities for working with arbitrarily large integers and rational numbers.
# BInt and RationalTemplate Library

## Overview

This library implements two core C++ classes: `BInt` and `RationalTemplate`. These classes enable efficient arithmetic and utility functions for arbitrarily large integers (`BInt`) and rational numbers (`RationalTemplate`) built on top of `BInt`.

### Features

1. **BInt Class**:
   - Handles representation and operations for arbitrarily large integers.
   - Supports basic arithmetic operations: addition, subtraction, multiplication, and division.
   - Includes advanced utility functions:
     - Normalization: Removes unnecessary leading zeros.
     - Modular arithmetic for use in number theory algorithms like primality testing.
     - Efficient multiplication using the Karatsuba algorithm.

2. **RationalTemplate Class**:
   - Represents rational numbers as a pair of numerator and denominator using the `BInt` class.
   - Implements basic arithmetic operations: addition, subtraction, multiplication, and division.
   - Automatically reduces fractions using the greatest common divisor (GCD).
   - Handles edge cases like division by zero gracefully.

---

## Usage

### BInt Class

The `BInt` class provides efficient operations for arbitrarily large integers:

#### Example:
```cpp
#include "BInt.h"

BInt num1("36423452523432434");
BInt num2("-18423432424542345");

BInt sum = num1 + num2;   // Addition
BInt diff = num1 - num2;  // Subtraction
BInt prod = num1 * num2;  // Multiplication
BInt quot = num1 / num2;  // Division

std::cout << "Sum: " << sum << "\n";
std::cout << "Difference: " << diff << "\n";
std::cout << "Product: " << prod << "\n";
std::cout << "Quotient: " << quot << "\n";
