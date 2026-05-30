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

### **Functions and Macros**

### Core Functions

### `double unittest_timer_real(void)`

**Purpose**: Measures real (wall-clock) time elapsed since the first test was run.

**Parameters**: None.

**Return Value**: Time in seconds as a `double`.

**Usage Case**: Use when you need to measure elapsed wall-clock time for a test or a block of code.

---

### `double unittest_timer_cpu(void)`

**Purpose**: Measures CPU time consumed by the test process since the first test was run.

**Parameters**: None.

**Return Value**: CPU time in seconds as a `double`.

**Usage Case**: Use when you want to measure actual CPU processing time (excluding I/O and sleep) for a test.

---

### `void unittest_report(void)`

**Purpose**: Prints a summary of all test results, including the number of tests, assertions, and failures.

**Parameters**: None.

**Return Value**: None.

**Usage Case**: Call at the end of `main()` to print a human-readable test summary.

---

### Macros

### `UNITTEST_CASE(method_name)`

**Purpose**: Defines a test case function.

**Parameters**:
- `method_name`: The name of the test case function.

**Return Value**: None.

**Usage Case**: Use to declare an individual test case that can be run inside a suite.

---

### `UNITTEST_SUITE(suite_name)`

**Purpose**: Defines a suite of related test cases.

**Parameters**:
- `suite_name`: The name of the suite function.

**Return Value**: None.

**Usage Case**: Use to group related test cases together and control their setup/teardown.

---

### `UNITTEST_RUN_SUITE(suite_name)`

**Purpose**: Runs a test suite.

**Parameters**:
- `suite_name`: The name of the suite function to run.

**Return Value**: None.

**Usage Case**: Call in `main()` to execute all tests grouped under a named suite.

---

### `UNITTEST_SUITE_CONFIGURE(setup_func, teardown_func)`

**Purpose**: Configures setup and teardown functions for a test suite.

**Parameters**:
- `setup_func`: Pointer to the setup function.
- `teardown_func`: Pointer to the teardown function.

**Return Value**: None.

**Usage Case**: Use inside a `UNITTEST_SUITE` body to register per-test setup and teardown callbacks.

---

### `UNITTEST_RUN_TEST(test)`

**Purpose**: Runs an individual test case within a suite.

**Parameters**:
- `test`: The name of the test case function to run.

**Return Value**: None.

**Usage Case**: Use inside a `UNITTEST_SUITE` body to schedule a test case for execution.

---

### `UNITTEST_REPORT()`

**Purpose**: Generates and prints the test results summary.

**Parameters**: None.

**Return Value**: None.

**Usage Case**: Call at the end of `main()` as a macro alternative to `unittest_report()`.

---

### `UNITTEST_RETRY_COUNT(count)`

**Purpose**: Allows retrying a failed test a specified number of times before marking it as failed.

**Parameters**:
- `count`: The maximum number of retry attempts for a failed test.

**Return Value**: None.

**Usage Case**: Use inside a suite to make flaky tests retry automatically before being marked as failures.

- **Behavior**:
  - Retries a test up to `count` times if it initially fails.
  - If the test passes in any of the retries, it is marked as successful.
  - Logs each retry attempt, providing details about the failure and retry process.

---

### `UNITTEST_CHECK_NO_LEAKS(code_block)`

**Purpose**: Ensures that no memory is leaked during the execution of a code block.

**Parameters**:
- `code_block`: Code to execute and monitor.

**Return Value**: None.

**Usage Case**: Wrap any code block that allocates memory via `UNITTEST_MALLOC` to automatically detect and report leaks.

---

### **Assertions**

### `unittest_check(condition)`

**Purpose**: Checks a condition, marking the test as failed if the condition is false.

**Parameters**:
- `condition`: Boolean condition to check.

**Return Value**: None.

**Usage Case**: Use for simple boolean assertions inside a test case.

---

### `unittest_fail(message)`

**Purpose**: Fails a test with a custom message.

**Parameters**:
- `message`: A string describing the failure.

**Return Value**: None.

**Usage Case**: Use to unconditionally fail a test with a descriptive message when an expected condition cannot be expressed as a boolean.

---

### `unittest_assert(condition, message_format, ...)`

**Purpose**: Validates a condition and logs a failure message if the condition is false.

**Parameters**:
- `condition`: A boolean expression to validate.
- `message_format`: A printf-style format string that describes the assertion failure.
- `...`: Additional arguments to format the failure message.

**Return Value**: None.

**Usage Case**: Use for formatted, conditional assertions where you need a descriptive failure message with printf-style formatting.

- **Behavior**:
  - If the condition is true, it logs a dot (`.`).
  - If the condition is false, it logs the failure message and terminates the test case.

---

### `unittest_assert_regex_match(pattern, string, flags, message_format, ...)`

**Purpose**: Asserts that a string matches a regex pattern.

**Parameters**:
- `pattern`: The regex pattern to match.
- `string`: The input string to validate against the regex.
- `flags`: Optional flags to control regex behavior (e.g., case-insensitive).
- `message_format`: A printf-style format string describing the failure.
- `...`: Additional arguments for the format string.

**Return Value**: None.

**Usage Case**: Use to validate that an input string conforms to a regular expression pattern.

- **Behavior**:
  - Compiles the regex pattern and validates the input string.
  - Fails if the regex compilation or match search fails.

---

### `unittest_assert_regex_group(pattern, input, group_index, expected, flags, message_format, ...)`

**Purpose**: Asserts that a specific captured group from a regex match equals an expected string.

**Parameters**:
- `pattern`: The regex pattern to match.
- `input`: The input string to validate.
- `group_index`: The index of the group to validate.
- `expected`: The expected string value of the group.
- `flags`: Optional regex flags.
- `message_format`: A printf-style format string describing the failure.
- `...`: Additional arguments for the format string.

**Return Value**: None.

**Usage Case**: Use to assert the value of a specific regex capture group without manually extracting it.

- **Behavior**:
  - Validates if the regex compiles and matches the input.
  - Fails if the group index is invalid or the captured group does not match the expected string.

---

### `unittest_assert_int_eq(expected, actual, message_format, ...)`

**Purpose**: Asserts that two integers are equal. Logs a formatted failure message if the values differ.

**Parameters**:
- `expected`: Expected integer value.
- `actual`: Actual integer value.
- `message_format`: A format string for the failure message.
- `...`: Additional arguments for the format string.

**Return Value**: None.

**Usage Case**: Use to compare two integer values and report a descriptive mismatch message on failure.

---

### `unittest_assert_double_eq(expected, actual, message_format, ...)`

**Purpose**: Asserts that two double-precision floating-point numbers are equal within a specified epsilon. Logs a formatted failure message if the values differ.

**Parameters**:
- `expected`: Expected double-precision floating-point value.
- `actual`: Actual double-precision floating-point value.
- `message_format`: A format string for the failure message.
- `...`: Additional arguments for the format string.

**Return Value**: None.

**Usage Case**: Use to compare floating-point values with tolerance-based equality checking.

---

### `unittest_assert_string_eq(expected, actual, message_format, ...)`

**Purpose**: Asserts that two strings are equal. Logs a formatted failure message if the strings differ. Handles `NULL` pointers gracefully by treating them as `<null>`.

**Parameters**:
- `expected`: Expected string value.
- `actual`: Actual string value.
- `message_format`: A format string for the failure message.
- `...`: Additional arguments for the format string.

**Return Value**: None.

**Usage Case**: Use to compare C strings and get a clear failure message showing both the expected and actual values.

---

### `unittest_assert_not_null(pointer, message_format, ...)`

**Purpose**: Asserts that a pointer is not `NULL`. Logs a formatted failure message if the pointer is `NULL`.

**Parameters**:
- `pointer`: The pointer to check.
- `message_format`: A format string for the failure message.
- `...`: Additional arguments for the format string.

**Return Value**: None.

**Usage Case**: Use after allocation or resource-acquisition calls to ensure the result is a valid non-NULL pointer.

----

### `unittest_assert_in_range(value, min, max, message_format, ...)`

**Purpose**: This macro checks if a given value falls within a specified inclusive range (min to max). If the value is outside the range, it logs an error and fails the test.

