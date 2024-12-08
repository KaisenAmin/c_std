# Numbers Library in C

**Author:** amin tahmasebi
**Release Date:** 2024
**License:** ISC License


The Numbers library in C provides constants for common mathematical values, analogous to the `<numbers>` header introduced in C++20. It offers a simple way to access important mathematical constants in C programming, enhancing the precision and readability of mathematical computations.

## Compilation

To compile the Numbers library with your main program, include the `numbers.h` file in your project and use a standard GCC compilation command. If other libraries are needed, include their `.c` files in the command:

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c 
```

Make sure that GCC is installed on your system and that the `numbers.h` file is in the correct directory.

## Usage

To use the Numbers library, simply include the `numbers.h` header file in your C source files:

```c
#include "numbers.h"
```

## Constants Description

Each constant in the `numbers.h` library represents a fundamental mathematical value, providing high precision for mathematical calculations:


### **Mathematical Constants**
- **`NUMBERS_E`**: Euler's number (`e`), approximately 2.718, is the base of natural logarithms and is fundamental in calculus, particularly in exponential growth and decay problems.
- **`NUMBERS_LOG2E`**: Logarithm of `e` to the base 2, approximately 1.443, useful in binary logarithms and data compression algorithms.
- **`NUMBERS_LOG10E`**: Logarithm of `e` to the base 10, approximately 0.434, commonly used in logarithmic scales (e.g., pH, decibels).
- **`NUMBERS_PI`**: Pi (`π`), approximately 3.14159, represents the ratio of a circle's circumference to its diameter, fundamental in geometry and trigonometry.
- **`NUMBERS_INV_PI`**: The inverse of `π` (1/π), approximately 0.318, appears in normalization of Fourier transforms.
- **`NUMBERS_INV_SQRTPI`**: Reciprocal of the square root of `π` (1/√π), approximately 0.564, used in probability density functions.
- **`NUMBERS_LN2`**: Natural logarithm of 2, approximately 0.693, important in binary systems and entropy calculations.
- **`NUMBERS_LN10`**: Natural logarithm of 10, approximately 2.303, useful in base-10 logarithmic conversions.
- **`NUMBERS_SQRT2`**: Square root of 2, approximately 1.414, represents the diagonal of a unit square.
- **`NUMBERS_SQRT3`**: Square root of 3, approximately 1.732, appears in equilateral triangles and trigonometry.
- **`NUMBERS_INV_SQRT3`**: Reciprocal of the square root of 3 (1/√3), approximately 0.577, used in coordinate transformations.
- **`NUMBERS_EGAMMA`**: Euler-Mascheroni constant (`γ`), approximately 0.577, a constant in number theory and analysis.
- **`NUMBERS_PHI`**: The golden ratio (`φ`), approximately 1.618, often seen in art, architecture, and nature.
- **`NUMBERS_TAU`**: Tau (2π), approximately 6.283, an alternative to π in periodic functions.
- **`NUMBERS_SQRT5`**: Square root of 5, approximately 2.236, used in formulas for the golden ratio and Fibonacci numbers.
- **`NUMBERS_CUBE_RT2`**: Cube root of 2, approximately 1.26, used in scaling problems and volume calculations.

---

### **Physics Constants**
- **`NUMBERS_C`**: Speed of light in a vacuum, 299,792,458 m/s, a cornerstone of relativity and electromagnetic theory.
- **`NUMBERS_G`**: Gravitational constant, approximately 6.67430e-11 m³·kg⁻¹·s⁻², governs the force of gravity between two masses.
- **`NUMBERS_PLANCK`**: Planck's constant, approximately 6.626e-34 J·s, defines the scale of quantum effects.
- **`NUMBERS_BOLTZ`**: Boltzmann constant, approximately 1.380649e-23 J/K, relates temperature to energy.
- **`NUMBERS_AVOGADRO`**: Avogadro's number, 6.022e23 mol⁻¹, defines the number of particles in one mole.
- **`NUMBERS_R`**: Ideal gas constant, 8.314 J/(mol·K), appears in the ideal gas law.

---

### **Conversion Factors**
#### Angular Conversion:
- **`NUMBERS_DEG_TO_RAD`**: Converts degrees to radians (`π/180`), approximately 0.01745.
- **`NUMBERS_RAD_TO_DEG`**: Converts radians to degrees (`180/π`), approximately 57.296.

#### Length Conversion:
- **`NUMBERS_INCH_TO_CM`**: Converts inches to centimeters, exact value of 2.54.
- **`NUMBERS_CM_TO_INCH`**: Converts centimeters to inches, approximately 0.3937.

---

### **Length and Distance**
- **`NUMBERS_M_TO_KM`**: Converts meters to kilometers (1/1000).
- **`NUMBERS_KM_TO_M`**: Converts kilometers to meters (1000).
- **`NUMBERS_M_TO_CM`**: Converts meters to centimeters (100).
- **`NUMBERS_CM_TO_M`**: Converts centimeters to meters (1/100).
- **`NUMBERS_FT_TO_M`**: Converts feet to meters, approximately 0.3048.
- **`NUMBERS_M_TO_FT`**: Converts meters to feet, approximately 3.2808.
- **`NUMBERS_YD_TO_M`**: Converts yards to meters, approximately 0.9144.
- **`NUMBERS_M_TO_YD`**: Converts meters to yards, approximately 1.0936.
- **`NUMBERS_MILE_TO_KM`**: Converts miles to kilometers, approximately 1.609.
- **`NUMBERS_KM_TO_MILE`**: Converts kilometers to miles, approximately 0.621.

---

### **Mass and Weight**
- **`NUMBERS_KG_TO_LB`**: Converts kilograms to pounds, approximately 2.205.
- **`NUMBERS_LB_TO_KG`**: Converts pounds to kilograms, approximately 0.4536.
- **`NUMBERS_OZ_TO_GRAM`**: Converts ounces to grams, approximately 28.35.
- **`NUMBERS_GRAM_TO_OZ`**: Converts grams to ounces, approximately 0.03527.
- **`NUMBERS_TON_TO_KG`**: Converts metric tons to kilograms (1000).
- **`NUMBERS_KG_TO_TON`**: Converts kilograms to metric tons (1/1000).

---

### **Area**
- **`NUMBERS_SQFT_TO_SQM`**: Converts square feet to square meters, approximately 0.0929.
- **`NUMBERS_SQM_TO_SQFT`**: Converts square meters to square feet, approximately 10.764.
- **`NUMBERS_ACRE_TO_SQM`**: Converts acres to square meters, approximately 4046.856.
- **`NUMBERS_SQM_TO_ACRE`**: Converts square meters to acres, approximately 0.000247.
- **`NUMBERS_HECTARE_TO_SQM`**: Converts hectares to square meters (10,000).
- **`NUMBERS_SQM_TO_HECTARE`**: Converts square meters to hectares (1/10,000).

---

### **Volume**
- **`NUMBERS_L_TO_ML`**: Converts liters to milliliters (1000).
- **`NUMBERS_ML_TO_L`**: Converts milliliters to liters (1/1000).
- **`NUMBERS_GAL_TO_L`**: Converts US gallons to liters, approximately 3.785.
- **`NUMBERS_L_TO_GAL`**: Converts liters to US gallons, approximately 0.264.
- **`NUMBERS_CUBIC_M_TO_L`**: Converts cubic meters to liters (1000).
- **`NUMBERS_L_TO_CUBIC_M`**: Converts liters to cubic meters (1/1000).
- **`NUMBERS_CUBIC_IN_TO_L`**: Converts cubic inches to liters, approximately 0.01639.
- **`NUMBERS_L_TO_CUBIC_IN`**: Converts liters to cubic inches, approximately 61.024.

---

### **Speed**
- **`NUMBERS_MPS_TO_KMPH`**: Converts meters per second to kilometers per hour (3.6).
- **`NUMBERS_KMPH_TO_MPS`**: Converts kilometers per hour to meters per second (1/3.6).
- **`NUMBERS_MPH_TO_KMPH`**: Converts miles per hour to kilometers per hour, approximately 1.609.
- **`NUMBERS_KMPH_TO_MPH`**: Converts kilometers per hour to miles per hour, approximately 0.621.
- **`NUMBERS_KNOT_TO_KMPH`**: Converts knots to kilometers per hour, approximately 1.852.
- **`NUMBERS_KMPH_TO_KNOT`**: Converts kilometers per hour to knots, approximately 0.54.

---

### **Temperature**
- **`NUMBERS_C_TO_K`**: Converts Celsius to Kelvin (add 273.15).
- **`NUMBERS_K_TO_C`**: Converts Kelvin to Celsius (subtract 273.15).
- **`NUMBERS_F_TO_C_MULT`**: Multiplier for Fahrenheit to Celsius conversion (5/9).
- **`NUMBERS_C_TO_F_MULT`**: Multiplier for Celsius to Fahrenheit conversion (9/5).
- **`NUMBERS_F_TO_C_OFFSET`**: Offset to subtract 32 in Fahrenheit to Celsius conversion.
- **`NUMBERS_C_TO_F_OFFSET`**: Offset to add 32 in Celsius to Fahrenheit conversion.

---

### **Energy and Power**
- **`NUMBERS_J_TO_CAL`**: Converts joules to calories, approximately 0.239.
- **`NUMBERS_CAL_TO_J`**: Converts calories to joules (4.184).
- **`NUMBERS_J_TO_KWH`**: Converts joules to kilowatt-hours, approximately 2.778e-7.
- **`NUMBERS_KWH_TO_J`**: Converts kilowatt-hours to joules (3.6e6).
- **`NUMBERS_W_TO_HP`**: Converts watts to horsepower, approximately 0.00134.
- **`NUMBERS_HP_TO_W`**: Converts horsepower to watts, approximately 745.7.

---

### **Time**
- **`NUMBERS_SEC_TO_MIN`**: Converts seconds to minutes (1/60).
- **`NUMBERS_MIN_TO_SEC`**: Converts minutes to seconds (60).
- **`NUMBERS_MIN_TO_HOUR`**: Converts minutes to hours (1/60).
- **`NUMBERS_HOUR_TO_MIN`**: Converts hours to minutes (60).
- **`NUMBERS_DAY_TO_HOUR`**: Converts days to hours (24).
- **`NUMBERS_HOUR_TO_DAY`**: Converts hours to days (1/24).
- **`NUMBERS_YEAR_TO_SEC`**: Converts years to seconds, approximately 31,556,952 (based on 365.2425 days/year).

----

## Examples 

### Example 1: Calculating Compound Interest Using Euler's Number `(e)`

```c
#include "numbers/numbers.h"
#include "fmt/fmt.h"
#include <math.h>

