/**
 * @author Amin Tahmasebi
 * @class Plot
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in plot.c (file-level convention).
 *
 * Lightweight charting layer on top of raylib. Supports multiple
 * series per Plot (line / bar / scatter / pie / histogram), a legend,
 * grid, configurable colors, and PNG export.
 */

#ifndef PLOT_H_
#define PLOT_H_

#include <stdio.h>
#include <stdbool.h>
#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif


/* #define PLOT_LOGGING_ENABLE */

#ifdef PLOT_LOGGING_ENABLE
    #define PLOT_LOG(fmt, ...) \
        do { fprintf(stderr, "[PLOT LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define PLOT_LOG(fmt, ...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Limits                                                             */
/* ------------------------------------------------------------------ */

#define PLOT_MAX_SERIES 8


/* ------------------------------------------------------------------ */
/* Public types                                                       */
/* ------------------------------------------------------------------ */

typedef enum {
    PLTYPE_LINE,
    PLTYPE_BAR,
    PLTYPE_SCATTER,
    PLTYPE_PIE,
    PLTYPE_HISTOGRAM
} PlotType;


typedef struct {
    unsigned char r, g, b, a;
} PlotColor;


typedef struct {
    float*      xData;     /* may be NULL: implied 0..dataSize-1 */
    float*      yData;
    size_t      dataSize;
    const char* label;     /* heap-owned by the Plot             */
    PlotColor   color;
    PlotType    pltype;
} PlotSeries;


typedef struct {
    PlotSeries  series[PLOT_MAX_SERIES];
    size_t      seriesCount;
    const char* xLabel;    /* heap-owned by the Plot             */
    const char* yLabel;
    const char* title;
    int         width;
    int         height;
    bool        showLegend;
    bool        showGrid;
    PlotColor   bgColor;
    PlotColor   gridColor;
    PlotColor   axisColor;
    PlotColor   legendBgColor;
    PlotColor   legendTextColor;
    int         titleFontSize;   /* pixel size for the title   (default 28) */
    int         labelFontSize;   /* pixel size for axis labels (default 20) */
    /* Manual axis ranges (opaque — use plot_set_xlim/plot_set_ylim).
       When the *_Manual flag is false the axis auto-scales to the data. */
    float       xMinManual, xMaxManual;
    float       yMinManual, yMaxManual;
    bool        xRangeManual;
    bool        yRangeManual;
} Plot;


/* ------------------------------------------------------------------ */
/* Core lifecycle                                                     */
/* ------------------------------------------------------------------ */

Plot*       plot_create                       (const char* title, const char* xLabel, const char* yLabel);
void        plot_destroy                      (Plot* plot);


/* ------------------------------------------------------------------ */
/* Series management                                                  */
/* ------------------------------------------------------------------ */

int         plot_add_series                   (Plot* plot, const float* xData, const float* yData, size_t dataSize, const char* label, PlotType pltype, PlotColor color);
int         plot_add_line                     (Plot* plot, const float* y, size_t n, const char* label, PlotColor color);
int         plot_add_scatter                  (Plot* plot, const float* x, const float* y, size_t n, const char* label, PlotColor color);
int         plot_add_bar                      (Plot* plot, const float* y, size_t n, const char* label, PlotColor color);
bool        plot_update_series_data           (Plot* plot, size_t seriesIndex, const float* xData, const float* yData, size_t dataSize);
bool        plot_remove_series                (Plot* plot, size_t seriesIndex);
void        plot_clear_series                 (Plot* plot);
size_t      plot_get_series_count             (const Plot* plot);
bool        plot_has_series                   (const Plot* plot, size_t index);


/* ------------------------------------------------------------------ */
/* Series accessors / mutators                                        */
/* ------------------------------------------------------------------ */

const char* plot_get_series_label             (const Plot* plot, size_t index);
PlotType    plot_get_series_type              (const Plot* plot, size_t index);
PlotColor   plot_get_series_color             (const Plot* plot, size_t index);
size_t      plot_get_series_size              (const Plot* plot, size_t index);
bool        plot_set_series_label             (Plot* plot, size_t index, const char* label);
bool        plot_set_series_color             (Plot* plot, size_t index, PlotColor color);
bool        plot_set_series_type              (Plot* plot, size_t index, PlotType type);


/* ------------------------------------------------------------------ */
/* Per-series stats                                                   */
/* ------------------------------------------------------------------ */

bool        plot_series_min                   (const Plot* plot, size_t index, float* out);
bool        plot_series_max                   (const Plot* plot, size_t index, float* out);
bool        plot_series_sum                   (const Plot* plot, size_t index, float* out);
bool        plot_series_mean                  (const Plot* plot, size_t index, float* out);


/* ------------------------------------------------------------------ */
/* Style / sizing                                                     */
/* ------------------------------------------------------------------ */

void        plot_set_window_size              (Plot* plot, int width, int height);
bool        plot_set_size                     (Plot* plot, int width, int height);
bool        plot_set_title                    (Plot* plot, const char* title);
bool        plot_set_axis_labels              (Plot* plot, const char* xLabel, const char* yLabel);
void        plot_set_legend                   (Plot* plot, bool show);
void        plot_set_grid                     (Plot* plot, bool show);
void        plot_set_colors                   (Plot* plot, PlotColor bg, PlotColor grid, PlotColor axis, PlotColor legendBg, PlotColor legendText);
void        plot_set_background_color         (Plot* plot, PlotColor c);
void        plot_set_grid_color               (Plot* plot, PlotColor c);
void        plot_set_axis_color               (Plot* plot, PlotColor c);
void        plot_set_legend_colors            (Plot* plot, PlotColor bg, PlotColor text);


/* ------------------------------------------------------------------ */
/* Axis range control                                                 */
/* ------------------------------------------------------------------ */

bool        plot_set_xlim                     (Plot* plot, float min, float max);
bool        plot_set_ylim                     (Plot* plot, float min, float max);
void        plot_autoscale                    (Plot* plot);
bool        plot_get_xlim                     (const Plot* plot, float* outMin, float* outMax);
bool        plot_get_ylim                     (const Plot* plot, float* outMin, float* outMax);


/* ------------------------------------------------------------------ */
/* Themes                                                             */
/* ------------------------------------------------------------------ */

void        plot_apply_theme_dark             (Plot* plot);
void        plot_apply_theme_light            (Plot* plot);


/* ------------------------------------------------------------------ */
/* Color helpers                                                      */
/* ------------------------------------------------------------------ */

PlotColor   plot_color_rgb                    (unsigned char r, unsigned char g, unsigned char b);
PlotColor   plot_color_hex                    (unsigned int rgb_or_rgba);

static inline PlotColor plot_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    PlotColor c = { r, g, b, a };
    return c;
}


/* ------------------------------------------------------------------ */
/* Rendering / export                                                 */
/* ------------------------------------------------------------------ */

void        plot_draw                         (Plot* plot);
bool        plot_export_image                 (Plot* plot, const char* filename);
bool        plot_export_csv                   (const Plot* plot, const char* filename);


/* ------------------------------------------------------------------ */
/* Utilities                                                          */
/* ------------------------------------------------------------------ */

bool        plot_linspace                     (float start, float end, size_t n, float* out);


/* ------------------------------------------------------------------ */
/* Fonts / convenience series / extra introspection                   */
/* ------------------------------------------------------------------ */

void        plot_set_font_size                (Plot* plot, int titleSize, int labelSize);
int         plot_add_histogram                (Plot* plot, const float* values, size_t n, const char* label, PlotColor color);
int         plot_add_pie                      (Plot* plot, const float* values, size_t n, const char* label, PlotColor color);
void        plot_get_size                     (const Plot* plot, int* width, int* height);
bool        plot_series_stddev                (const Plot* plot, size_t index, float* out);


#ifdef __cplusplus
}
#endif

#endif 