**Parameters**:
- `value`: The value to check.
- `min`: The minimum acceptable value.
- `max`: The maximum acceptable value.
- `message_format`: Custom error message format string.
- `...`: Additional arguments for the error message.

**Return Value**: None.

**Usage Case**: Use to validate that a numeric value lies within expected bounds.

---

### **Memory Leak Detection**
The framework now includes tools for memory tracking:
- Allocate memory using `UNITTEST_MALLOC`.
- Free memory using `UNITTEST_FREE`.
- Check for leaks in any block of code with `UNITTEST_CHECK_NO_LEAKS`.

### **Memory Management**
### `void* UNITTEST_MALLOC(size_t size)`

**Purpose**: Allocates memory and tracks its usage.

**Parameters**:
- `size`: Size in bytes to allocate.

**Return Value**: Pointer to the allocated memory.

**Usage Case**: Use instead of `malloc` inside tests so the framework can detect and report memory leaks.

### `void UNITTEST_FREE(void* ptr)`

**Purpose**: Frees memory and updates the tracking system.

**Parameters**:
- `ptr`: Pointer to the memory to free.

**Return Value**: None.

**Usage Case**: Use instead of `free` to release memory allocated with `UNITTEST_MALLOC` and keep tracking accurate.

### `size_t unittest_get_memory_usage(void)`

**Purpose**: Returns the total allocated memory currently in use.

**Parameters**: None.

**Return Value**: Total memory in bytes.

**Usage Case**: Use to query how much memory is currently tracked as live by the framework.

---

### **Timing Functions**
### `double unittest_timer_real(void)`

**Purpose**: Measures wall-clock time.

**Parameters**: None.

**Return Value**: Time in seconds as a `double`.

**Usage Case**: Use to record start/end timestamps for measuring elapsed real time in a test.

### `double unittest_timer_cpu(void)`

**Purpose**: Measures CPU time.

**Parameters**: None.

**Return Value**: Time in seconds as a `double`.

**Usage Case**: Use to measure CPU processing time consumed by a test independent of wall-clock time.

### `void unittest_set_timeout(double timeout_ms)`

**Purpose**: Sets a timeout in milliseconds for test execution.

**Parameters**:
- `timeout_ms`: Timeout value in milliseconds.

**Return Value**: None.

**Usage Case**: Call in `main()` before running suites to enforce a maximum execution time per test.

---

### **Array Comparsion Assertion**

### `unittest_assert_array_int_eq(expected, actual, size, message_format, ...)`

**Purpose**: Validates that two integer arrays are equal, comparing element by element.

**Parameters**:
- `expected`: The expected integer array.
- `actual`: The actual integer array to compare.
- `size`: The number of elements in the arrays.
- `message_format`: A format string for the failure message.
- `...`: Additional arguments for the format string.

**Return Value**: None.

**Usage Case**: Use to assert equality between two integer arrays with per-element mismatch reporting.

- **Behavior**:
  - Iterates through both arrays, checking each corresponding element for equality.
  - Logs a detailed failure message if a mismatch is found.

---

### `unittest_assert_array_float_eq(expected, actual, size, epsilon, message_format, ...)`

**Purpose**: Validates that two float arrays are equal, comparing element by element with a specified tolerance (`epsilon`).

**Parameters**:
- `expected`: The expected float array.
- `actual`: The actual float array to compare.
- `size`: The number of elements in the arrays.
- `epsilon`: The maximum allowable difference between elements for them to be considered equal.
- `message_format`: A format string for the failure message.
- `...`: Additional arguments for the format string.

**Return Value**: None.

**Usage Case**: Use to compare float arrays where exact equality is not expected due to floating-point precision.

- **Behavior**:
  - Checks the absolute difference between each pair of elements against the specified `epsilon`.
  - Logs a detailed failure message if any pair of elements differ by more than `epsilon`.

---

### `unittest_assert_array_double_eq(expected, actual, size, epsilon, message_format, ...)`

**Purpose**: Validates that two double-precision arrays are equal, comparing element by element with a specified tolerance (`epsilon`).

**Parameters**:
- `expected`: The expected double array.
- `actual`: The actual double array to compare.
- `size`: The number of elements in the arrays.
- `epsilon`: The maximum allowable difference between elements for them to be considered equal.
- `message_format`: A format string for the failure message.
- `...`: Additional arguments for the format string.

**Return Value**: None.

**Usage Case**: Use to compare double-precision arrays with tolerance-based per-element equality.

- **Behavior**:
  - Compares each pair of elements using the absolute difference, ensuring it is within `epsilon`.
  - Logs a detailed failure message if any pair of elements differs by more than `epsilon`.

---

### `unittest_assert_array_struct_eq(expected, actual, size, comparator, message_format, ...)`

**Purpose**: Validates that two arrays of structs are equal, using a user-defined comparator function.

**Parameters**:
- `expected`: The expected struct array.
- `actual`: The actual struct array to compare.
- `size`: The number of elements in the arrays.
- `comparator`: A function pointer for comparing two structs. Returns `1` if the structs are equal, and `0` otherwise.
- `message_format`: A format string for the failure message.
- `...`: Additional arguments for the format string.

**Return Value**: None.

**Usage Case**: Use when you need to compare arrays of custom struct types with a caller-supplied equality function.

- **Behavior**:
  - Calls the provided `comparator` function for each pair of corresponding elements.
  - Logs a detailed failure message if any comparison returns `0` (not equal).

---

### **Files Assertions** 

### `unittest_assert_file_eq(file1, file2, message_format, ...)`

**Purpose**: Asserts that the contents of two files are identical.

**Parameters**:
- `file1`, `file2`: Paths to the two files to compare.
- `message_format`: A custom failure message format string.
- `...`: Additional arguments to include in the formatted message.

**Return Value**: None.

**Usage Case**: Use to verify that a generated or modified file matches a reference file byte-for-byte.

- **Details**:
  - Reads the files in chunks and compares their contents.
  - Fails if the files have differing lengths or mismatched content.
- **Example**:

---

### `unittest_assert_file_exists(filepath, message_format, ...)`

**Purpose**: Asserts that a file exists at the specified path.

**Parameters**:
- `filepath`: Path to the file.
- `message_format`: A custom failure message format string.
- `...`: Additional arguments to include in the formatted message.

**Return Value**: None.

**Usage Case**: Use to verify that a required file was created or is present before a test proceeds.

- **Details**:
  - Uses platform-specific functionality (`access` or `_access`) to check the existence of the file.
  - Fails if the file does not exist.

---

### `unittest_assert_file_not_exists(filepath, message_format, ...)`

**Purpose**: Asserts that a file does not exist at the specified path.

**Parameters**:
- `filepath`: Path to the file.
- `message_format`: A custom failure message format string.
- `...`: Additional arguments to include in the formatted message.

**Return Value**: None.

**Usage Case**: Use to confirm that a file was deleted or was never created, as part of a cleanup or negative test.

- **Details**:
  - Uses platform-specific functionality (`access` or `_access`) to check the non-existence of the file.
  - Fails if the file exists.

---

### **Mocking**

### `unittest_register_mock_call(function_name, arg_count, args)`

**Purpose**: Registers a function call with the mock system.

**Parameters**:
- `function_name`: The name of the mocked function being called.
- `arg_count`: The number of arguments passed to the function.
- `args`: Array of pointers to the arguments passed to the function.

**Return Value**: None.

**Usage Case**: Call inside a mock function implementation to record each invocation so it can later be verified with `unittest_verify_mock_call`.

- **Details**:
  - Records each call to a mocked function, including the function name, arguments, and call count.
  - Stores call details in an internal registry to allow verification of function behavior in tests.
- **Behavior**:
  - Incrementally tracks calls for each mocked function.
  - Supports multiple calls to the same function and keeps them indexed.

---

### `unittest_get_mock_call(function_name)`

**Purpose**: Retrieves the most recent call to a mocked function.

**Parameters**:
- `function_name`: The name of the mocked function to retrieve.

**Return Value**: Pointer to the most recent `MockCall` record, or `NULL` if the function was never called.

