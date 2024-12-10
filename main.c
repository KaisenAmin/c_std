#include "unittest/unittest.h"
#include "fmt/fmt.h"


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
        
        fmt_printf("Generated String: %s\n", random_string);
        unittest_free_generated_data(random_string);
    }
}

UNITTEST_CASE(test_edge_case_integers) {
    size_t size;
    int* edge_cases = unittest_generate_edge_case_integers(&size);
    for (size_t i = 0; i < size; i++) {
        fmt_printf("Edge Case Integer: %d\n", edge_cases[i]);
        unittest_assert(edge_cases[i] <= INT_MAX && edge_cases[i] >= INT_MIN, "Integer edge case out of range");
    }
}

UNITTEST_CASE(test_edge_case_floats) {
    size_t size;
    float* edge_cases = unittest_generate_edge_case_floats(&size);
    for (size_t i = 0; i < size; i++) {
        fmt_printf("Edge Case Float: %f\n", edge_cases[i]);
        unittest_assert(edge_cases[i] <= FLT_MAX && edge_cases[i] >= -FLT_MAX, "Float edge case out of range");
    }
}

UNITTEST_CASE(test_edge_case_doubles) {
    size_t size;
    double* edge_cases = unittest_generate_edge_case_doubles(&size);
    for (size_t i = 0; i < size; i++) {
        fmt_printf("Edge Case Double: %lf\n", edge_cases[i]);
        unittest_assert(edge_cases[i] <= DBL_MAX && edge_cases[i] >= -DBL_MAX, "Double edge case out of range");
    }
}

UNITTEST_CASE(test_edge_case_strings) {
    size_t size;
    char** edge_cases = unittest_generate_edge_case_strings(&size);
    for (size_t i = 0; i < size; i++) {
        fmt_printf("Edge Case String: '%s'\n", edge_cases[i]);
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