int main() {
    double principal = 1000.0; // Initial investment
    double rate = 0.05;        // Annual interest rate
    int years = 10;            // Number of years

    // Compound interest formula: A = P * e^(rt)
    double amount = principal * pow(NUMBERS_E, rate * years);

    fmt_printf("Amount after %d years: $%.2f\n", years, amount);
    return 0;
}
```
**Result**
```
Amount after 10 years: $1648.72
```

---

### Example 2: Converting Radians to Degrees Using `Pi`

```c
#include "numbers/numbers.h"
#include "fmt/fmt.h"

int main() {
    double radians = 1.0; // Radians
    double degrees = radians * (180.0 / NUMBERS_PI); // Convert to degrees

    fmt_printf("%.2f radians is equal to %.2f degrees\n", radians, degrees);
    return 0;
}
```
**Result**
```
1.00 radians is equal to 57.30 degrees
```

---

### Example 3: Using the Golden Ratio `(Phi)`

```c
#include "numbers/numbers.h"
#include "fmt/fmt.h"
#include <math.h>

int main() {
    int n = 10; // nth position in Fibonacci sequence
    // Approximating the nth Fibonacci number using Golden Ratio
    double fib_n = (pow(NUMBERS_PHI, n) - pow(-NUMBERS_PHI, -n)) / sqrt(5);

    fmt_printf("Approximate %dth Fibonacci number: %.0f\n", n, fib_n);
    return 0;
}
```
**Result**
```
Approximate 10th Fibonacci number: 55
```

---

#### **Example 4: Calculate Distance Traveled by Light in Vacuum**
```c
#include "numbers/numbers.h"
#include "fmt/fmt.h"