**Usage Case**: Use to inspect the last recorded invocation of a mock function without index-based lookup.

- **Details**:
  - Searches the internal registry for the specified function and returns the most recent call details.
  - If no calls are found, returns `NULL`.
- **Example**:
  ```c
  MockCall* call = unittest_get_mock_call("api_request");
  if (call) {
      printf("Last call to 'api_request': %d arguments\n", call->arg_count);
  }
  ```

---

### `unittest_verify_mock_call(function_name, arg_count, expected_args, call_index)`

**Purpose**: Verifies that a specific call to a mocked function matches the expected arguments.

**Parameters**:
- `function_name`: The name of the mocked function.
- `arg_count`: The number of expected arguments.
- `expected_args`: Array of expected argument values.
- `call_index`: The index of the call to verify.

**Return Value**: None.

**Usage Case**: Use to assert that a mock function was called with specific arguments at a particular invocation index.

- **Behavior**:
  - Ensures that the specified call to the function matches the expected argument values.
  - Fails the test if the function was not called, the argument count is mismatched, or any argument value does not match.
- **Example**:
  ```c
  void* expected_args[] = { (void*)"GET /api/resource" };
  unittest_verify_mock_call("api_request", 1, expected_args, 0);
  ```

---

### `unittest_verify_mock_call_count(function_name, expected_count)`

**Purpose**: Verifies the total number of calls to a mocked function.

**Parameters**:
- `function_name`: The name of the mocked function.
- `expected_count`: The expected number of times the function should have been called.

**Return Value**: None.

**Usage Case**: Use to assert that a mock function was invoked exactly the expected number of times.

- **Behavior**:
  - Compares the actual call count of the function with the expected count.
  - Fails the test if the counts do not match.
- **Example**:
  ```c
  unittest_verify_mock_call_count("api_request", 2);
  ```

---

### `unittest_get_mock_call_by_index(function_name, call_index)`

**Purpose**: Retrieves details of a specific call to a mocked function by its index.

**Parameters**:
- `function_name`: The name of the mocked function.
- `call_index`: The index of the desired call.

**Return Value**: Pointer to the `MockCall` record at the given index, or `NULL` if not found.

**Usage Case**: Use to inspect the arguments of any specific invocation of a mock function by its zero-based call index.

- **Details**:
  - Searches the call registry for the specified function and retrieves the call at the given index.
  - Returns `NULL` if the function was not called the specified number of times.
- **Example**:
  ```c
  MockCall* call = unittest_get_mock_call_by_index("api_request", 1);
  if (call) {
      printf("Second call to 'api_request': %d arguments\n", call->arg_count);
  }
  ```

---

### **Stubbing**

### `unittest_register_stub(function_name, query, return_value)`

**Purpose**: Registers a stubbed return value for a mocked function.

**Parameters**:
- `function_name`: The name of the mocked function.
- `query`: The argument value for which the return value is stubbed.
- `return_value`: The value to return when the function is called with the specified argument.

**Return Value**: None.

**Usage Case**: Use to pre-configure a mock function to return a specific value when called with a particular argument.

- **Behavior**:
  - Associates a specific argument value with a return value.
  - Allows controlled responses from mocked functions during tests.
- **Example**:
  ```c
  unittest_register_stub("api_request", "GET /api/resource", (void*)"Resource data");
  ```

---

### `unittest_get_stub_return_value(function_name, query)`

**Purpose**: Retrieves the stubbed return value for a mocked function.

**Parameters**:
- `function_name`: The name of the mocked function.
- `query`: The argument value to match against stubbed return values.

**Return Value**: The stubbed `void*` value associated with the query, or `NULL` if not found.

**Usage Case**: Call inside a mock function implementation to look up and return the pre-registered stub value for the given argument.

- **Details**:
  - Searches for a matching stub based on the function name and argument value.
  - Returns the stubbed value if found, or `NULL` otherwise.
- **Example**:
  ```c
  const char* response = (const char*)unittest_get_stub_return_value("api_request", "GET /api/resource");
  ```

---

### **Macros for Mock Functions**

### `UNITTEST_MOCK_FUNCTION(func_name, ...)`

**Purpose**: Declares a mock function without specifying a return type.

**Parameters**:
- `func_name`: The name of the mock function.
- `...`: The parameter types of the function.

**Return Value**: N/A (code-generation macro).

**Usage Case**: Use to quickly declare a mock function stub when no return type is needed.

- **Details**:
  - Simplifies the declaration of mock functions.
- **Example**:
  ```c
  UNITTEST_MOCK_FUNCTION(my_mock_function, int, const char*);
  ```

---

### `UNITTEST_MOCK_FUNCTION_WITH_STUB(func_name, return_type, ...)`

**Purpose**: Declares a mock function with a return type and arguments.

**Parameters**:
- `func_name`: The name of the mock function.
- `return_type`: The return type of the function.
- `...`: The parameter types of the function.

**Return Value**: N/A (code-generation macro).

**Usage Case**: Use to declare a typed mock function that integrates with the stub registry for controlled return values.

- **Details**:
  - Automatically integrates with stubbing features.
- **Example**:
  ```c
  UNITTEST_MOCK_FUNCTION_WITH_STUB(my_mock_function, int, const char*);
  ```

---

### `UNITTEST_VERIFY_CALL_COUNT(func, count)`

**Purpose**: A macro for verifying the call count of a mocked function.

**Parameters**:
- `func`: The name of the mocked function.
- `count`: The expected call count.

**Return Value**: None.

**Usage Case**: Use as a concise shorthand for `unittest_verify_mock_call_count` inside a test case.

- **Details**:
  - A shorthand for `unittest_verify_mock_call_count`.
- **Example**:
  ```c
  UNITTEST_VERIFY_CALL_COUNT(my_mock_function, 3);
  ```

---

### `UNITTEST_VERIFY_ARGUMENTS(func, ...)`

**Purpose**: Verifies the arguments of a mocked function call.

**Parameters**:
- `func`: The name of the mocked function.
- `...`: The expected argument values.

**Return Value**: None.

**Usage Case**: Use as a concise shorthand to verify the arguments passed to a mock function without manually building an argument array.

- **Details**:
  - Packs the arguments into an array and verifies them using `unittest_verify_mock_call`.
- **Example**:
  ```c
  UNITTEST_VERIFY_ARGUMENTS(api_request, "GET /api/resource");
  ```

---

### **Random Integer Generator**

### `unittest_generate_random_int(int min, int max)`

**Purpose**: Generates a random integer between the specified `min` and `max` values, inclusive.

**Parameters**:
- `min`: The minimum value of the generated random integer.
- `max`: The maximum value of the generated random integer.

**Return Value**: A random `int` in the range `[min, max]`.

**Usage Case**: Use to generate random integer inputs for stress or fuzz testing numeric code.

- **Details**:
  - Uses the `rand()` function from the C standard library.
  - Ensures the generated value is within the specified range.

---

### **Random Float Generator**

### `unittest_generate_random_float(float min, float max)`

**Purpose**: Generates a random float between the specified `min` and `max` values.

**Parameters**:
- `min`: The minimum value of the generated random float.
- `max`: The maximum value of the generated random float.

**Return Value**: A random `float` in the range `[min, max]`.

**Usage Case**: Use to generate random single-precision float inputs for testing floating-point arithmetic or boundary conditions.

- **Details**:
  - Uses `rand()` and scales the result to the desired range.
  - Provides precision limited by the `float` data type.

---

### **Random Double Generator**

### `unittest_generate_random_double(double min, double max)`

**Purpose**: Generates a random double between the specified `min` and `max` values.

**Parameters**:
- `min`: The minimum value of the generated random double.
- `max`: The maximum value of the generated random double.

**Return Value**: A random `double` in the range `[min, max]`.

**Usage Case**: Use to generate random double-precision float inputs for testing high-precision computations.

- **Details**:
  - Uses `rand()` and scales the result to the desired range.
  - Provides precision limited by the `double` data type.

---

### **Random String Generator**

### `unittest_generate_random_string(size_t length)`

**Purpose**: Generates a random alphanumeric string of the specified length.

