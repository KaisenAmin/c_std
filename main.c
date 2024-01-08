#include "algorithm/algorithm.h"
#include "array/array.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int x;
    int y;
} Point;

int compare_points(const void* a, const void* b) {
    Point point1 = *(const Point*)a;
    Point point2 = *(const Point*)b;

    return (point1.x > point2.x) - (point1.x < point2.x);
}

void print_point(void* p) {
    Point* point = (Point*)p;
    printf("(%d, %d) ", point->x, point->y);
}

int main() {
    Point first[] = {{1, 2}, {3, 4}, {5, 6}};
    Point second[] = {{7, 8}, {9, 10}, {11, 12}};
    size_t size_first = sizeof(first) / sizeof(first[0]);
    size_t size_second = sizeof(second) / sizeof(second[0]);
    Array* arr = array_create(sizeof(Point), size_first + size_second);

    algorithm_sort(first, size_first, sizeof(Point), compare_points);
    algorithm_sort(second, size_second, sizeof(Point), compare_points);
    algorithm_merge(first, size_first, second, size_second, sizeof(Point), array_begin(arr), compare_points);

    for (size_t i = 0; i < array_size(arr); i++) {
        print_point(array_at(arr, i));
    }

    array_deallocate(arr);
    return 0;
}
