#ifndef PLOT_H_
#define PLOT_H_

#include <stdio.h>
#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif 

#define PLOT_LOGGING_ENABLE

#ifdef PLOT_LOGGING_ENABLE
    #define PLOT_LOG(fmt, ...) \
        do { fprintf(stderr, "[PLOT LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define PLOT_LOG(fmt, ...) do { } while (0)
#endif

typedef enum {
    PLTYPE_LINE,
    PLTYPE_BAR,
    PLTYPE_SCATTER,
    PLTYPE_PIE,
    PLTYPE_HISTOGRAM,
    PLTYPE_MOVING_AVERAGE,
} PlotType;

typedef struct {
    float* xData;
    float* yData;
    size_t dataSize;
    const char* xLable;
    const char* yLable;
    const char* title;
    int width;
    int height;
    PlotType pltype;
} Plot;


Plot* plot_create(size_t dataSize, const char* title, const char* xLable, const char* yLable);

void plot_destroy(Plot* plot);
void plot_set_data(Plot* plot, const float* xData, const float* yData);
void plot_draw(Plot* plot);
void plot_set_window_size(Plot* plot, int width, int height);
void plot_set_plot_type(Plot* plot, PlotType pltype);

#ifdef __cplusplus 
}
#endif 

#endif 