**Parameters**:
- `length`: The length of the string to generate (excluding the null terminator).

**Return Value**: Pointer to a newly allocated null-terminated string. Caller must free with `unittest_free_generated_data`.

**Usage Case**: Use to generate random string inputs for testing string-processing functions.

- **Details**:
  - Uses a predefined character set of alphanumeric characters.
  - Allocates memory for the string, including the null terminator.
  - Caller is responsible for freeing the generated string using `free()`.

---

### **Edge Case Integer Generator**

### `unittest_generate_edge_case_integers(size_t* size)`

**Purpose**: Provides an array of predefined edge case integers for testing.

**Parameters**:
- `size`: Pointer to a variable to store the number of generated edge cases.

**Return Value**: Pointer to a static array of edge case `int` values.

**Usage Case**: Use to iterate over boundary integer values (e.g., `INT_MIN`, `INT_MAX`) when testing numeric functions.

- **Details**:
  - Includes values like `INT_MIN`, `INT_MAX`, `-1`, `0`, and `1`.
  - The returned array is static and does not require manual memory management.

---

### **Edge Case Float Generator**

### `unittest_generate_edge_case_floats(size_t* size)`

**Purpose**: Provides an array of predefined edge case floats for testing.

**Parameters**:
- `size`: Pointer to a variable to store the number of generated edge cases.

**Return Value**: Pointer to a static array of edge case `float` values.

**Usage Case**: Use to iterate over boundary float values (e.g., `-FLT_MAX`, `FLT_MAX`) when testing float-handling code.

- **Details**:
  - Includes values like `-FLT_MAX`, `FLT_MAX`, `-1.0f`, `0.0f`, and `1.0f`.
  - The returned array is static and does not require manual memory management.

---

### **Edge Case Double Generator**

### `unittest_generate_edge_case_doubles(size_t* size)`

**Purpose**: Provides an array of predefined edge case doubles for testing.

**Parameters**:
- `size`: Pointer to a variable to store the number of generated edge cases.

**Return Value**: Pointer to a static array of edge case `double` values.

**Usage Case**: Use to iterate over boundary double values (e.g., `-DBL_MAX`, `DBL_MAX`) when testing double-handling code.

- **Details**:
  - Includes values like `-DBL_MAX`, `DBL_MAX`, `-1.0`, `0.0`, and `1.0`.
  - The returned array is static and does not require manual memory management.

---

### **Edge Case String Generator**

### `unittest_generate_edge_case_strings(size_t* size)`

**Purpose**: Provides an array of predefined edge case strings for testing.

**Parameters**:
- `size`: Pointer to a variable to store the number of generated edge cases.

**Return Value**: Pointer to a static array of edge case `char*` strings.

**Usage Case**: Use to iterate over boundary string values (empty, very long, special characters) when testing string-handling functions.

- **Details**:
  - Includes edge cases like empty strings, very long strings, and strings with special characters (e.g., null, newline, tab).
  - The returned array is static and does not require manual memory management.
---

### **Free Generated Data**

### `unittest_free_generated_data(void* data)`

**Purpose**: Frees memory allocated by `unittest_generate_random_string`.

**Parameters**:
- `data`: Pointer to the dynamically allocated memory.

**Return Value**: None.

**Usage Case**: Use to free strings returned by `unittest_generate_random_string` after the test is done with them.

- **Details**:
  - This function is primarily for use with random strings.
  - Safely frees memory, ensuring no double-free issues.

---

### `void unittest_reset_state(void)`

**Purpose**: Reset every global test-runner counter back to its initial state. Useful for running multiple independent test passes in the same process (benchmarks, parameterized fixtures).

**Parameters**: None.

**Return Value**: None.

**Usage Case**: Call between independent test runs in the same process to start each pass with a clean counter state.

- **Resets**: `unittest_run`, `unittest_assert`, `unittest_fail`,
  `unittest_status`, `unittest_skipped`, `unittest_real_timer`,
  `unittest_proc_timer`, `unittest_last_message`, `unittest_retry_count`,
  `unittest_timeout_ms`, `unittest_param`.
- **Does NOT touch**: mock / stub registries or the memory-tracking
  list — use `unittest_clear_mock_registry`,
  `unittest_clear_stub_registry`, and the existing `unittest_free` for those.

**Example**
```c
#include "unittest/unittest.h"
#include <stdio.h>

UNITTEST_CASE(trivial) {
    unittest_check(1 == 1);
}

UNITTEST_SUITE(suite) {
    UNITTEST_RUN_TEST(trivial, 1);
}

int main(void) {
    for (int trial = 0; trial < 3; ++trial) {
        unittest_reset_state();
        UNITTEST_RUN_SUITE(suite);
        printf("trial %d: %d test(s), %d failure(s)\n",
               trial, unittest_run, unittest_fail);
    }
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```

---

### `void unittest_clear_mock_registry(void)`

**Purpose**: Walk the mock-call registry, freeing each `MockCall`'s strdup'd `function_name`, each strdup'd entry in `actual_args`, and the node itself. After this call the registry is empty and `unittest_get_mock_call` / `_by_index` return NULL.

**Parameters**: None.

**Return Value**: None.

**Usage Case**: Call between test groups or right before `UNITTEST_REPORT()` to free all captured mock data and prevent leaks.

- **Why it matters**: Previously the lib never freed these allocations,
  so every captured mock argument leaked for the lifetime of the
  process.
- **Call**: Between test groups, or right before `UNITTEST_REPORT()` in
  `main()`.

**Example**
```c
#include "unittest/unittest.h"

UNITTEST_CASE(mock_test) {
    void* args[] = { "alice", "42" };
    unittest_register_mock_call("greet", 2, args);
    UNITTEST_VERIFY_CALL_COUNT(greet, 1);
}

UNITTEST_SUITE(suite) {
    UNITTEST_RUN_TEST(mock_test, 1);
}

int main(void) {
    UNITTEST_RUN_SUITE(suite);
    unittest_clear_mock_registry();   /* free captured mock data */
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```

---

### `void unittest_clear_stub_registry(void)`

**Purpose**: Reset `stub_count` to 0 and zero every entry's three pointer fields. `unittest_register_stub` stores raw pointers (it does NOT own the strings), so no `free` is required — clearing the index is enough to invalidate every subsequent lookup.

**Parameters**: None.

**Return Value**: None.

**Usage Case**: Call between test groups to invalidate all registered stubs so subsequent lookups return `NULL`.

**Example**
```c
#include "unittest/unittest.h"
#include <stdio.h>

int main(void) {
    int v1 = 42;
    unittest_register_stub("fetch", "key1", &v1);

    int* hit = (int*)unittest_get_stub_return_value("fetch", "key1");
    printf("before clear: %d\n", *hit);   /* 42 */

    unittest_clear_stub_registry();

    void* miss = unittest_get_stub_return_value("fetch", "key1");
    printf("after clear:  %p\n", miss);   /* (nil) */
    return 0;
}
```

---

### `void unittest_generate_random_bytes(unsigned char* buffer, size_t size)`

**Purpose**: Fill `buffer` with `size` random bytes (`0..255`) via `rand() & 0xFF`. NULL buffer or zero size is a safe no-op. Useful for fuzzing binary parsers, generating randomized payloads for hashing / compression / network code.

**Parameters**:
- `buffer` — destination. NULL is rejected.
- `size` — number of bytes to fill.

**Return Value**: None.

**Usage Case**: Use to generate random binary payloads for fuzz-testing parsers, hash functions, or network protocol handlers.

- **Determinism**: Seed `rand()` first with `srand()` if you want
  reproducible runs.

**Example**
```c
#include "unittest/unittest.h"
#include <stdio.h>
#include <stdlib.h>

UNITTEST_CASE(crc_fuzz) {
    unsigned char payload[256];
    srand(12345);                                  /* deterministic */
    unittest_generate_random_bytes(payload, sizeof(payload));
    /* feed `payload` into your CRC32 / parser / decoder ... */
    unittest_check(payload[0] != 0xAA);
}

UNITTEST_SUITE(suite) {
    UNITTEST_RUN_TEST(crc_fuzz, 1);
}

int main(void) {
    UNITTEST_RUN_SUITE(suite);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```