int main() {
    double time_seconds = 10.0; 
    double distance = NUMBERS_C * time_seconds; 

    fmt_printf("Light travels %.2e meters in %.2f seconds.\n", distance, time_seconds);
    return 0;
}
```
**Result**
```
Light travels 3.00e+09 meters in 10.00 seconds.
```

---

#### **Example 5: Gravitational Force Between Two Masses**
```c
#include "numbers/numbers.h"
#include "fmt/fmt.h"

int main() {
    double mass1 = 5.0;     
    double mass2 = 10.0;    
    double distance = 2.0;  

    double force = (NUMBERS_G * mass1 * mass2) / (distance * distance);

    fmt_printf("Gravitational force: %.2e N\n", force);
    return 0;
}
```
**Result**
```
Gravitational force: 8.34e-10 N
```

---

#### **Example 6: Calculate Energy Using Planck's Constant**
```c
#include "numbers/numbers.h"
#include "fmt/fmt.h"

int main() {
    double frequency = 5.0e14; 
    double energy = NUMBERS_PLANCK * frequency;

    fmt_printf("Energy of photon: %.2e J\n", energy);
    return 0;
}
```
**Result**
```
Energy of photon: 3.31e-19 J
```

---

#### **Example 7: Convert Temperature from Fahrenheit to Celsius**
```c
#include "numbers/numbers.h"
#include "fmt/fmt.h"

