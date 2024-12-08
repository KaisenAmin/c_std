# UnitTest Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License  

## Overview

The UnitTest library is a lightweight and minimal testing framework for the C programming language, designed to facilitate unit testing of C programs. It provides essential features for creating, running, and reporting test cases, making it an ideal choice for testing C libraries and applications.

## **Key Features**

1. **Simple API**: Minimal boilerplate code for creating and running tests.
2. **Cross-Platform**: Fully compatible with Windows, Linux, and macOS.
3. **Performance Measurement**: Measure both real and CPU execution times for tests.
4. **Assertions**: 
   - Integer, floating-point, and string comparison.
   - Custom condition checks.
5. **Memory Leak Detection**: Automatically detect and report memory leaks in test cases.
6. **Parameterized Tests**: Test cases can accept parameters for versatile testing scenarios.
7. **Setup and Teardown**: Customizable functions for pre-test and post-test setup.
8. **Detailed Reports**: Generate concise summaries of test results.
9. **Timeouts**: Enforce execution time limits for test cases.
10. **Safe Resource Management**: Safeguard memory usage with `UNITTEST_MALLOC` and `UNITTEST_FREE`.

---

## Structure
The UnitTest library consists of:
- **Header File**: `unittest.h` defines the API, macros, and data structures.
- **Implementation File**: `unittest.c` provides the implementation of the API and utility functions.

---

## Functions and Macros

### Core Functions

#### `double unittest_timer_real(void)`
- **Purpose**: Measures real (wall-clock) time elapsed since the first test was run.
- **Parameters**: None.
- **Returns**: Time in seconds as a `double`.

---

#### `double unittest_timer_cpu(void)`
- **Purpose**: Measures CPU time consumed by the test process since the first test was run.
- **Parameters**: None.
- **Returns**: CPU time in seconds as a `double`.

---

#### `void unittest_report(void)`
- **Purpose**: Prints a summary of all test results, including the number of tests, assertions, and failures.
- **Parameters**: None.
- **Returns**: Nothing (void).

---

### Macros

#### `UNITTEST_CASE(method_name)`
- **Purpose**: Defines a test case function.
- **Parameters**: 
  - `method_name`: The name of the test case function.
- **Returns**: Nothing (void).

---

#### `UNITTEST_SUITE(suite_name)`
- **Purpose**: Defines a suite of related test cases.
- **Parameters**:
  - `suite_name`: The name of the suite function.
- **Returns**: Nothing (void).

---

#### `UNITTEST_RUN_SUITE(suite_name)`
- **Purpose**: Runs a test suite.
- **Parameters**:
  - `suite_name`: The name of the suite function to run.
- **Returns**: Nothing (void).

---

#### `UNITTEST_SUITE_CONFIGURE(setup_func, teardown_func)`
- **Purpose**: Configures setup and teardown functions for a test suite.
- **Parameters**:
  - `setup_func`: Pointer to the setup function.
  - `teardown_func`: Pointer to the teardown function.
- **Returns**: Nothing (void).

---

#### `UNITTEST_RUN_TEST(test)`
- **Purpose**: Runs an individual test case within a suite.
- **Parameters**:
  - `test`: The name of the test case function to run.
- **Returns**: Nothing (void).

---

#### `UNITTEST_REPORT()`
- **Purpose**: Generates and prints the test results summary.
- **Parameters**: None.
- **Returns**: Nothing (void).

---

#### `UNITTEST_RETRY_COUNT(count)`
- **Purpose**: Allows retrying a failed test a specified number of times before marking it as failed.
- **Parameters**:
  - `count`: The maximum number of retry attempts for a failed test.
- **Returns**: Nothing (void).
- **Behavior**:
  - Retries a test up to `count` times if it initially fails.
  - If the test passes in any of the retries, it is marked as successful.
  - Logs each retry attempt, providing details about the failure and retry process.

---

#### `UNITTEST_CHECK_NO_LEAKS(code_block)`
- **Purpose**: Ensures that no memory is leaked during the execution of a code block.
- **Parameters**: `code_block` - Code to execute and monitor.

---

### Assertions

#### `unittest_check(condition)`
- **Purpose**: Checks a condition, marking the test as failed if the condition is false.
- **Parameters**:
  - `condition`: Boolean condition to check.