---

### `const char* unittest_mock_call_arg(const char* function_name, int call_index, size_t arg_index)`

**Purpose**: Programmatic getter for an argument that a mock function was called with. Pairs with `unittest_register_mock_call`. Unlike `unittest_verify_mock_call`, this does NOT fail the test on a miss — it returns NULL so test logic can branch on the captured value.

**Parameters**:
- `function_name` — mock function to look up. NULL → NULL.
- `call_index` — zero-based call index (first call = 0).
- `arg_index` — zero-based argument index, must be `< MAX_ARGS` and `< the call's recorded arg_count`.

**Return Value**: Borrowed pointer to the captured argument string. Stays valid until `unittest_clear_mock_registry` is called. NULL on any lookup failure (unknown function, OOR call_index, OOR arg_index, NULL name).

**Usage Case**: Use when you need to inspect or branch on the actual argument values captured by a mock, without hard-failing the test on a miss.

**Example**
```c
#include "unittest/unittest.h"
#include <stdio.h>
#include <string.h>

UNITTEST_CASE(mock_inspect) {
    void* args[] = { "alice", "42" };
    unittest_register_mock_call("greet", 2, args);

    const char* name = unittest_mock_call_arg("greet", 0, 0);   /* "alice" */
    const char* age  = unittest_mock_call_arg("greet", 0, 1);   /* "42"    */
    const char* nope = unittest_mock_call_arg("greet", 99, 0);  /* NULL    */

    unittest_check(strcmp(name, "alice") == 0);
    unittest_check(strcmp(age,  "42")    == 0);
    unittest_check(nope == NULL);
}

UNITTEST_SUITE(suite) {
    UNITTEST_RUN_TEST(mock_inspect, 1);
}

int main(void) {
    UNITTEST_RUN_SUITE(suite);
    unittest_clear_mock_registry();
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```

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
    UNITTEST_RUN_TEST(test_addition, 1);
}