int main() {
    double fahrenheit = 98.6;
    double celsius = (fahrenheit + NUMBERS_F_TO_C_OFFSET) * NUMBERS_F_TO_C_MULT;

    fmt_printf("%.2f°F is %.2f°C\n", fahrenheit, celsius);
    return 0;
}
```
**Result**
```
98.60F is 37.00C
```

---

#### **Example 8: Convert Speed from Kilometers per Hour to Miles per Hour**
```c
#include "numbers/numbers.h"
#include "fmt/fmt.h"

int main() {
    double speed_kmph = 100.0;
    double speed_mph = speed_kmph * NUMBERS_KMPH_TO_MPH;

    fmt_printf("%.2f km/h is %.2f mph\n", speed_kmph, speed_mph);
    return 0;
}
```
**Result**
```
100.00 km/h is 62.14 mph
```

---

#### **Example 9: Approximate Area of a Circle**
```c
#include "numbers/numbers.h"
#include "fmt/fmt.h"

int main() {
    double radius = 5.0; 
    double area = NUMBERS_PI * radius * radius;

    fmt_printf("Area of a circle with radius %.2f meters: %.2f square meters\n", radius, area);
    return 0;
}
```
**Result**
```
Area of a circle with radius 5.00 meters: 78.54 square meters
```

---

#### **Example 10: Calculating Natural Logarithm Using `NUMBERS_LN2`**

```c
#include "numbers/numbers.h"
#include "fmt/fmt.h"
#include <math.h>

int main() {
    double x = 8.0;
    // Using change of base formula: ln(x) = log2(x) * NUMBERS_LN2
    double ln_x = log2(x) * NUMBERS_LN2;

    fmt_printf("Natural logarithm of %.2f is approximately %.4f\n", x, ln_x);
    return 0;
}
```
**Result**
```
Natural logarithm of 8.00 is approximately 2.0794
```

---

#### **Example 11: Converting Inches to Centimeters Using `NUMBERS_INCH_TO_CM`**

```c
#include "numbers/numbers.h"
#include "fmt/fmt.h"

int main() {
    double length_in_inches = 12.0; // Length in inches
    double length_in_cm = length_in_inches * NUMBERS_INCH_TO_CM; // Convert to centimeters

    fmt_printf("%.2f inches is equal to %.2f centimeters\n", length_in_inches, length_in_cm);
    return 0;
}
```
**Result**
```
12.00 inches is equal to 30.48 centimeters
```

---

#### **Example 12: Calculating Energy from Temperature Using Boltzmann Constant**

```c
#include "numbers/numbers.h"
#include "fmt/fmt.h"

int main() {
    double temperature = 300.0; // Temperature in Kelvin
    double energy = NUMBERS_BOLTZ * temperature; // Energy in joules

    fmt_printf("Thermal energy at %.2f K is %.2e J\n", temperature, energy);
    return 0;
}
```
**Result**
```
Thermal energy at 300.00 K is 4.14e-21 J
```

---

## License

This project is open-source and available under [ISC License].