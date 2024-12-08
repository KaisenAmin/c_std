/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Numbers
*/

#ifndef NUMBERS_H_
#define NUMBERS_H_

#ifdef __cplusplus
extern "C" {
#endif 

// Mathematical constants
#define NUMBERS_E          2.71828182845904523536  // e
#define NUMBERS_LOG2E      1.44269504088896340736  // log2(e)
#define NUMBERS_LOG10E     0.434294481903251827651 // log10(e)
#define NUMBERS_PI         3.14159265358979323846  // pi
#define NUMBERS_INV_PI     0.318309886183790671538 // 1/pi
#define NUMBERS_INV_SQRTPI 0.564189583547756286948 // 1/sqrt(pi)
#define NUMBERS_LN2        0.693147180559945309417 // ln(2)
#define NUMBERS_LN10       2.30258509299404568402  // ln(10)
#define NUMBERS_SQRT2      1.41421356237309504880  // sqrt(2)
#define NUMBERS_SQRT3      1.73205080756887729352  // sqrt(3)
#define NUMBERS_INV_SQRT3  0.577350269189625764509 // 1/sqrt(3)
#define NUMBERS_EGAMMA     0.577215664901532860606 // Euler-Mascheroni constant
#define NUMBERS_PHI        1.61803398874989484820  // Golden ratio
#define NUMBERS_TAU        6.283185307179586       // 2 * PI
#define NUMBERS_SQRT5      2.23606797749979        // sqrt(5)
#define NUMBERS_CUBE_RT2   1.25992104989487        // Cube root of 2

// Physics constants
#define NUMBERS_C          299792458.0            // Speed of light (m/s)
#define NUMBERS_G          6.67430e-11            // Gravitational constant (m^3 kg^-1 s^-2)
#define NUMBERS_PLANCK     6.62607015e-34         // Planck's constant (J·s)
#define NUMBERS_BOLTZ      1.380649e-23           // Boltzmann constant (J/K)
#define NUMBERS_AVOGADRO   6.02214076e23          // Avogadro's number (mol^-1)
#define NUMBERS_R          8.314462618            // Ideal gas constant (J/(mol·K))

// Conversion factors
#define NUMBERS_DEG_TO_RAD 0.017453292519943295   // Degrees to radians (PI / 180)
#define NUMBERS_RAD_TO_DEG 57.29577951308232      // Radians to degrees (180 / PI)
#define NUMBERS_INCH_TO_CM 2.54                   // Inches to centimeters
#define NUMBERS_CM_TO_INCH 0.39370078740157       // Centimeters to inches

// Length and Distance
#define NUMBERS_M_TO_KM        0.001            // Meters to kilometers
#define NUMBERS_KM_TO_M        1000.0           // Kilometers to meters
#define NUMBERS_M_TO_CM        100.0            // Meters to centimeters
#define NUMBERS_CM_TO_M        0.01             // Centimeters to meters
#define NUMBERS_FT_TO_M        0.3048           // Feet to meters
#define NUMBERS_M_TO_FT        3.28083989501312 // Meters to feet
#define NUMBERS_YD_TO_M        0.9144           // Yards to meters
#define NUMBERS_M_TO_YD        1.09361329833771 // Meters to yards
#define NUMBERS_MILE_TO_KM     1.609344         // Miles to kilometers
#define NUMBERS_KM_TO_MILE     0.62137119223733 // Kilometers to miles

// Mass and Weight
#define NUMBERS_KG_TO_LB       2.20462262184878  // Kilograms to pounds
#define NUMBERS_LB_TO_KG       0.45359237        // Pounds to kilograms
#define NUMBERS_OZ_TO_GRAM     28.349523125      // Ounces to grams
#define NUMBERS_GRAM_TO_OZ     0.03527396194958  // Grams to ounces
#define NUMBERS_TON_TO_KG      1000.0            // Metric tons to kilograms
#define NUMBERS_KG_TO_TON      0.001             // Kilograms to metric tons

// Area
#define NUMBERS_SQFT_TO_SQM    0.092903          // Square feet to square meters
#define NUMBERS_SQM_TO_SQFT    10.7639104167097  // Square meters to square feet
#define NUMBERS_ACRE_TO_SQM    4046.8564224      // Acres to square meters
#define NUMBERS_SQM_TO_ACRE    0.00024710538147  // Square meters to acres
#define NUMBERS_HECTARE_TO_SQM 10000.0           // Hectares to square meters
#define NUMBERS_SQM_TO_HECTARE 0.0001            // Square meters to hectares

// Volume
#define NUMBERS_L_TO_ML        1000.0            // Liters to milliliters
#define NUMBERS_ML_TO_L        0.001             // Milliliters to liters
#define NUMBERS_GAL_TO_L       3.785411784       // US gallons to liters
#define NUMBERS_L_TO_GAL       0.26417205235815  // Liters to US gallons
#define NUMBERS_CUBIC_M_TO_L   1000.0            // Cubic meters to liters
#define NUMBERS_L_TO_CUBIC_M   0.001             // Liters to cubic meters
#define NUMBERS_CUBIC_IN_TO_L  0.016387064       // Cubic inches to liters
#define NUMBERS_L_TO_CUBIC_IN  61.0237440947323  // Liters to cubic inches

// Speed
#define NUMBERS_MPS_TO_KMPH    3.6               // Meters per second to kilometers per hour
#define NUMBERS_KMPH_TO_MPS    0.27777777777778  // Kilometers per hour to meters per second
#define NUMBERS_MPH_TO_KMPH    1.609344          // Miles per hour to kilometers per hour
#define NUMBERS_KMPH_TO_MPH    0.62137119223733  // Kilometers per hour to miles per hour
#define NUMBERS_KNOT_TO_KMPH   1.852             // Knots to kilometers per hour
#define NUMBERS_KMPH_TO_KNOT   0.53995680345572  // Kilometers per hour to knots

// Temperature
#define NUMBERS_C_TO_K         273.15            // Celsius to Kelvin
#define NUMBERS_K_TO_C        -273.15            // Kelvin to Celsius
#define NUMBERS_F_TO_C_MULT    0.55555555555556  // Factor for Fahrenheit to Celsius ((°F - 32) * NUMBERS_F_TO_C_MULT)
#define NUMBERS_C_TO_F_MULT    1.8               // Factor for Celsius to Fahrenheit (°C * NUMBERS_C_TO_F_MULT + 32)
#define NUMBERS_F_TO_C_OFFSET -32.0              // Offset for Fahrenheit to Celsius
#define NUMBERS_C_TO_F_OFFSET  32.0              // Offset for Celsius to Fahrenheit

// Energy and Power
#define NUMBERS_J_TO_CAL       0.23900573613767  // Joules to calories
#define NUMBERS_CAL_TO_J       4.184             // Calories to joules
#define NUMBERS_J_TO_KWH       2.77778e-7        // Joules to kilowatt-hours
#define NUMBERS_KWH_TO_J       3600000.0         // Kilowatt-hours to joules
#define NUMBERS_W_TO_HP        0.00134102209     // Watts to horsepower
#define NUMBERS_HP_TO_W        745.699872        // Horsepower to watts

// Time
#define NUMBERS_SEC_TO_MIN     0.01666666666667  // Seconds to minutes
#define NUMBERS_MIN_TO_SEC     60.0             // Minutes to seconds
#define NUMBERS_MIN_TO_HOUR    0.01666666666667 // Minutes to hours
#define NUMBERS_HOUR_TO_MIN    60.0             // Hours to minutes
#define NUMBERS_DAY_TO_HOUR    24.0             // Days to hours
#define NUMBERS_HOUR_TO_DAY    0.04166666666667 // Hours to days
#define NUMBERS_YEAR_TO_SEC    31556952.0       // Years to seconds (based on 365.2425 days/year)

#ifdef __cplusplus
}
#endif 

#endif 