int main() {
    UNITTEST_RUN_SUITE(math_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result**
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
    UNITTEST_RUN_TEST(test_example, 1);
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
    UNITTEST_RUN_TEST(test_strings, 2);
    UNITTEST_RUN_TEST(test_floats, 2);
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
    unittest_assert((end - start) > 0.0, "Execution time should be %s", "Positive");
}

UNITTEST_SUITE(timing_suite) {
    UNITTEST_RUN_TEST(test_timing, 1);
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
    UNITTEST_RUN_TEST(test_integer_failure, 3);
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
    UNITTEST_RUN_TEST(test_double_failure, 3);
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
    UNITTEST_RUN_TEST(test_string_failure, 3);
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
    unittest_assert(value > 40, "Value should be greater than %d", 40);  
}


UNITTEST_SUITE(custom_failure_tests) {
    UNITTEST_RUN_TEST(test_custom_failure, 1);
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
    UNITTEST_RUN_TEST(test_integer_failure, 3);
    UNITTEST_RUN_TEST(test_double_failure, 3);
    UNITTEST_RUN_TEST(test_string_failure, 3);
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

UNITTEST_CASE(test_regex_basic_match) {
    const char* pattern = "hello";
    const char* test_string = "hello world!";
    
    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    unittest_assert(regex != NULL, "Failed to compile %s", "regex");
    
    RegexMatch match;
    RegexResult result = regex_search(regex, test_string, &match);
    unittest_assert(result == REGEX_SUCCESS, "Expected a successful %s", "match");

    char matched_string[match.length + 1]; 
    strncpy(matched_string, match.start, match.length);
    matched_string[match.length] = '\0';  
    
    unittest_assert_string_eq("hello", matched_string, "Expected '%s' but got '%s'", "hello", matched_string);
    
    regex_deallocate(regex);
}


UNITTEST_SUITE(regex_tests) {
    UNITTEST_RUN_TEST(test_regex_basic_match, 1);
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
#include "unittest/unittest.h"
#include "random/random.h"

UNITTEST_CASE(test_random_integer_generation) {
    int min = 1, max = 10;

    random_seed(42);  
    for (int i = 0; i < 5; ++i) {
        int random_number = random_randint(min, max);
        unittest_assert(random_number >= min && random_number <= max, "Generated number is out of range (%d, %d)", min, max);
    }
}

UNITTEST_SUITE(random_tests) {
    UNITTEST_RUN_TEST(test_random_integer_generation, 1);
}


int main() {
    UNITTEST_RUN_SUITE(random_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result**
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
    /* Sample variance = 32/7 ≈ 4.571428... Use exact fraction, not a
       rounded literal, to avoid epsilon-1e-12 mismatch. */
    double expected_variance = 32.0 / 7.0;
    unittest_assert_double_eq(expected_variance, variance, "Expected variance %.6f but got %.6f", expected_variance, variance);
}


UNITTEST_SUITE(statistics_tests) {
    UNITTEST_RUN_TEST(test_mean_calculation, 3);
    UNITTEST_RUN_TEST(test_median_calculation, 3);
    UNITTEST_RUN_TEST(test_variance_calculation, 3);
}


int main() {
    UNITTEST_RUN_SUITE(statistics_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
.Elapsed time for test_mean_calculation: 0.01 ms
Test passed: test_mean_calculation
.Elapsed time for test_median_calculation: 0.01 ms
Test passed: test_median_calculation
.Elapsed time for test_variance_calculation: 0.01 ms
Test passed: test_variance_calculation


3 tests, 3 assertions, 0 failures, 0 skipped

Finished in 0.00100000 seconds (real) 0.00000000 seconds (proc)
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
    UNITTEST_RUN_TEST(test_feature, 2);
    UNITTEST_RUN_TEST(test_always_run, 2);
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
    unittest_assert(1 == 1, "This should pass %d == %d", 1, 1);
}


UNITTEST_CASE(test_fast) {
    int result = 2 + 2;
    unittest_assert_int_eq(4, result, "Expected 4 and result is %d", result);
}


UNITTEST_SUITE(example_tests) {
    UNITTEST_RUN_TEST(test_long_running, 2);
    UNITTEST_RUN_TEST(test_fast, 2);
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
    UNITTEST_RUN_TEST(test_memory_leak, 2);
    UNITTEST_RUN_TEST(test_no_memory_leak, 2);
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
#include "unittest/unittest.h"

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
    UNITTEST_RUN_TEST(test_no_memory_leak, 2);
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
#include "unittest/unittest.h"

UNITTEST_CASE(test_flaky) {
    static int fail_count = 2; 
    if (fail_count > 0) {
        fail_count--;
        unittest_fail("Flaky test failed!");
    }
    unittest_assert(1 == 1, "This should pass after retries %d == %d", 1, 1);
}


UNITTEST_SUITE(flaky_suite) {
    UNITTEST_RETRY_COUNT(3); // 3 retries
    UNITTEST_RUN_TEST(test_flaky, 1);
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
Progress: [==================================================] 3/3 (100.00%)Test passed: test_flaky


3 tests, 3 assertions, 0 failures, 0 skipped

Finished in 0.01456210 seconds (real) 0.00000000 seconds (proc)
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
    UNITTEST_RUN_TEST(test_pointer_not_null, 1);
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
    UNITTEST_RUN_TEST(test_value_in_range, 1);
    UNITTEST_RUN_TEST(test_value_out_of_range, 1);
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
    UNITTEST_RUN_TEST(test_arrays_equal, 1);
    UNITTEST_RUN_TEST(test_arrays_not_equal, 1);
    UNITTEST_RUN_TEST(test_string_arrays_equal, 1);
    UNITTEST_RUN_TEST(test_string_arrays_not_equal, 1);
    UNITTEST_RUN_TEST(test_double_arrays_equal, 1);
    UNITTEST_RUN_TEST(test_double_arrays_not_equal, 1);
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

--- 

### Example 22 : Validate Email Address 
```c
#include "unittest/unittest.h"

UNITTEST_CASE(test_valid_email) {
    const char* pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"; // Regex for email
    const char* email = "example@test.com";

    unittest_assert_regex_match(
        pattern, 
        email, 
        REGEX_DEFAULT, 
        "Expected '%s' to match email regex '%s'", 
        email, 
        pattern
    );
}


UNITTEST_CASE(test_invalid_email) {
    const char* pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"; // Regex for email
    const char* email = "invalid-email";

    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    if (!regex) {
        unittest_fail("Failed to compile regex");
        return;
    }

    RegexMatch match;
    RegexResult result = regex_search(regex, email, &match);
    regex_deallocate(regex);

    unittest_assert(result == REGEX_NO_MATCH, "Expected '%s' to NOT match email regex '%s'", email, pattern);
}


UNITTEST_SUITE(email_tests) {
    UNITTEST_RUN_TEST(test_valid_email, 1);
    UNITTEST_RUN_TEST(test_invalid_email, 1);
}


int main() {
    UNITTEST_RUN_SUITE(email_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
.Elapsed time for test_valid_email: 0.19 ms
.Elapsed time for test_invalid_email: 0.06 ms


2 tests, 1 assertions, 0 failures, 0 skipped

Finished in 0.00502870 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 23 : Validate Date Formate with regex 
```c
#include "unittest/unittest.h"
#include "fmt/fmt.h"

UNITTEST_CASE(test_valid_date) {
    const char* pattern = "(\\d{2})/(\\d{2})/(\\d{4})"; // Regex for date
    const char* date = "21/10/2024";

    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    if (!regex) {
        unittest_fail("Failed to compile regex");
        return;
    }

    RegexMatch match;
    RegexResult result = regex_search(regex, date, &match);

    unittest_assert(result == REGEX_SUCCESS, "Expected regex to match '%s'", date);

    printf("Debug: Captured Groups:\n");
    for (int i = 0; i < match.group_count; i++) {
        printf("Group %d: %.*s\n", i + 1, (int)match.group_lengths[i], match.group_starts[i]);
    }

    // Validate individual groups
    unittest_assert(match.group_count >= 3, "Expected at least %d groups in the regex match", 3);
    unittest_assert(strncmp(match.group_starts[0], "21", match.group_lengths[0]) == 0, "Expected day to be '21', got '%.*s'", (int)match.group_lengths[0], match.group_starts[0]);
    unittest_assert(strncmp(match.group_starts[1], "10", match.group_lengths[1]) == 0, "Expected month to be '10', got '%.*s'", (int)match.group_lengths[1], match.group_starts[1]);
    unittest_assert(strncmp(match.group_starts[2], "2024", match.group_lengths[2]) == 0, "Expected year to be '2024', got '%.*s'", (int)match.group_lengths[2], match.group_starts[2]);

    regex_match_free(&match);
    regex_deallocate(regex);
}


UNITTEST_CASE(test_invalid_date) {
    const char* pattern = "(\\d{2})/(\\d{2})/(\\d{4})"; // Regex for date
    const char* date = "invalid-date";
    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);

    if (!regex) {
        unittest_fail("Failed to compile regex");
        return;
    }

    RegexMatch match;
    RegexResult result = regex_search(regex, date, &match);

    unittest_assert(result == REGEX_NO_MATCH, "Expected '%s' to NOT match date regex '%s'", date, pattern);
    regex_match_free(&match);
    regex_deallocate(regex);
}


UNITTEST_SUITE(date_tests) {
    UNITTEST_RUN_TEST(test_valid_date, 1);
    UNITTEST_RUN_TEST(test_invalid_date, 1);
}


int main() {
    UNITTEST_RUN_SUITE(date_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
.Debug: Captured Groups:
Group 1: 21
Group 2: 10
Group 3: 2024
....Elapsed time for test_valid_date: 3.36 ms
.Elapsed time for test_invalid_date: 0.05 ms


2 tests, 6 assertions, 0 failures, 0 skipped

Finished in 0.00696130 seconds (real) 0.00000000 seconds (proc)
```

--- 
### Example 24 : using  `unittest_assert_regex_group`

```c
#include "unittest/unittest.h"



UNITTEST_CASE(test_regex_group_validation) {
    const char* pattern = "(\\d{2})/(\\d{2})/(\\d{4})"; // Regex for date
    const char* date = "21/10/2024";

    // Validate individual groups
    unittest_assert_regex_group(
        pattern, 
        date, 
        0, 
        "21", 
        REGEX_DEFAULT, 
        "Expected group 0 (day) to be '%d', got different value", 21
    );

    unittest_assert_regex_group(
        pattern, 
        date, 
        1, 
        "10", 
        REGEX_DEFAULT, 
        "Expected group 1 (month) to be '%d', got different value", 10
    );

    unittest_assert_regex_group(
        pattern, 
        date, 
        2, 
        "2024", 
        REGEX_DEFAULT, 
        "Expected group 2 (year) to be '%d', got different value", 2024
    );
}


UNITTEST_CASE(test_invalid_group_index) {
    const char* pattern = "(\\d{2})/(\\d{2})/(\\d{4})";
    const char* date = "21/10/2024";

    // Validate an out-of-bounds group index
    unittest_assert_regex_group(
        pattern, 
        date, 
        3, // Invalid index (out of bounds)
        "INVALID", 
        REGEX_DEFAULT, 
        "Expected group %d to throw an out-of-bounds error", 3
    );
}


UNITTEST_SUITE(regex_group_tests) {
    UNITTEST_RUN_TEST(test_regex_group_validation, 1);
    UNITTEST_RUN_TEST(test_invalid_group_index, 1);
}


int main() {
    UNITTEST_RUN_SUITE(regex_group_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}

```
**Result**
```
...Elapsed time for test_regex_group_validation: 0.42 ms
test_invalid_group_index failed:
        main.cpp:41: Group index 3 out of bounds
Elapsed time for test_invalid_group_index: 3.61 ms


2 tests, 0 assertions, 1 failures, 0 skipped

Finished in 0.00845910 seconds (real) 0.00000000 seconds (proc)
```

--- 
### Example 25 : arrays comparsion in different types 

```c
#include "unittest/unittest.h"



typedef struct {
    int id;
    char name[50];
} Person;


int compare_person(const Person* a, const Person* b) {
    return a->id == b->id && strcmp(a->name, b->name) == 0;
}


UNITTEST_CASE(test_integer_array_comparison) {
    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 3, 4, 5};
    size_t size = sizeof(expected) / sizeof(expected[0]);

    unittest_assert_array_int_eq(expected, actual, size, "Integer arrays are not equal size is %zu", size);
}


UNITTEST_CASE(test_float_array_comparison) {
    float expected[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    float actual[] = {1.0, 2.0, 3.0, 4.0001, 5.0};
    size_t size = sizeof(expected) / sizeof(expected[0]);
    float epsilon = 1E-3;

    unittest_assert_array_float_eq(expected, actual, size, epsilon, "Float arrays are not equal size is %zu", size);
}


UNITTEST_CASE(test_double_array_comparison) {
    double expected[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double actual[] = {1.0, 2.0, 3.0000001, 4.0, 5.0};
    size_t size = sizeof(expected) / sizeof(expected[0]);
    double epsilon = 1E-7;

    unittest_assert_array_double_eq(expected, actual, size, epsilon, "Double arrays are not equal size is %zu", size);
}


UNITTEST_CASE(test_struct_array_comparison) {
    Person expected[] = {
        {1, "Alice"},
        {2, "Bob"},
        {3, "Charlie"}
    };

    Person actual[] = {
        {1, "Alice"},
        {2, "Bob"},
        {3, "Charlie"}
    };

    size_t size = sizeof(expected) / sizeof(expected[0]);

    unittest_assert_array_struct_eq(expected, actual, size, compare_person, "Struct arrays are not equal size is %zu", size);
}


UNITTEST_SUITE(array_tests) {
    UNITTEST_RUN_TEST(test_integer_array_comparison, 1);
    UNITTEST_RUN_TEST(test_float_array_comparison, 1);
    UNITTEST_RUN_TEST(test_double_array_comparison, 1);
    UNITTEST_RUN_TEST(test_struct_array_comparison, 1);
}


int main() {
    UNITTEST_RUN_SUITE(array_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}

```
**Result**
```
.Elapsed time for test_integer_array_comparison: 0.20 ms
.Elapsed time for test_float_array_comparison: 0.07 ms
.Elapsed time for test_double_array_comparison: 0.04 ms
.Elapsed time for test_struct_array_comparison: 0.04 ms


4 tests, 4 assertions, 0 failures, 0 skipped

Finished in 0.00949780 seconds (real) 0.00000000 seconds (proc)
```

--- 
### Example 26 : Files Assertions 

```c
#include "unittest/unittest.h"

UNITTEST_CASE(test_file_comparison) {
    const char* file1 = "test1.txt";
    const char* file2 = "test2.txt";

    FILE* f1 = fopen(file1, "wb");
    FILE* f2 = fopen(file2, "wb");
    if (!f1 || !f2) {
        unittest_fail("Failed to create test files.");
    }

    const char* content = "Hello, World!\nThis is a test.\n";
    fwrite(content, 1, strlen(content), f1);
    fwrite(content, 1, strlen(content), f2);

    fclose(f1);
    fclose(f2);

    unittest_assert_file_eq(file1, file2, "Files '%s' and '%s' should be identical", file1, file2);
}


UNITTEST_CASE(test_file_existence) {
    const char* existing_file = "test1.txt";
    const char* non_existing_file = "nonexistent.txt";

    unittest_assert_file_exists(existing_file, "File '%s' should exist", existing_file);
    unittest_assert_file_not_exists(non_existing_file, "File '%s' should not exist", non_existing_file);
}


UNITTEST_SUITE(file_tests) {
    UNITTEST_RUN_TEST(test_file_comparison, 1);
    UNITTEST_RUN_TEST(test_file_existence, 1);
}


int main() {
    UNITTEST_RUN_SUITE(file_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
Debug: Written to file1: 30 bytes, to file2: 30 bytes
.Elapsed time for test_file_comparison: 4.14 ms
..Elapsed time for test_file_existence: 0.17 ms

Finished in 0.00910570 seconds (real) 0.00000000 seconds (proc)
```

---
### Example 27 : Test with Mock Functions 

```c
#include "unittest/unittest.h"

UNITTEST_MOCK_FUNCTION_WITH_STUB(database_query, int, const char*)

int database_query(const char* query) {
    void* args[] = {(void*)query};
    unittest_register_mock_call("database_query", 1, args);

    int stubbed_return = (int)(intptr_t)unittest_get_stub_return_value("database_query", query);
    return stubbed_return;
}


UNITTEST_CASE(test_database_query_mock) {
    // Stub expected return values
    unittest_register_stub("database_query", "SELECT COUNT(*) FROM users", (void*)(intptr_t)10);
    unittest_register_stub("database_query", "DELETE FROM users WHERE id=1", (void*)(intptr_t)0);

    // Call mocked function
    int result1 = database_query("SELECT COUNT(*) FROM users");
    int result2 = database_query("DELETE FROM users WHERE id=1");

    // Verify results
    unittest_assert_int_eq(10, result1, "Expected first result to be %d", 10);
    unittest_assert_int_eq(0, result2, "Expected second result to be %d", 0);

    // Verify calls and arguments
    void* expected_args1[] = { (void*)"SELECT COUNT(*) FROM users" };
    void* expected_args2[] = { (void*)"DELETE FROM users WHERE id=1" };

    unittest_verify_mock_call("database_query", 1, expected_args1, 0); // First call
    unittest_verify_mock_call("database_query", 1, expected_args2, 1); // Second call

    UNITTEST_VERIFY_CALL_COUNT(database_query, 2);
}


UNITTEST_SUITE(database_tests) {
    UNITTEST_RUN_TEST(test_database_query_mock, 1);
}


int main() {
    UNITTEST_RUN_SUITE(database_tests);
    UNITTEST_REPORT();

    return UNITTEST_EXIT_CODE;
}

```
**Result**
```
..Debug: Found call 'database_query' at index 0
..Debug: Found call 'database_query' at index 1
...Elapsed time for test_database_query_mock: 4.83 ms
Progress: [==================================================] 1/1 (100.00%)Test passed: test_database_query_mock


1 tests, 7 assertions, 0 failures, 0 skipped

Finished in 0.01160650 seconds (real) 0.00000000 seconds (proc)
```

---

### Example 27: Mock Feature with API Response Simulation

```c
#include "unittest/unittest.h"



// Define the mock function for simulating an API request
UNITTEST_MOCK_FUNCTION_WITH_STUB(api_request, const char*, const char*)

const char* api_request(const char* endpoint) {
    void* args[] = {(void*)endpoint};
    unittest_register_mock_call("api_request", 1, args);

    const char* stubbed_return = (const char*)unittest_get_stub_return_value("api_request", endpoint);
    return stubbed_return;
}


UNITTEST_CASE(test_api_request_mock) {
    // Stub expected return values for different endpoints
    unittest_register_stub("api_request", "/user/info", (void*)"User Info: John Doe");
    unittest_register_stub("api_request", "/user/settings", (void*)"Settings: Enabled");

    // Call the mocked function
    const char* response1 = api_request("/user/info");
    const char* response2 = api_request("/user/settings");

    // Verify results
    unittest_assert_string_eq("User Info: John Doe", response1, "Expected API response for '/user/info' to be '%s'", "User Info: John Doe");
    unittest_assert_string_eq("Settings: Enabled", response2, "Expected API response for '/user/settings' to be '%s'", "Settings: Enabled");

    // Verify calls and arguments
    void* expected_args1[] = { (void*)"/user/info" };
    void* expected_args2[] = { (void*)"/user/settings" };

    unittest_verify_mock_call("api_request", 1, expected_args1, 0); // First call
    unittest_verify_mock_call("api_request", 1, expected_args2, 1); // Second call

    UNITTEST_VERIFY_CALL_COUNT(api_request, 2);
}


UNITTEST_SUITE(api_mock_tests) {
    UNITTEST_RUN_TEST(test_api_request_mock, 1);
}


int main() {
    UNITTEST_RUN_SUITE(api_mock_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```

**Result**
```
..Debug: Found call 'api_request' at index 0
..Debug: Found call 'api_request' at index 1
...Elapsed time for test_api_request_mock: 5.13 ms
Progress: [==================================================] 1/1 (100.00%)Test passed: test_api_request_mock


1 tests, 7 assertions, 0 failures, 0 skipped

Finished in 0.01178220 seconds (real) 0.00000000 seconds (proc)

```

---
### Example 28: Mock Feature with Advanced Verification and Setup/Teardown

```c
#include "unittest/unittest.h"

// Mock function simulating database query
UNITTEST_MOCK_FUNCTION_WITH_STUB(database_query, int, const char*)

int database_query(const char* query) {
    void* args[] = {(void*)query};
    unittest_register_mock_call("database_query", 1, args);

    int stubbed_return = (int)(intptr_t)unittest_get_stub_return_value("database_query", query);
    return stubbed_return;
}


// Setup and Teardown functions
void setup_database_mock() {
    unittest_register_stub("database_query", "SELECT COUNT(*) FROM users", (void*)(intptr_t)42);
    unittest_register_stub("database_query", "DELETE FROM users WHERE id=1", (void*)(intptr_t)1);
    unittest_register_stub("database_query", "DROP TABLE users", NULL); // Simulate failure
}


void teardown_database_mock() {
    // Cleanup or reset mock state
    printf("Debug: Teardown completed. All stubs and mocks cleared.\n");
}


// Test cases
UNITTEST_CASE(test_database_query_count) {
    int result = database_query("SELECT COUNT(*) FROM users");
    unittest_assert_int_eq(42, result, "Expected result to be 42 for query '%s'", "SELECT COUNT(*) FROM users");

    void* expected_args[] = { (void*)"SELECT COUNT(*) FROM users" };
    unittest_verify_mock_call("database_query", 1, expected_args, 0);
}


UNITTEST_CASE(test_database_query_delete) {
    int result = database_query("DELETE FROM users WHERE id=1");
    unittest_assert_int_eq(1, result, "Expected result to be 1 for query '%s'", "DELETE FROM users WHERE id=1");

    void* expected_args[] = { (void*)"DELETE FROM users WHERE id=1" };
    unittest_verify_mock_call("database_query", 1, expected_args, 1);
}


UNITTEST_CASE(test_database_query_drop_table) {
    int result = database_query("DROP TABLE users");
    unittest_assert(result == 0, "Expected no return value for query '%s'", "DROP TABLE users");

    void* expected_args[] = { (void*)"DROP TABLE users" };
    unittest_verify_mock_call("database_query", 1, expected_args, 2);
}


// Combine tests into a suite
UNITTEST_SUITE(database_tests) {
    UNITTEST_SUITE_CONFIGURE(setup_database_mock, teardown_database_mock);
    UNITTEST_RUN_TEST(test_database_query_count, 3);
    UNITTEST_RUN_TEST(test_database_query_delete, 3);
    UNITTEST_RUN_TEST(test_database_query_drop_table, 3);
}


int main() {
    UNITTEST_RUN_SUITE(database_tests);
    UNITTEST_REPORT();
    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
.Debug: Found call 'database_query' at index 0
..Elapsed time for test_database_query_count: 2.96 ms
Progress: [================>                                 ] 1/3 (33.33%)Test passed: test_database_query_count
.Debug: Found call 'database_query' at index 1
..Elapsed time for test_database_query_delete: 1.95 ms
Progress: [=================================>                ] 2/3 (66.67%)Test passed: test_database_query_delete
.Debug: Found call 'database_query' at index 2
..Elapsed time for test_database_query_drop_table: 2.40 ms
Progress: [==================================================] 3/3 (100.00%)Test passed: test_database_query_drop_table


3 tests, 9 assertions, 0 failures, 0 skipped

Finished in 0.02833920 seconds (real) 0.00000000 seconds (proc)

```

--- 

### Example 29 : random generators and edge case functions

```c
#include <stdio.h>
#include <time.h>
#include "unittest/unittest.h"


UNITTEST_CASE(test_random_integers) {
    for (int i = 0; i < 5; i++) {
        int value = unittest_generate_random_int(-100, 100);
        unittest_assert(value >= -100 && value <= 100, "Generated integer %d is out of range", value);
    }
}

UNITTEST_CASE(test_random_floats) {
    for (int i = 0; i < 5; i++) {
        float value = unittest_generate_random_float(-10.0f, 10.0f);
        unittest_assert(value >= -10.0f && value <= 10.0f, "Generated float %f is out of range", value);
    }
}


UNITTEST_CASE(test_random_doubles) {
    for (int i = 0; i < 5; i++) {
        double value = unittest_generate_random_double(-1000.0, 1000.0);
        unittest_assert(value >= -1000.0 && value <= 1000.0, "Generated double %lf is out of range", value);
    }
}


UNITTEST_CASE(test_random_strings) {
    for (int i = 0; i < 5; i++) {
        char* random_string = unittest_generate_random_string(10);
        unittest_assert(random_string != NULL, "Random string generation failed");
        unittest_assert(strlen(random_string) == 10, "Generated string length mismatch");
        
        printf("Generated String: %s\n", random_string);
        unittest_free_generated_data(random_string);
    }
}


UNITTEST_CASE(test_edge_case_integers) {
    size_t size;
    int* edge_cases = unittest_generate_edge_case_integers(&size);
    for (size_t i = 0; i < size; i++) {
        printf("Edge Case Integer: %d\n", edge_cases[i]);
        unittest_assert(edge_cases[i] <= INT_MAX && edge_cases[i] >= INT_MIN, "Integer edge case out of range");
    }
}


UNITTEST_CASE(test_edge_case_floats) {
    size_t size;
    float* edge_cases = unittest_generate_edge_case_floats(&size);
    for (size_t i = 0; i < size; i++) {
        printf("Edge Case Float: %f\n", edge_cases[i]);
        unittest_assert(edge_cases[i] <= FLT_MAX && edge_cases[i] >= -FLT_MAX, "Float edge case out of range");
    }
}


UNITTEST_CASE(test_edge_case_doubles) {
    size_t size;
    double* edge_cases = unittest_generate_edge_case_doubles(&size);
    for (size_t i = 0; i < size; i++) {
        printf("Edge Case Double: %lf\n", edge_cases[i]);
        unittest_assert(edge_cases[i] <= DBL_MAX && edge_cases[i] >= -DBL_MAX, "Double edge case out of range");
    }
}


UNITTEST_CASE(test_edge_case_strings) {
    size_t size;
    char** edge_cases = unittest_generate_edge_case_strings(&size);
    for (size_t i = 0; i < size; i++) {
        printf("Edge Case String: '%s'\n", edge_cases[i]);
        unittest_assert(edge_cases[i] != NULL, "String edge case is NULL");
    }
}


UNITTEST_SUITE(random_and_edge_cases_tests) {
    UNITTEST_RUN_TEST(test_random_integers, 8);
    UNITTEST_RUN_TEST(test_random_floats, 8);
    UNITTEST_RUN_TEST(test_random_doubles, 8);
    UNITTEST_RUN_TEST(test_random_strings, 8);
    UNITTEST_RUN_TEST(test_edge_case_integers, 8);
    UNITTEST_RUN_TEST(test_edge_case_floats, 8);
    UNITTEST_RUN_TEST(test_edge_case_doubles, 8);
    UNITTEST_RUN_TEST(test_edge_case_strings, 8);
}


int main() {
    srand((unsigned)time(NULL));

    UNITTEST_RUN_SUITE(random_and_edge_cases_tests);
    UNITTEST_REPORT();

    return UNITTEST_EXIT_CODE;
}
```
**Result**
```
.....Elapsed time for test_random_integers: 0.44 ms
Progress: [======>                                           ] 1/8 (12.50%)Test passed: test_random_integers
.....Elapsed time for test_random_floats: 0.34 ms
Progress: [============>                                     ] 2/8 (25.00%)Test passed: test_random_floats
.....Elapsed time for test_random_doubles: 0.21 ms
Progress: [==================>                               ] 3/8 (37.50%)Test passed: test_random_doubles
..Generated String: oWrS3EXbiX
..Generated String: 1xmS1J4mBZ
..Generated String: bzFOXgdYG9
..Generated String: p6HseDHHFK
..Generated String: qmDmox7sn4
Elapsed time for test_random_strings: 6.43 ms
Progress: [=========================>                        ] 4/8 (50.00%)Test passed: test_random_strings
Edge Case Integer: -2147483648
.Edge Case Integer: -1
.Edge Case Integer: 0
.Edge Case Integer: 1
.Edge Case Integer: 2147483647
.Elapsed time for test_edge_case_integers: 5.27 ms
Progress: [===============================>                  ] 5/8 (62.50%)Test passed: test_edge_case_integers
Edge Case Float: -340282346638528859811704183484516925440.000000
.Edge Case Float: -1.000000
.Edge Case Float: 0.000000
.Edge Case Float: 1.000000
.Edge Case Float: 340282346638528859811704183484516925440.000000
.Elapsed time for test_edge_case_floats: 9.02 ms
Progress: [=====================================>            ] 6/8 (75.00%)Test passed: test_edge_case_floats
Edge Case Double: -179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868504 
55133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000
.Edge Case Double: -1.000000
.Edge Case Double: 0.000000
.Edge Case Double: 1.000000
.Edge Case Double: 179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000
.Elapsed time for test_edge_case_doubles: 30.33 ms
Progress: [===========================================>      ] 7/8 (87.50%)Test passed: test_edge_case_doubles
Edge Case String: ''
.Edge Case String: 'a'
.Edge Case String: 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
.Edge Case String: ''
.Edge Case String: 'newline
'
.Edge Case String: 'tab '
.Elapsed time for test_edge_case_strings: 14.15 ms
Progress: [==================================================] 8/8 (100.00%)Test passed: test_edge_case_strings


8 tests, 46 assertions, 0 failures, 0 skipped

Finished in 0.12817030 seconds (real) 0.00000000 seconds (proc)
```

----

## License

This project is open-source and available under the ISC License.
