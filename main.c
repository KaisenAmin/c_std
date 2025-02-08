// #include "fmt/fmt.h"
// #include "plot/plot.h"
// #include "bigint/bigint.h"
// #include <time.h>
// #include <stdlib.h>

// int main() {

//     // float x[] = {1.f, 2.f, 3.f, 4.f};  
//     // float y[] = {20.f, 30.f, 15.f, 35.f};  

//     // fmt_printf("Start Plot Program\n");

//     // Plot* plot = plot_create(5, "Pie Chart Example", "", ""); 
//     // if (plot) {
//     //     fmt_printf("Plot Created Successfully\n");
//     //     plot_set_window_size(plot, 1000, 700);
//     //     plot_set_plot_type(plot, PLTYPE_PIE);
//     //     plot_set_data(plot, NULL, y);  // Ensure yData is passed correctly
//     //     plot_draw(plot);
//     // }

//     // plot_destroy(plot);


//     // float x[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
//     // float y[] = {0.1, 0.4, 0.6, 0.3, 0.8, 0.5, 0.7, 0.9, 0.2, 1.0};

//     // fmt_printf("Start Plot Program\n");

//     // Plot* plot = plot_create(10, "Bar Chart Example", "Categories", "Values");
//     // if (plot) {
//     //     fmt_printf("Plot Created Successfully\n");
//     //     plot_set_window_size(plot, 1000, 700);
//     //     plot_set_plot_type(plot, PLTYPE_SCATTER);
//     //     plot_set_data(plot, x, y);
//     //     plot_draw(plot);
//     // }

//     // plot_destroy(plot);

//     // float data[] = {1.2, 3.5, 2.1, 4.8, 3.3, 2.9, 4.1, 5.6, 3.7, 1.8, 
//     //     2.3, 3.9, 4.4, 2.5, 3.1, 5.0, 4.2, 1.5, 3.6, 2.7};

//     // fmt_printf("Start Histogram Plot Program\n");

//     // Plot* plot = plot_create(20, "Histogram Example", "Bins", "Frequency");
//     // if (plot) {
//     //     fmt_printf("Plot Created Successfully\n");
//     //     plot_set_window_size(plot, 1000, 700);
//     //     plot_set_plot_type(plot, PLTYPE_HISTOGRAM);
//     //     plot_set_data(plot, NULL, data);
//     //     plot_draw(plot);
//     // }

//     // plot_destroy(plot);

//     // size_t dataSize = 1000;  // Large dataset
//     // float data[1000];

//     // srand(time(NULL));
//     // for (size_t i = 0; i < dataSize; i++) {
//     //     data[i] = 1.0f + ((float)rand() / RAND_MAX) * 4.0f;  // Values between 1.0 and 5.0
//     // }

//     // fmt_printf("Start Histogram Plot Program (Large Dataset)\n");

//     // Plot* plot = plot_create(dataSize, "Histogram: Large Dataset", "Bins", "Frequency");
//     // if (plot) {
//     //     fmt_printf("Plot Created Successfully\n");
//     //     plot_set_window_size(plot, 1000, 700);
//     //     plot_set_plot_type(plot, PLTYPE_HISTOGRAM);
//     //     plot_set_data(plot, NULL, data);
//     //     plot_draw(plot);
//     // }

//     // plot_destroy(plot);


//     // size_t dataSize = 20;
//     // float data[20];

//     // for (size_t i = 0; i < dataSize; i++) {
//     //     data[i] = 3.0f;  // All values are the same
//     // }

//     // fmt_printf("Start Histogram Plot Program (Uniform Data)\n");

//     // Plot* plot = plot_create(dataSize, "Histogram: Uniform Data", "Bins", "Frequency");
//     // if (plot) {
//     //     fmt_printf("Plot Created Successfully\n");
//     //     plot_set_window_size(plot, 1000, 700);
//     //     plot_set_plot_type(plot, PLTYPE_HISTOGRAM);
//     //     plot_set_data(plot, NULL, data);
//     //     plot_draw(plot);
//     // }

//     // plot_destroy(plot);

//     // size_t dataSize = 15;
//     // float data[15] = {-5.0, -4.2, -3.8, -3.0, -2.5, -1.2, -0.8, 0.0, 0.5, 1.2, 
//     //                   2.5, 3.0, 3.8, 4.5, 5.0};

//     // fmt_printf("Start Histogram Plot Program (Negative Values)\n");

//     // Plot* plot = plot_create(dataSize, "Histogram: Negative Values", "Bins", "Frequency");
//     // if (plot) {
//     //     fmt_printf("Plot Created Successfully\n");
//     //     plot_set_window_size(plot, 1000, 700);
//     //     plot_set_plot_type(plot, PLTYPE_HISTOGRAM);
//     //     plot_set_data(plot, NULL, data);
//     //     plot_draw(plot);
//     // }

//     // plot_destroy(plot);

//     // float x[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//     // float y[] = {10, 15, 14, 20, 18, 22, 21, 25, 24, 30};

//     // fmt_printf("Start Moving Average Plot\n");

//     // Plot* plot = plot_create(10, "Moving Average Example", "Time", "Value");
//     // if (plot) {
//     //     fmt_printf("Plot Created Successfully\n");
//     //     plot_set_window_size(plot, 1000, 700);
//     //     plot_set_plot_type(plot, PLTYPE_MOVING_AVERAGE);
//     //     plot_set_data(plot, x, y);
//     //     plot_draw(plot);
//     // }

//     // plot_destroy(plot);

    
//     return 0;
// }