- **Returns**: Nothing (void).

---

#### `unittest_fail(message)`
- **Purpose**: Fails a test with a custom message.
- **Parameters**:
  - `message`: A string describing the failure.
- **Returns**: Nothing (void).

---

#### `unittest_assert(condition, message)`
- **Purpose**: Asserts a condition with a custom failure message.
- **Parameters**:
  - `condition`: Boolean condition to assert.
  - `message`: A string describing the failure if the assertion fails.
- **Returns**: Nothing (void).

---


#### `unittest_assert_int_eq(expected, actual, message_format, ...)`
- **Purpose**: Asserts that two integers are equal. Logs a formatted failure message if the values differ.
- **Parameters**:
  - `expected`: Expected integer value.
  - `actual`: Actual integer value.
  - `message_format`: A format string for the failure message.
  - `...`: Additional arguments for the format string.
- **Returns**: Nothing (void).

---

#### `unittest_assert_double_eq(expected, actual, message_format, ...)`
- **Purpose**: Asserts that two double-precision floating-point numbers are equal within a specified epsilon. Logs a formatted failure message if the values differ.
- **Parameters**:
  - `expected`: Expected double-precision floating-point value.
  - `actual`: Actual double-precision floating-point value.
  - `message_format`: A format string for the failure message.
  - `...`: Additional arguments for the format string.
- **Returns**: Nothing (void).

---

#### `unittest_assert_string_eq(expected, actual, message_format, ...)`
- **Purpose**: Asserts that two strings are equal. Logs a formatted failure message if the strings differ. Handles `NULL` pointers gracefully by treating them as `<null>`.
- **Parameters**:
  - `expected`: Expected string value.
  - `actual`: Actual string value.
  - `message_format`: A format string for the failure message.
  - `...`: Additional arguments for the format string.
- **Returns**: Nothing (void).

---

#### `unittest_assert_not_null(pointer, message_format, ...)`
- **Purpose**: Asserts that a pointer is not `NULL`. Logs a formatted failure message if the pointer is `NULL`.
- **Parameters**:
  - `pointer`: The pointer to check.
  - `message_format`: A format string for the failure message.
  - `...`: Additional arguments for the format string.
- **Returns**: Nothing (void).

----

#### `unittest_assert_in_range(value, min, max, message_format, ...)`
- **Purpose**: This macro checks if a given value falls within a specified inclusive range (min to max). If the value is outside the range, it logs an error and fails the test.
- **Parameters**:
   - `value`: The value to check.
   - `min`: The minimum acceptable value.
   - `max`: The maximum acceptable value.
   - `message_format`: Custom error message format string.
   - `...`: Additional arguments for the error message.
- **Returns**: Nothing (void).

---

### **Memory Leak Detection**
The framework now includes tools for memory tracking:
- Allocate memory using `UNITTEST_MALLOC`.
- Free memory using `UNITTEST_FREE`.
- Check for leaks in any block of code with `UNITTEST_CHECK_NO_LEAKS`.

### **Memory Management**
#### `void* UNITTEST_MALLOC(size_t size)`
- **Purpose**: Allocates memory and tracks its usage.
- **Parameters**: `size` - Size in bytes to allocate.
- **Returns**: Pointer to the allocated memory.

#### `void UNITTEST_FREE(void* ptr)`
- **Purpose**: Frees memory and updates the tracking system.
- **Parameters**: `ptr` - Pointer to the memory to free.

#### `size_t unittest_get_memory_usage(void)`
- **Purpose**: Returns the total allocated memory currently in use.
- **Parameters**: None.
- **Returns**: Total memory in bytes.

---

### **Timing Functions**
#### `double unittest_timer_real(void)`
- **Purpose**: Measures wall-clock time.
- **Returns**: Time in seconds as a `double`.

#### `double unittest_timer_cpu(void)`
- **Purpose**: Measures CPU time.
- **Returns**: Time in seconds as a `double`.

#### `void unittest_set_timeout(double timeout_ms)`
- **Purpose**: Sets a timeout in milliseconds for test execution.
- **Parameters**: `timeout_ms` - Timeout value in milliseconds.

---


## Examples

