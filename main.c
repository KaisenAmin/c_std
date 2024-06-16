#include "fmt/fmt.h"
#include "statistics/statistics.h"
#include <math.h>
#include <stdarg.h>
#include <string.h>

int max(int num_args, ...) {
    va_list args;
    int arr[num_args];

    va_start(args, num_args);

    for (int i = 0; i < num_args; i++) {
        int x = va_arg(args, int);
        arr[i] = x;
    }

    int maxi = arr[0];

    for (int i = 1; i < num_args; i++) {
        if (arr[i] > maxi) 
            maxi = arr[i];
    }

    va_end(args);

    return maxi; 
}

void print(char * str, ...) {
    va_list args;
    int num = strlen(str);

    va_start(args, str);

    for (int i = 0; i < num; i++) {
        if (str[i] == 'd') {
            int x = va_arg(args, int);
            printf("%d\n", x);
        }
        else if (str[i] == 'f') {
            double x = va_arg(args, double);
            printf("%f\n", x);
        }
    }
}
int main() {
    
    printf("%d\n", max(3, 10, 8, 13));

    print("%d %d %f %d", 10, 20, 32.32, 25);

    return 0;
}