### Example 1: Basic UnitTest Case
```c
#include "unittest/unittest.h"

UNITTEST_CASE(test_addition) {
    int result = 2 + 2;
    unittest_assert_int_eq(5, result, "Expected %d but got %d", 4, result);
}

UNITTEST_SUITE(math_tests) {
    UNITTEST_RUN_TEST(test_addition);
}

int main() {
    UNITTEST_RUN_SUITE(math_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result:**
```
Elapsed time for test_addition: 0.01 ms
Test failed: test_addition. Retrying...
Test ultimately failed after retries: test_addition


1 tests, 1 assertions, 1 failures, 0 skipped

Finished in 0.00581600 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 2: Using Setup and Teardown Functions
```c
#include "unittest/unittest.h"

void setup() {
    // Initialize resources
}

void teardown() {
    // Clean up resources
}

UNITTEST_CASE(test_example) {
    unittest_assert(1 == 1, "This should always pass");
}

UNITTEST_SUITE(example_suite) {
    UNITTEST_SUITE_CONFIGURE(setup, teardown);
    UNITTEST_RUN_TEST(test_example);
}

int main() {
    UNITTEST_RUN_SUITE(example_suite);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
1 tests, 1 assertions, 0 failures

Finished in 0.00268240 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 3: String and Floating Point Assertions
```c
#include "unittest/unittest.h"

UNITTEST_CASE(test_strings) {
    const char* expected = "Hello, World!";
    const char* actual = "Hello";
    unittest_assert_string_eq(expected, actual, "Expected '%s' but got '%s'", expected, actual);
}

UNITTEST_CASE(test_floats) {
    double expected = 3.14;
    double actual = 3.14000001;
    unittest_assert(fabs(expected - actual) < 1E-7, "Floats are not approximately equal");
}

UNITTEST_SUITE(misc_tests) {
    UNITTEST_RUN_TEST(test_strings);
    UNITTEST_RUN_TEST(test_floats);
}

int main() {
    UNITTEST_RUN_SUITE(misc_tests);
    UNITTEST_REPORT();
    
    return UNITTEST_EXIT_CODE;
}

```

**Result**
```
test_strings failed:
        main.c:6: Expected 'Hello, World!' but got 'Hello, '
Elapsed time for test_strings: 5.24 ms
Test failed: test_strings. Retrying...
Test ultimately failed after retries: test_strings
.Elapsed time for test_floats: 0.04 ms


2 tests, 2 assertions, 1 failures, 0 skipped

Finished in 0.01202710 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 4: Timing a Test Case
```c
#include "unittest/unittest.h"

UNITTEST_CASE(test_timing) {
    double start = unittest_timer_real();
    for (volatile int i = 0; i < 1000000; ++i); 
    
    double end = unittest_timer_real();
    unittest_assert((end - start) > 0.0, "Execution time should be positive");
}

UNITTEST_SUITE(timing_suite) {
    UNITTEST_RUN_TEST(test_timing);
}

int main() {
    UNITTEST_RUN_SUITE(timing_suite);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```

**Result**
```

1 tests, 1 assertions, 0 failures

Finished in 0.00345860 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 5: Integer Equality Failure
```c
#include "unittest/unittest.h"

UNITTEST_CASE(test_integer_failure) {
    int expected = 10;
    int actual = 5 + 4; 
    unittest_assert_int_eq(expected, actual, "Expected %d but got %d", expected, actual);
}

UNITTEST_SUITE(integer_failure_tests) {
    UNITTEST_RUN_TEST(test_integer_failure);
}

int main() {
    UNITTEST_RUN_SUITE(integer_failure_tests);
    UNITTEST_REPORT();

    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
Elapsed time for test_integer_failure: 0.01 ms
Test failed: test_integer_failure. Retrying...
Test ultimately failed after retries: test_integer_failure


1 tests, 1 assertions, 1 failures, 0 skipped

Finished in 0.00909300 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 6: Double Precision Failure
```c
#include "unittest/unittest.h"

UNITTEST_CASE(test_double_failure) {
    double expected = 9.0;
    double actual = 3.01 * 3.01;  
    unittest_assert_double_eq(expected, actual, "Expected %.5f but got %.5f", expected, actual);
}

UNITTEST_SUITE(double_failure_tests) {
    UNITTEST_RUN_TEST(test_double_failure);
}

int main() {
    UNITTEST_RUN_SUITE(double_failure_tests);
    UNITTEST_REPORT();

    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
test_double_failure failed:
        main.c:6: Expected 9.00000 but got 9.06010
Elapsed time for test_double_failure: 5.15 ms
Test failed: test_double_failure. Retrying...
Test ultimately failed after retries: test_double_failure


1 tests, 1 assertions, 1 failures, 0 skipped

Finished in 0.01118970 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 7: String Equality Failure
```c
#include "unittest/unittest.h"

UNITTEST_CASE(test_string_failure) {
    const char* expected = "UnitTest";
    const char* actual = "TestUnit";  
    unittest_assert_string_eq(expected, actual, "String mismatch: Expected '%s' but got '%s'", expected, actual);
}

UNITTEST_SUITE(string_failure_tests) {
    UNITTEST_RUN_TEST(test_string_failure);
}

int main() {
    UNITTEST_RUN_SUITE(string_failure_tests);
    UNITTEST_REPORT();

    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
test_string_failure failed:
        main.c:6: String mismatch: Expected 'UnitTest' but got 'TestUnit'
Elapsed time for test_string_failure: 5.92 ms
Test failed: test_string_failure. Retrying...
Test ultimately failed after retries: test_string_failure


1 tests, 1 assertions, 1 failures, 0 skipped

Finished in 0.01298610 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 8: Custom Assertion Failure
```c
#include "unittest/unittest.h"

UNITTEST_CASE(test_custom_failure) {
    int value = 35;
    unittest_assert(value > 40, "Value should be greater than 40");  
}

UNITTEST_SUITE(custom_failure_tests) {
    UNITTEST_RUN_TEST(test_custom_failure);
}

int main() {
    UNITTEST_RUN_SUITE(custom_failure_tests);
    UNITTEST_REPORT();

    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
F
test_custom_failure failed:
        main.c:6: Value should be greater than 40

1 tests, 1 assertions, 1 failures

Finished in 0.00012000 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 9: Multiple Failures in a Suite
```c
#include "unittest/unittest.h"

UNITTEST_CASE(test_integer_failure) {
    int expected = 10;
    int actual = 9;
    unittest_assert_int_eq(expected, actual, "Expected %d but got %d", expected, actual);
}

UNITTEST_CASE(test_double_failure) {
    double expected = 3.14;
    double actual = 3.15;
    unittest_assert_double_eq(expected, actual, "Expected %.2f but got %.2f", expected, actual);
}

UNITTEST_CASE(test_string_failure) {
    const char* expected = "Hello";
    const char* actual = "World";
    unittest_assert_string_eq(expected, actual, "Expected '%s' but got '%s'", expected, actual);
}

UNITTEST_SUITE(multiple_failures) {
    UNITTEST_RUN_TEST(test_integer_failure);
    UNITTEST_RUN_TEST(test_double_failure);
    UNITTEST_RUN_TEST(test_string_failure);
}

int main() {
    UNITTEST_RUN_SUITE(multiple_failures);
    UNITTEST_REPORT();

    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
test_integer_failure failed:
        main.c:6: Expected 10 but got 9
Elapsed time for test_integer_failure: 4.56 ms
Test failed: test_integer_failure. Retrying...
Test ultimately failed after retries: test_integer_failure
test_double_failure failed:
        main.c:12: Expected 3.14 but got 3.15
Elapsed time for test_double_failure: 4.34 ms
Test failed: test_double_failure. Retrying...
Test ultimately failed after retries: test_double_failure
test_string_failure failed:
        main.c:18: Expected 'Hello' but got 'World'
Elapsed time for test_string_failure: 4.88 ms
Test failed: test_string_failure. Retrying...
Test ultimately failed after retries: test_string_failure


3 tests, 3 assertions, 3 failures, 0 skipped

Finished in 0.02825010 seconds (real) 0.00000000 seconds (proc)
``` 

---

### Example 10 : Regex Matching Test Case 

```c
#include "unittest/unittest.h"
#include "regex/std_regex.h"

UNITTEST_CASE(test_regex_basic_match) {
    const char* pattern = "hello";
    const char* test_string = "hello world!";
    
    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    unittest_assert(regex != NULL, "Failed to compile regex");
    
    RegexMatch match;
    RegexResult result = regex_search(regex, test_string, &match);
    unittest_assert(result == REGEX_SUCCESS, "Expected a successful match");

    char matched_string[match.length + 1]; 
    strncpy(matched_string, match.start, match.length);
    matched_string[match.length] = '\0';  
    
    unittest_assert_string_eq("hello", matched_string, "Expected '%s' but got '%s'", "hello", matched_string);
    
    regex_deallocate(regex);
}

UNITTEST_SUITE(regex_tests) {
    UNITTEST_RUN_TEST(test_regex_basic_match);
}

int main() {
    UNITTEST_RUN_SUITE(regex_tests);
    UNITTEST_REPORT();

    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
...
Elapsed time for test_regex_basic_match: 1.13 ms


1 tests, 3 assertions, 0 failures, 0 skipped

Finished in 0.00368750 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 11: Random Integer Generation Test Case

```c

```c
#include "unittest/unittest.h"
#include "random/random.h"

UNITTEST_CASE(test_random_integer_generation) {
    int min = 1, max = 10;

    random_seed(42);  
    for (int i = 0; i < 5; ++i) {
        int random_number = random_randint(min, max);
        unittest_assert(random_number >= min && random_number <= max, "Generated number is out of range");
    }
}

UNITTEST_SUITE(random_tests) {
    UNITTEST_RUN_TEST(test_random_integer_generation);
}

int main() {
    UNITTEST_RUN_SUITE(random_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
***Result**
```
1 tests, 5 assertions, 0 failures

Finished in 0.04441680 seconds (real) 0.00000000 seconds (proc)
```

--- 

### Example 12 : Statistics Lib 

```c
#include "unittest/unittest.h"
#include "statistics/statistics.h"
#include <math.h>

UNITTEST_CASE(test_mean_calculation) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    size_t n = sizeof(data) / sizeof(data[0]);

    double mean = statistics_mean(data, n);
    unittest_assert_double_eq(3.0, mean, "Expected mean %.2f but got %.2f", 3.0, mean);
}

UNITTEST_CASE(test_median_calculation) {
    double data[] = {5.0, 3.0, 1.0, 4.0, 2.0};
    size_t n = sizeof(data) / sizeof(data[0]);

    double median = statistics_median(data, n);
    unittest_assert_double_eq(3.0, median, "Expected median %.2f but got %.2f", 3.0, median);
}

UNITTEST_CASE(test_variance_calculation) {
    double data[] = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    size_t n = sizeof(data) / sizeof(data[0]);

    double variance = statistics_variance(data, n, false, 0.0);  
    double expected_variance = 4.5714; // Variance with the correct value
    unittest_assert_double_eq(expected_variance, variance, "Expected variance %.4f but got %.4f", expected_variance, variance);
}

UNITTEST_SUITE(statistics_tests) {
    UNITTEST_RUN_TEST(test_mean_calculation);
    UNITTEST_RUN_TEST(test_median_calculation);
    UNITTEST_RUN_TEST(test_variance_calculation);
}

int main() {
    UNITTEST_RUN_SUITE(statistics_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
Failed Tests :
----------
.Elapsed time for test_mean_calculation: 6.08 ms
.Elapsed time for test_median_calculation: 73.20 ms
test_variance_calculation failed:
        C:/Users/asus/OneDrive/Desktop/project/c_std/main.c:27: Expected variance 4.5714 but got 4.5714
Elapsed time for test_variance_calculation: 17.60 ms
Test failed: test_variance_calculation. Retrying...
Test ultimately failed after retries: test_variance_calculation


3 tests, 3 assertions, 1 failures, 0 skipped

Finished in 0.10827450 seconds (real) 0.00000000 seconds (proc)
```

--- 

### Example 13 : Parameterized Tests 

```c
#include "unittest/unittest.h"

int multiply(int a, int b) {
    return a * b;
}

UNITTEST_CASE(test_multiplication) {
    int* param = (int*)unittest_param; 
    int a = param[0];
    int b = param[1];
    int expected = param[2];

    int actual = multiply(a, b);
    unittest_assert_int_eq(expected, actual, "Expected multiply(%d, %d) to be %d but got %d", a, b, expected, actual);
}

UNITTEST_SUITE(multiplication_tests) {

    int params[][3] = {
        {1, 1, 1},
        {2, 3, 6},
        {-2, 4, -8},
        {0, 5, 0},
        {100, 0, 0}
    };

    UNITTEST_RUN_PARAM_TEST(test_multiplication, params, sizeof(params) / sizeof(params[0]));
}

int main() {
    UNITTEST_RUN_SUITE(multiplication_tests);
    UNITTEST_REPORT(); 

    return UNITTEST_EXIT_CODE; 
}
```
**Result**
```
5 tests, 5 assertions, 0 failures

Finished in 0.00034030 seconds (real) 0.00000000 seconds (proc)
```

--- 

### Example 14 : Test Skip Feature 
```c
#include "unittest/unittest.h"

int is_feature_enabled() {
    return 0; 
}

UNITTEST_CASE(test_feature) {
    if (!is_feature_enabled()) {
        UNITTEST_SKIP("Feature is not enabled; skipping test.");
    }

    int result = 2 + 2;
    unittest_assert_int_eq(4, result, "Expected 4 and result is %d", result);
}

UNITTEST_CASE(test_always_run) {
    int result = 2 * 3;
    unittest_assert_int_eq(6, result, "Expected 6 and result is %d", result);
}

UNITTEST_SUITE(example_tests) {
    UNITTEST_RUN_TEST(test_feature);
    UNITTEST_RUN_TEST(test_always_run);
}

int main() {
    UNITTEST_RUN_SUITE(example_tests);
    UNITTEST_REPORT();

    return UNITTEST_EXIT_CODE;
}
```
**Result**
```

2 tests, 1 assertions, 0 failures, 1 skipped

Skipped Tests:
---------------
test_feature skipped:
        File: main.c
        Line: 9
        Reason: Feature is not enabled; skipping test.

Finished in 0.00016000 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 15 : Timeout Feature 

```c
#include "unittest/unittest.h"


UNITTEST_CASE(test_long_running) {
    for (volatile int i = 0; i < 1000000000; ++i); 
    unittest_assert(1 == 1, "This should pass.");
}

UNITTEST_CASE(test_fast) {
    int result = 2 + 2;
    unittest_assert_int_eq(4, result, "Expected 4 and result is %d", result);
}

UNITTEST_SUITE(example_tests) {
    UNITTEST_RUN_TEST(test_long_running);
    UNITTEST_RUN_TEST(test_fast);
}

int main() {
    unittest_set_timeout(10000); 

    UNITTEST_RUN_SUITE(example_tests);    
    UNITTEST_REPORT();                    

    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
.Elapsed time for test_long_running: 1336.24 ms
.Elapsed time for test_fast: 0.06 ms


2 tests, 2 assertions, 0 failures, 0 skipped

Finished in 1.34042110 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 16 : Memory Leak Detection 

```c
#include "unittest/unittest.h"


void my_function_that_leaks_memory() {
    UNITTEST_MALLOC(100); // Memory allocated but not freed
}

void my_function_with_no_leak() {
    void* ptr = UNITTEST_MALLOC(100);
    UNITTEST_FREE(ptr);
}

UNITTEST_CASE(test_memory_leak) {
    UNITTEST_CHECK_NO_LEAKS({
        my_function_that_leaks_memory();
    });
}

UNITTEST_CASE(test_no_memory_leak) {
    UNITTEST_CHECK_NO_LEAKS({
        my_function_with_no_leak();
    });
}

UNITTEST_SUITE(memory_tests) {
    UNITTEST_RUN_TEST(test_memory_leak);
    UNITTEST_RUN_TEST(test_no_memory_leak);
}

int main() {
    UNITTEST_RUN_SUITE(memory_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
Memory leak detected: 100 bytes not freed. Allocated at main.c:5
Elapsed time for test_memory_leak: 5.30 ms
Failed Tests :
----------
Memory leak detected: 100 bytes not freed
.Elapsed time for test_no_memory_leak: 0.04 ms


2 tests, 0 assertions, 1 failures, 0 skipped

Finished in 0.01151540 seconds (real) 0.00000000 seconds (proc)
```

--- 
### Example 17 : No Memory Leak 

```c
#include "unittest.h"

void my_function_with_no_leak() {
    void* ptr = UNITTEST_MALLOC(100); // Allocate memory
    UNITTEST_FREE(ptr);               // Properly free memory
}

UNITTEST_CASE(test_no_memory_leak) {
    UNITTEST_CHECK_NO_LEAKS({
        my_function_with_no_leak();
    });
}

UNITTEST_SUITE(memory_tests) {
    UNITTEST_RUN_TEST(test_no_memory_leak);
}

int main() {
    UNITTEST_RUN_SUITE(memory_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
.Elapsed time for test_no_memory_leak: 0.09 ms


1 tests, 0 assertions, 0 failures, 0 skipped

Finished in 0.00197990 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 18 : Retry Mechanism 

```c
#include "unittest.h"

UNITTEST_CASE(test_flaky) {
    static int fail_count = 2; 
    if (fail_count > 0) {
        fail_count--;
        unittest_fail("Flaky test failed!");
    }
    unittest_assert(1 == 1, "This should pass after retries");
}

UNITTEST_SUITE(flaky_suite) {
    UNITTEST_RETRY_COUNT(3); // 3 retries
    UNITTEST_RUN_TEST(test_flaky);
}

int main() {
    UNITTEST_RUN_SUITE(flaky_suite);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
Elapsed time for test_flaky: 0.01 ms
Test failed: test_flaky. Retrying...
Elapsed time for test_flaky: 0.00 ms
Test failed: test_flaky. Retrying...
.Elapsed time for test_flaky: 0.04 ms


3 tests, 3 assertions, 0 failures, 0 skipped

Finished in 0.00818300 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 19 : Check null assertion 

```c
#include "unittest/unittest.h"
#include <stdlib.h>

UNITTEST_CASE(test_pointer_not_null) {
    int* ptr = (int*)malloc(sizeof(int));

    unittest_assert_not_null(ptr, "Pointer should not be NULL after malloc");

    free(ptr);

    ptr = NULL;
    unittest_assert_not_null(ptr, "Pointer is <null>"); 
}

UNITTEST_SUITE(pointer_tests) {
    UNITTEST_RUN_TEST(test_pointer_not_null);
}

int main() {
    UNITTEST_RUN_SUITE(pointer_tests);
    UNITTEST_REPORT();

    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
.test_pointer_not_null failed:
        main.c:13: Pointer is <null>
Elapsed time for test_pointer_not_null: 4.60 ms
Test failed: test_pointer_not_null. Retrying...
Test ultimately failed after retries: test_pointer_not_null


1 tests, 2 assertions, 1 failures, 0 skipped

Finished in 0.01094550 seconds (real) 0.00000000 seconds (proc)
```

--- 

### Example 20 : assertion for in range value 

```c
#include "unittest/unittest.h"

UNITTEST_CASE(test_value_in_range) {
    double value = 15.0;
    double min = 10.0;
    double max = 20.0;

    unittest_assert_in_range(value, min, max, "Value %.2f is not in range [%.2f, %.2f]", value, min, max);
}

UNITTEST_CASE(test_value_out_of_range) {
    double value = 25.0;
    double min = 10.0;
    double max = 20.0;

    unittest_assert_in_range(value, min, max, "Value %.2f is not in range [%.2f, %.2f]", value, min, max);
}

UNITTEST_SUITE(range_tests) {
    UNITTEST_RUN_TEST(test_value_in_range);
    UNITTEST_RUN_TEST(test_value_out_of_range);
}

int main() {
    UNITTEST_RUN_SUITE(range_tests);
    UNITTEST_REPORT();

    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
.Elapsed time for test_value_in_range: 0.08 ms
test_value_out_of_range failed:
        main.c:16: Value 25.00 is not in range [10.00, 20.00]
Elapsed time for test_value_out_of_range: 5.27 ms
Test failed: test_value_out_of_range. Retrying...
Test ultimately failed after retries: test_value_out_of_range


2 tests, 2 assertions, 1 failures, 0 skipped

Finished in 0.01392990 seconds (real) 0.00000000 seconds (proc)
```

---
### Example 21 : Arrays assertions 

```c
#include "unittest/unittest.h"

UNITTEST_CASE(test_arrays_equal) {
    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 3, 4, 5};
    int size = sizeof(expected) / sizeof(expected[0]);

    unittest_assert_array_int_eq(expected, actual, size, "Arrays are not equal");
}

UNITTEST_CASE(test_arrays_not_equal) {
    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 0, 4, 5};
    int size = sizeof(expected) / sizeof(expected[0]);

    unittest_assert_array_int_eq(expected, actual, size, "Arrays mismatch");
}

UNITTEST_CASE(test_string_arrays_equal) {
    const char* expected[] = {"hello", "world", "unit", "test"};
    const char* actual[] = {"hello", "world", "unit", "test"};
    int size = sizeof(expected) / sizeof(expected[0]);

    for (int i = 0; i < size; i++) {
        unittest_assert_string_eq(expected[i], actual[i], "String arrays mismatch at index %d", i);
    }
}

UNITTEST_CASE(test_string_arrays_not_equal) {
    const char* expected[] = {"hello", "world", "unit", "test"};
    const char* actual[] = {"hello", "world", "unittest", "test"};
    int size = sizeof(expected) / sizeof(expected[0]);

    for (int i = 0; i < size; i++) {
        unittest_assert_string_eq(expected[i], actual[i], "String arrays mismatch at index %d", i);
    }
}

UNITTEST_CASE(test_double_arrays_equal) {
    double expected[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    double actual[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    int size = sizeof(expected) / sizeof(expected[0]);

    for (int i = 0; i < size; i++) {
        unittest_assert_double_eq(expected[i], actual[i], "Double arrays mismatch at index %d: expected %.2f, got %.2f", i, expected[i], actual[i]);
    }
}

UNITTEST_CASE(test_double_arrays_not_equal) {
    double expected[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    double actual[] = {1.1, 2.2, 3.0, 4.4, 5.5};
    int size = sizeof(expected) / sizeof(expected[0]);

    for (int i = 0; i < size; i++) {
        unittest_assert_double_eq(expected[i], actual[i], "Double arrays mismatch at index %d: expected %.2f, got %.2f", i, expected[i], actual[i]);
    }
}

UNITTEST_SUITE(array_tests) {
    UNITTEST_RUN_TEST(test_arrays_equal);
    UNITTEST_RUN_TEST(test_arrays_not_equal);
    UNITTEST_RUN_TEST(test_string_arrays_equal);
    UNITTEST_RUN_TEST(test_string_arrays_not_equal);
    UNITTEST_RUN_TEST(test_double_arrays_equal);
    UNITTEST_RUN_TEST(test_double_arrays_not_equal);
}

int main() {
    UNITTEST_RUN_SUITE(array_tests);
    UNITTEST_REPORT();

    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
.Elapsed time for test_arrays_equal: 0.08 ms
test_arrays_not_equal failed:
        main.c:16: Arrays mismatch at index 2 (expected: 3, actual: 0)
Elapsed time for test_arrays_not_equal: 5.39 ms
Test failed: test_arrays_not_equal. Retrying...
Test ultimately failed after retries: test_arrays_not_equal
....Elapsed time for test_string_arrays_equal: 0.24 ms
..test_string_arrays_not_equal failed:
        main.c:35: String arrays mismatch at index 2
Elapsed time for test_string_arrays_not_equal: 5.17 ms
Test failed: test_string_arrays_not_equal. Retrying...
Test ultimately failed after retries: test_string_arrays_not_equal
.....Elapsed time for test_double_arrays_equal: 0.23 ms
..test_double_arrays_not_equal failed:
        main.c:55: Double arrays mismatch at index 2: expected 3.30, got 3.00
Elapsed time for test_double_arrays_not_equal: 5.86 ms
Test failed: test_double_arrays_not_equal. Retrying...
Test ultimately failed after retries: test_double_arrays_not_equal


6 tests, 17 assertions, 3 failures, 0 skipped

Finished in 0.04452300 seconds (real) 0.00000000 seconds (proc)
```

----

## Conclusion

The UnitTest library provides an efficient and user-friendly framework for writing unit tests in C. Its lightweight design and straightforward API make it suitable for projects of any size, ensuring high code quality and reliable functionality.