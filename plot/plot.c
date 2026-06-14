/**
 * @author Amin Tahmasebi
 * @class Plot
 *
 * Implementation of the Plot module. Doxygen comments live above each
 * public function.
 */
#include "plot.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <raylib.h>

/* Compatibility shim for raylib 4.x vs 5.x. */
#if !defined(GetTextureData)
#define GetTextureData LoadImageFromTexture
#endif


static Color plotcolor_to_raylib(PlotColor c) {
    return (Color){c.r, c.g, c.b, c.a};
}


/* Portable string duplication so we don't drag in strdup. */
static char* p_strdup(const char* s) {
    if (!s) {
        return NULL;
    }

    size_t n = strlen(s) + 1;
    char* out = (char*)malloc(n);
    if (out) {
        memcpy(out, s, n);
    }

    return out;
}


/* Compute the effective x/y min/max used for rendering. Each axis is
 * auto-scaled to the data unless a manual range was pinned with
 * plot_set_xlim / plot_set_ylim, in which case the pinned range wins. */
static void compute_extents(const Plot* plot, float* xMin, float* xMax, float* yMin, float* yMax) {
    float axMin =  FLT_MAX, axMax = -FLT_MAX;
    float ayMin =  FLT_MAX, ayMax = -FLT_MAX;
    for (size_t s = 0; s < plot->seriesCount; ++s) {
        const PlotSeries* ser = &plot->series[s];

        for (size_t i = 0; i < ser->dataSize; ++i) {
            float xv = ser->xData ? ser->xData[i] : (float)i;
            if (xv < axMin) {
                axMin = xv;
            }
            if (xv > axMax) {
                axMax = xv;
            }
            if (ser->yData[i] < ayMin) {
                ayMin = ser->yData[i];
            }
            if (ser->yData[i] > ayMax) {
                ayMax = ser->yData[i];
            }
        }
    }
    if (axMax == axMin) {
        axMin -= 1.0f;
        axMax += 1.0f;
    }
    if (ayMax == ayMin) {
        ayMin -= 1.0f;
        ayMax += 1.0f;
    }

    /* Manual ranges override the auto-computed ones, independently per axis. */
    if (plot->xRangeManual) {
        *xMin = plot->xMinManual;
        *xMax = plot->xMaxManual;
    } else {
        *xMin = axMin;
        *xMax = axMax;
    }
    if (plot->yRangeManual) {
        *yMin = plot->yMinManual;
        *yMax = plot->yMaxManual;
    } else {
        *yMin = ayMin;
        *yMax = ayMax;
    }
}


/* Draw the plot contents into the currently-bound render target.
 * Shared by plot_draw (live window) and plot_export_image (offscreen). */
static void render_plot(const Plot* plot, Font font) {
    const int padding = 60;
    float xMin, xMax, yMin, yMax;
    compute_extents(plot, &xMin, &xMax, &yMin, &yMax);

    float xRange = xMax - xMin;
    float yRange = yMax - yMin;

    ClearBackground(plotcolor_to_raylib(plot->bgColor));

    if (plot->showGrid) {
        for (int i = 0; i <= 10; ++i) {
            int x = padding + (plot->width  - 2 * padding) * i / 10;
            int y = padding + (plot->height - 2 * padding) * i / 10;

            DrawLine(x, padding, x, plot->height - padding, plotcolor_to_raylib(plot->gridColor));
            DrawLine(padding, y, plot->width - padding, y, plotcolor_to_raylib(plot->gridColor));
        }
    }

    DrawLine(padding, plot->height - padding, plot->width - padding, plot->height - padding, plotcolor_to_raylib(plot->axisColor));
    DrawLine(padding, plot->height - padding, padding, padding, plotcolor_to_raylib(plot->axisColor));
    DrawText(plot->xLabel, plot->width / 2 - MeasureText(plot->xLabel, plot->labelFontSize) / 2, plot->height - padding + 20, plot->labelFontSize, plotcolor_to_raylib(plot->axisColor));

    Vector2 yLabelPos = {10, (float)(plot->height / 2 + MeasureText(plot->yLabel, plot->labelFontSize) / 2)};

    DrawTextPro(font, plot->yLabel, yLabelPos, (Vector2){0, 0}, -90, (float)plot->labelFontSize, 2, plotcolor_to_raylib(plot->axisColor));
    DrawText(plot->title, plot->width / 2 - MeasureText(plot->title, plot->titleFontSize) / 2, 10, plot->titleFontSize, plotcolor_to_raylib(plot->axisColor));

    for (size_t s = 0; s < plot->seriesCount; ++s) {
        const PlotSeries* ser = &plot->series[s];
        Color c = plotcolor_to_raylib(ser->color);

        if (ser->pltype == PLTYPE_LINE) {
            for (size_t i = 0; i + 1 < ser->dataSize; ++i) {
                int x1 = padding + (int)(((ser->xData ? ser->xData[i]     : (float)i)     - xMin) / xRange * (plot->width  - 2 * padding));
                int y1 = plot->height - padding - (int)((ser->yData[i]     - yMin) / yRange * (plot->height - 2 * padding));
                int x2 = padding + (int)(((ser->xData ? ser->xData[i + 1] : (float)(i + 1)) - xMin) / xRange * (plot->width  - 2 * padding));
                int y2 = plot->height - padding - (int)((ser->yData[i + 1] - yMin) / yRange * (plot->height - 2 * padding));

                DrawLine(x1, y1, x2, y2, c);
                DrawCircle(x1, y1, 3, c);
            }
        } 
        else if (ser->pltype == PLTYPE_BAR) {
            int avail = plot->width - 2 * padding;
            int barWidth = ser->dataSize ? (int)(avail / ser->dataSize) - 4 : 4;

            if (barWidth < 2) {
                barWidth = 2;
            }
            for (size_t i = 0; i < ser->dataSize; ++i) {
                int x = padding + (int)(((ser->xData ? ser->xData[i] : (float)i) - xMin) / xRange * avail);
                int y = plot->height - padding - (int)((ser->yData[i] - yMin) / yRange * (plot->height - 2 * padding));
                int h = (plot->height - padding) - y;

                DrawRectangle(x - barWidth / 2, y, barWidth, h, c);
                DrawRectangleLines(x - barWidth / 2, y, barWidth, h, plotcolor_to_raylib(plot->axisColor));
            }
        } 
        else if (ser->pltype == PLTYPE_SCATTER) {
            for (size_t i = 0; i < ser->dataSize; ++i) {
                int x = padding + (int)(((ser->xData ? ser->xData[i] : (float)i) - xMin) / xRange * (plot->width - 2 * padding));
                int y = plot->height - padding - (int)((ser->yData[i] - yMin) / yRange * (plot->height - 2 * padding));

                DrawCircle(x, y, 6, c);
                DrawCircleLines(x, y, 6, plotcolor_to_raylib(plot->axisColor));
            }
        } 
        else if (ser->pltype == PLTYPE_PIE) {
            float total = 0;
            for (size_t i = 0; i < ser->dataSize; ++i) {
                if (ser->yData[i] > 0) {
                    total += ser->yData[i];
                }
            }
            if (total <= 0) {
                continue;
            }

            Vector2 center = {(float)(plot->width / 2), (float)(plot->height / 2)};
            float radius = (float)((plot->height < plot->width ? plot->height : plot->width) / 3);
            float startAngle = 0;

            for (size_t i = 0; i < ser->dataSize; ++i) {
                if (ser->yData[i] <= 0) {
                    continue;
                }
                float sliceAngle = (ser->yData[i] / total) * 360.0f;
                if (sliceAngle < 0.1f) {
                    continue;
                }
                
                DrawCircleSector(center, radius, startAngle, startAngle + sliceAngle, 50, c);
                float midAngle = startAngle + sliceAngle / 2.0f;
                float labelX = center.x + cosf(DEG2RAD * midAngle) * (radius * 0.6f);
                float labelY = center.y + sinf(DEG2RAD * midAngle) * (radius * 0.6f);
                char label[16];
                
                snprintf(label, sizeof(label), "%.1f%%", (ser->yData[i] / total) * 100);
                int labelW = MeasureText(label, 16);
                DrawText(label, (int)(labelX - labelW / 2), (int)labelY, 16, plotcolor_to_raylib(plot->axisColor));
                startAngle += sliceAngle;
            }
        } 
        else if (ser->pltype == PLTYPE_HISTOGRAM) {
            const int numBins = 10;
            float binWidth = (yMax - yMin) / numBins;
            if (binWidth == 0) {
                binWidth = 1e-6f;
            }
            int binCounts[10] = {0};

            for (size_t i = 0; i < ser->dataSize; ++i) {
                int binIdx = (int)((ser->yData[i] - yMin) / binWidth);
                if (binIdx < 0) {
                    binIdx = 0;
                }
                if (binIdx >= numBins) {
                    binIdx = numBins - 1;
                }
                binCounts[binIdx]++;
            }
            float maxCount = 0;
            for (int i = 0; i < numBins; ++i) {
                if (binCounts[i] > maxCount) {
                    maxCount = (float)binCounts[i];
                }
            }
            if (maxCount == 0) {
                continue;
            }
            float barWidth = (float)((plot->width - 2 * padding) / numBins);
            for (int i = 0; i < numBins; ++i) {
                float barHeight = (binCounts[i] / maxCount) * (plot->height - 150);
                int x = padding + (int)(i * barWidth);
                int y = plot->height - padding - (int)barHeight;
                DrawRectangle(x, y, (int)barWidth - 5, (int)barHeight, c);
                DrawRectangleLines(x, y, (int)barWidth - 5, (int)barHeight, plotcolor_to_raylib(plot->axisColor));
            }
        }
    }

    if (plot->showLegend && plot->seriesCount > 0) {
        int legendW = 180, legendH = (int)(30 * plot->seriesCount + 10);
        int lx = plot->width - legendW - 20, ly = 40;
        DrawRectangle(lx, ly, legendW, legendH, plotcolor_to_raylib(plot->legendBgColor));

        for (size_t s = 0; s < plot->seriesCount; ++s) {
            DrawRectangle(lx + 10, ly + 10 + 30 * (int)s, 20, 20, plotcolor_to_raylib(plot->series[s].color));
            DrawText(plot->series[s].label ? plot->series[s].label : "(series)", lx + 40, ly + 10 + 30 * (int)s, 18, plotcolor_to_raylib(plot->legendTextColor));
        }
        DrawRectangleLines(lx, ly, legendW, legendH, plotcolor_to_raylib(plot->axisColor));
    }
}


/**
 * @brief Create a new plot configured with default colours and sizes.
 *
 * Allocates a zero-initialized Plot via `calloc` and populates it with
 * sane defaults so it can be drawn immediately even before any series
 * is attached:
 *   - title / xLabel / yLabel are deep-copied (NULL becomes the literal
 *     "Untitled Plot" / "X Axis" / "Y Axis"),
 *   - the canvas size is 800x600 pixels,
 *   - the legend and grid are both visible,
 *   - the colour palette matches the built-in light theme (white
 *     background, black axes, soft grey grid, off-white legend).
 *
 * Ownership semantics: the returned Plot owns every string it stores;
 * the caller may free its own copies of @p title / @p xLabel / @p yLabel
 * immediately after this call returns. The returned object must be
 * released with plot_destroy() to avoid leaks.
 *
 * @param title  Plot title text. NULL is replaced with "Untitled Plot".
 * @param xLabel Horizontal axis label. NULL is replaced with "X Axis".
 * @param yLabel Vertical axis label. NULL is replaced with "Y Axis".
 *
 * @return Newly-allocated Plot on success, or NULL on allocation
 *         failure. The caller takes ownership and must call
 *         plot_destroy() when done.
 *
 * @note The strings stored on the plot are individually allocated; if
 *       any one of the three deep copies fails internally the plot is
 *       still returned with whatever subset succeeded — pair this with
 *       plot_set_title() / plot_set_axis_labels() if you need stricter
 *       error handling.
 *
 * @code
 *     Plot* p = plot_create("Temperature today", "Hour", "Celsius");
 *     if (!p) {
 *         fprintf(stderr, "out of memory\n");
 *         return 1;
 *     }
 *     // ... add series, then ...
 *     plot_draw(p);
 *     plot_destroy(p);
 * @endcode
 */
Plot* plot_create(const char* title, const char* xLabel, const char* yLabel) {
    PLOT_LOG("[plot_create]: enter (title=%s, xLabel=%s, yLabel=%s)", title  ? title  : "(null)",
             xLabel ? xLabel : "(null)",
             yLabel ? yLabel : "(null)");
    Plot* plot = (Plot*)calloc(1, sizeof(Plot));
    if (!plot) {
        PLOT_LOG("[plot_create]: Allocation failed -> NULL");
        return NULL;
    }

    plot->title  = p_strdup(title  ? title  : "Untitled Plot");
    plot->xLabel = p_strdup(xLabel ? xLabel : "X Axis");
    plot->yLabel = p_strdup(yLabel ? yLabel : "Y Axis");
    plot->width  = 800;
    plot->height = 600;
    plot->showLegend = true;
    plot->showGrid   = true;
    plot->bgColor          = plot_color(255, 255, 255, 255);
    plot->gridColor        = plot_color(220, 220, 220, 255);
    plot->axisColor        = plot_color(  0,   0,   0, 255);
    plot->legendBgColor    = plot_color(245, 245, 245, 220);
    plot->legendTextColor  = plot_color(  0,   0,   0, 255);
    plot->titleFontSize = 28;
    plot->labelFontSize = 20;
    plot->seriesCount = 0;
    plot->xRangeManual = false;   /* axes auto-scale to the data until pinned */
    plot->yRangeManual = false;

    PLOT_LOG("[plot_create]: exit -> %p", (void*)plot);
    return plot;
}


/**
 * @brief Release every resource owned by the plot.
 *
 * Iterates each series and frees its xData / yData / label buffers,
 * then frees the title and axis-label strings, and finally the Plot
 * struct itself. After this call the pointer @p plot is invalid and
 * must not be reused.
 *
 * This function is the only correct way to dispose of a Plot returned
 * by plot_create(); using `free()` directly would leak every embedded
 * allocation.
 *
 * @param plot Plot to destroy. May be NULL, in which case the call is
 *             a no-op.
 *
 * @note This function does not touch any live raylib window. If you
 *       invoked plot_draw() and it returned, the window has already
 *       been closed and there is nothing extra to clean up.
 * @warning Do not call plot_destroy() twice on the same pointer — the
 *          second call is a double-free.
 *
 * @code
 *     Plot* p = plot_create("demo", NULL, NULL);
 *     // ... use p ...
 *     plot_destroy(p);
 *     p = NULL;          // recommended: prevents accidental reuse
 * @endcode
 */
void plot_destroy(Plot* plot) {
    PLOT_LOG("[plot_destroy]: enter (plot=%p)", (void*)plot);
    if (!plot) {
        PLOT_LOG("[plot_destroy]: NULL plot -> noop");
        return;
    }

    for (size_t i = 0; i < plot->seriesCount; ++i) {
        free(plot->series[i].xData);
        free(plot->series[i].yData);
        free((char*)plot->series[i].label);
    }

    free((char*)plot->xLabel);
    free((char*)plot->yLabel);
    free((char*)plot->title);
    free(plot);

    PLOT_LOG("[plot_destroy]: exit -> destroyed");
}


/**
 * @brief Append a new series of any drawing style to the plot.
 *
 * Allocates internal buffers and deep-copies @p xData (if non-NULL),
 * @p yData, and @p label into them. The series is stored in the next
 * free slot of the plot's fixed-capacity series table and given the
 * supplied colour and drawing style.
 *
 * If @p xData is NULL the renderer falls back to using the implicit
 * indices 0..dataSize-1 when drawing — useful for line/bar charts where
 * the horizontal axis is "sample number".
 *
 * Ownership semantics:
 *   - The plot retains ownership of every internal copy; the caller
 *     may free its own buffers and label string immediately after this
 *     call returns.
 *   - On any failure all partial allocations are rolled back and the
 *     plot is left untouched.
 *
 * @param plot     Target plot. Must be non-NULL.
 * @param xData    Optional x values. NULL means "use 0..dataSize-1".
 *                 When non-NULL it must point to @p dataSize floats.
 * @param yData    Y values. Must be non-NULL and point to @p dataSize
 *                 floats.
 * @param dataSize Number of points. Must be > 0.
 * @param label    Legend label; deep-copied. NULL is allowed and means
 *                 "no label".
 * @param pltype   Drawing style (PLTYPE_LINE, PLTYPE_BAR, PLTYPE_SCATTER,
 *                 PLTYPE_PIE, PLTYPE_HISTOGRAM).
 * @param color    Stroke / fill colour for this series.
 *
 * @return Zero-based index of the new series on success, or -1 on bad
 *         input (NULL plot/yData, zero dataSize), if the plot already
 *         holds PLOT_MAX_SERIES series, or on allocation failure.
 *
 * @warning Indices returned by this function are only stable until
 *          plot_remove_series() is called; removing a series shifts
 *          every later series down by one.
 *
 * @code
 *     float y[] = {1.0f, 2.5f, 4.0f, 2.0f};
 *     int idx = plot_add_series(plot, NULL, y, 4, "samples",
 *                               PLTYPE_LINE, plot_color_rgb(0, 102, 204));
 *     if (idx < 0) {
 *         // handle full plot / OOM
 *     }
 * @endcode
 */
int plot_add_series(Plot* plot, const float* xData, const float* yData, size_t dataSize, const char* label, PlotType pltype, PlotColor color) {
    PLOT_LOG("[plot_add_series]: enter (plot=%p, dataSize=%zu, label=%s, pltype=%d, seriesCount=%zu)", (void*)plot, dataSize, label ? label : "(null)", (int)pltype,
             plot ? plot->seriesCount : (size_t)0);

    if (!plot || !yData || dataSize == 0 || plot->seriesCount >= PLOT_MAX_SERIES) {
        PLOT_LOG("[plot_add_series]: Invalid args or max series reached -> -1");
        return -1;
    }

    float* xCopy = NULL;
    if (xData) {
        xCopy = (float*)malloc(sizeof(float) * dataSize);
        if (!xCopy) {
            PLOT_LOG("[plot_add_series]: OOM xCopy -> -1");
            return -1;
        }
        memcpy(xCopy, xData, sizeof(float) * dataSize);
    }

    float* yCopy = (float*)malloc(sizeof(float) * dataSize);
    if (!yCopy) {
        PLOT_LOG("[plot_add_series]: OOM yCopy -> -1");
        free(xCopy);

        return -1;
    }
    memcpy(yCopy, yData, sizeof(float) * dataSize);

    char* labelCopy = NULL;
    if (label) {
        labelCopy = p_strdup(label);
        if (!labelCopy) {
            PLOT_LOG("[plot_add_series]: OOM labelCopy -> -1");
            free(xCopy);
            free(yCopy);

            return -1;
        }
    }

    size_t idx = plot->seriesCount++;
    plot->series[idx].xData    = xCopy;
    plot->series[idx].yData    = yCopy;
    plot->series[idx].dataSize = dataSize;
    plot->series[idx].label    = labelCopy;
    plot->series[idx].color    = color;
    plot->series[idx].pltype   = pltype;

    PLOT_LOG("[plot_add_series]: exit -> index=%d (%s)", (int)idx, label ? label : "(no label)");
    return (int)idx;
}


/**
 * @brief Remove the series at @p seriesIndex, shifting later series left.
 *
 * Frees the targeted series' xData, yData, and label buffers, then
 * compacts the series table so that any series that were stored at a
 * higher index get shifted down by one. The plot's seriesCount is
 * decremented to reflect the removal.
 *
 * @param plot        Target plot. Must be non-NULL.
 * @param seriesIndex Index of the series to remove. Must be strictly
 *                    less than plot_get_series_count(plot).
 *
 * @return true on success; false if @p plot is NULL or @p seriesIndex
 *         is out of range (in which case nothing is freed).
 *
 * @warning Any series indices cached by the caller for series stored
 *          *after* @p seriesIndex are invalidated by this call —
 *          re-query them via plot_get_series_count() / your own label
 *          lookup if you need them again.
 *
 * @code
 *     // Drop the first series and keep the rest:
 *     if (!plot_remove_series(plot, 0)) {
 *         fprintf(stderr, "no such series\n");
 *     }
 * @endcode
 */
bool plot_remove_series(Plot* plot, size_t seriesIndex) {
    PLOT_LOG("[plot_remove_series]: enter (plot=%p, seriesIndex=%zu)", (void*)plot, seriesIndex);
    if (!plot || seriesIndex >= plot->seriesCount) {
        PLOT_LOG("[plot_remove_series]: bad arg or OOR -> false");
        return false;
    }

    free(plot->series[seriesIndex].xData);
    free(plot->series[seriesIndex].yData);
    free((char*)plot->series[seriesIndex].label);

    for (size_t i = seriesIndex; i + 1 < plot->seriesCount; ++i) {
        plot->series[i] = plot->series[i + 1];
    }
    plot->seriesCount--;

    PLOT_LOG("[plot_remove_series]: exit -> true (count=%zu)", plot->seriesCount);
    return true;
}


/**
 * @brief Replace a series' x/y data while keeping its label, color, type.
 *
 * Allocates fresh internal buffers, deep-copies @p xData (if non-NULL)
 * and @p yData into them, frees the series' previous buffers, and
 * installs the new ones in place. The series' label, colour, and
 * drawing style are preserved — only the data and its length change.
 *
 * Use this for animated / "live" plots where a series' data evolves
 * frame by frame, instead of repeatedly removing and re-adding it.
 *
 * Ownership semantics: the plot retains ownership of the new buffers;
 * the caller may free its own copies of @p xData / @p yData
 * immediately after the call returns. On failure neither the old nor
 * the new data is leaked.
 *
 * @param plot        Target plot. Must be non-NULL.
 * @param seriesIndex Index of the series to update. Must be in range.
 * @param xData       Optional new x values; NULL means "use implicit
 *                    0..dataSize-1".
 * @param yData       New y values; must point to @p dataSize floats.
 * @param dataSize    Number of points in the new arrays. Must be > 0.
 *
 * @return true on success; false on bad input (NULL plot/yData, zero
 *         dataSize, out-of-range index) or on allocation failure (the
 *         previous data is preserved in that case).
 *
 * @code
 *     // Replace series 0 with a noisier signal:
 *     float y[256];
 *     // ... fill y ...
 *     plot_update_series_data(plot, 0, NULL, y, 256);
 * @endcode
 */
bool plot_update_series_data(Plot* plot, size_t seriesIndex, const float* xData, const float* yData, size_t dataSize) {
    PLOT_LOG("[plot_update_series_data]: enter (plot=%p, seriesIndex=%zu, dataSize=%zu)", (void*)plot, seriesIndex, dataSize);
    if (!plot || seriesIndex >= plot->seriesCount || !yData || dataSize == 0) {
        PLOT_LOG("[plot_update_series_data]: bad arg or OOR -> false");
        return false;
    }

    float* xCopy = NULL;
    if (xData) {
        xCopy = (float*)malloc(sizeof(float) * dataSize);
        if (!xCopy) {
            PLOT_LOG("[plot_update_series_data]: OOM xCopy -> false");
            return false;
        }
        memcpy(xCopy, xData, sizeof(float) * dataSize);
    }

    float* yCopy = (float*)malloc(sizeof(float) * dataSize);
    if (!yCopy) {
        PLOT_LOG("[plot_update_series_data]: OOM yCopy -> false");
        free(xCopy);
        return false;
    }
    memcpy(yCopy, yData, sizeof(float) * dataSize);

    PlotSeries* s = &plot->series[seriesIndex];
    free(s->xData);
    free(s->yData);

    s->xData = xCopy;
    s->yData = yCopy;
    s->dataSize = dataSize;

    PLOT_LOG("[plot_update_series_data]: exit -> true (series %zu now has %zu points)", seriesIndex, dataSize);
    return true;
}


/**
 * @brief Set the desired window size in pixels.
 *
 * Updates the plot's @c width and @c height fields, which are used
 * both for the live raylib window opened by plot_draw() and for the
 * offscreen render target used by plot_export_image(). Either value
 * below 100 is silently clamped to 100 to avoid degenerate, hard-to-
 * read plots.
 *
 * @param plot   Target plot. NULL is a no-op.
 * @param width  Desired canvas width in pixels (minimum 100).
 * @param height Desired canvas height in pixels (minimum 100).
 *
 * @note Calling this after plot_draw() has already opened its window
 *       has no effect on the currently-displayed frame; the window
 *       size is captured at the start of plot_draw(). Use plot_set_size()
 *       if you want strict input validation that fails rather than
 *       clamps.
 */
void plot_set_window_size(Plot* plot, int width, int height) {
    PLOT_LOG("[plot_set_window_size]: enter (plot=%p, width=%d, height=%d)", (void*)plot, width, height);
    if (!plot) {
        PLOT_LOG("[plot_set_window_size]: NULL plot -> noop");
        return;
    }
    if (width  < 100) {
        width  = 100;
    }
    if (height < 100) {
        height = 100;
    }

    plot->width  = width;
    plot->height = height;

    PLOT_LOG("[plot_set_window_size]: exit -> %dx%d", width, height);
}


/**
 * @brief Toggle the legend on / off.
 *
 * Enables or disables the rendering of the legend box in the top-right
 * corner of the plot. The legend lists one row per series, each with
 * the series' colour swatch and its label (or `(series)` when the
 * series was added with a NULL label).
 *
 * @param plot Target plot. NULL is a no-op.
 * @param show true to render the legend; false to hide it.
 *
 * @note Hiding the legend does not free any memory; series labels are
 *       still kept on the plot and can be re-shown at any time.
 */
void plot_set_legend(Plot* plot, bool show) {
    PLOT_LOG("[plot_set_legend]: enter (plot=%p, show=%d)", (void*)plot, (int)show);
    if (!plot) {
        PLOT_LOG("[plot_set_legend]: NULL plot -> noop");
        return;
    }
    plot->showLegend = show;

    PLOT_LOG("[plot_set_legend]: exit");
}


/**
 * @brief Toggle the grid on / off.
 *
 * Controls whether the renderer draws the 11x11 background grid lines
 * inside the plot frame. The grid is drawn in @c plot->gridColor (see
 * plot_set_grid_color()) so its visibility against the chart's
 * background is style-sensitive.
 *
 * @param plot Target plot. NULL is a no-op.
 * @param show true to render the grid; false to hide it.
 */
void plot_set_grid(Plot* plot, bool show) {
    PLOT_LOG("[plot_set_grid]: enter (plot=%p, show=%d)", (void*)plot, (int)show);

    if (!plot) {
        PLOT_LOG("[plot_set_grid]: NULL plot -> noop");
        return;
    }
    plot->showGrid = show;

    PLOT_LOG("[plot_set_grid]: exit");
}


/**
 * @brief Set all five styling colours of the plot in one call.
 *
 * Overwrites the plot's background, grid, axis, legend-background, and
 * legend-text colours with the supplied values. The series-specific
 * colours are not affected — they belong to each series and are
 * changed individually via plot_set_series_color().
 *
 * Use this when you want to install a custom theme of your own; for
 * the two built-in themes prefer plot_apply_theme_light() /
 * plot_apply_theme_dark(). When you only want to tweak one colour
 * reach for plot_set_background_color(), plot_set_grid_color(),
 * plot_set_axis_color(), or plot_set_legend_colors() instead.
 *
 * @param plot       Target plot. NULL is a no-op.
 * @param bg         New background fill.
 * @param grid       New grid line colour.
 * @param axis       New axis, frame, title and label colour.
 * @param legendBg   New legend background fill.
 * @param legendText New legend label text colour.
 *
 * @code
 *     // Solarised-ish manual theme:
 *     plot_set_colors(plot,
 *         plot_color_hex(0xFDF6E3),  // bg
 *         plot_color_hex(0xEEE8D5),  // grid
 *         plot_color_hex(0x657B83),  // axis
 *         plot_color_hex(0xEEE8D5),  // legend bg
 *         plot_color_hex(0x586E75)); // legend text
 * @endcode
 */
void plot_set_colors(Plot* plot, PlotColor bg, PlotColor grid, PlotColor axis, PlotColor legendBg, PlotColor legendText) {
    PLOT_LOG("[plot_set_colors]: enter (plot=%p)", (void*)plot);
    if (!plot) {
        PLOT_LOG("[plot_set_colors]: NULL plot -> noop");
        return;
    }

    plot->bgColor          = bg;
    plot->gridColor        = grid;
    plot->axisColor        = axis;
    plot->legendBgColor    = legendBg;
    plot->legendTextColor  = legendText;
    PLOT_LOG("[plot_set_colors]: exit");
}


/**
 * @brief Open a window and render the plot until the user closes it.
 *
 * Initialises a raylib window sized to the plot's current width/height,
 * sets the frame rate to 60 FPS, loads the default font, and enters a
 * blocking render loop that re-renders the plot every frame using the
 * shared internal `render_plot()` helper. The loop exits when the user
 * closes the window (or presses ESC, per raylib defaults), after which
 * the window is destroyed before returning.
 *
 * Because this function opens a real OS window, it can only be used in
 * environments where a display server is available. For headless
 * environments (CI, servers) use plot_export_image() to render directly
 * to a PNG file instead.
 *
 * @param plot Target plot. If @p plot is NULL or has zero series the
 *             call returns immediately without opening a window.
 *
 * @warning This call is blocking — it does not return until the user
 *          closes the window. Plot mutations made from a separate
 *          thread while plot_draw() is running are not synchronized
 *          and lead to undefined behaviour.
 *
 * @code
 *     Plot* p = plot_create("Live demo", "x", "y");
 *     plot_add_line(p, samples, n, "input", plot_color_rgb(0, 102, 204));
 *     plot_draw(p);          // blocks until the window is closed
 *     plot_destroy(p);
 * @endcode
 */
void plot_draw(Plot* plot) {
    PLOT_LOG("[plot_draw]: enter (plot=%p, seriesCount=%zu)", (void*)plot, plot ? plot->seriesCount : (size_t)0);
    if (!plot || plot->seriesCount == 0) {
        PLOT_LOG("[plot_draw]: No data -> return");
        return;
    }

    InitWindow(plot->width, plot->height, plot->title);
    SetTargetFPS(60);
    Font font = GetFontDefault();

    while (!WindowShouldClose()) {
        BeginDrawing();
        render_plot(plot, font);
        EndDrawing();
    }

    CloseWindow();
}


/**
 * @brief Render the plot to an offscreen surface and export as PNG.
 *
 * Performs a complete one-shot render of the plot to disk without
 * presenting a visible window:
 *   1. Configures raylib to suppress log output and to keep the
 *      window hidden (FLAG_WINDOW_HIDDEN). A window must still be
 *      created so raylib can establish a GL context.
 *   2. Allocates a RenderTexture2D matching the plot's width/height
 *      and renders the plot into it through the shared internal
 *      `render_plot()` helper.
 *   3. Reads the texture back into an Image and flips it vertically
 *      (the texture is bottom-up vs. screen coordinates), then writes
 *      the image to disk using raylib's ExportImage(). The file
 *      format is inferred from @p filename's extension (".png",
 *      ".jpg", ".bmp", ...).
 *   4. Releases the texture and image, then closes the temporary
 *      window before returning.
 *
 * @param plot     Source plot. Must be non-NULL and contain at least
 *                 one series.
 * @param filename Output path; the extension drives the file format.
 *                 Must be non-NULL.
 *
 * @return true on successful export; false on bad input (NULL plot or
 *         filename), if the plot has no data, or if raylib refused to
 *         write the file.
 *
 * @warning Like plot_draw(), this temporarily creates a (hidden) GL
 *          context, so it still requires a graphical environment to
 *          succeed — pure headless servers without a display backend
 *          may fail at InitWindow().
 *
 * @code
 *     plot_export_image(plot, "build/output.png");
 * @endcode
 */
bool plot_export_image(Plot* plot, const char* filename) {
    PLOT_LOG("[plot_export_image]: enter (plot=%p, filename=%s)", (void*)plot, filename ? filename : "(null)");
    if (!plot || !filename) {
        PLOT_LOG("[plot_export_image]: bad arg -> false");
        return false;
    }
    if (plot->seriesCount == 0) {
        PLOT_LOG("[plot_export_image]: No data to export -> false");
        return false;
    }

    SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(plot->width, plot->height, "Plot Export");

    RenderTexture2D target = LoadRenderTexture(plot->width, plot->height);
    Font font = GetFontDefault();

    BeginTextureMode(target);
    render_plot(plot, font);
    EndTextureMode();

    Image img = GetTextureData(target.texture);
    /* RenderTexture is stored upside-down vs. screen coordinates. */
    ImageFlipVertical(&img);
    bool ok = ExportImage(img, filename);
    UnloadImage(img);
    UnloadRenderTexture(target);
    CloseWindow();

    PLOT_LOG("[plot_export_image]: exit -> %s (file=%s)", ok ? "true" : "false", filename);
    return ok;
}


/**
 * @brief Build an opaque PlotColor from three 8-bit channels.
 *
 * Convenience wrapper over plot_color() that hard-codes the alpha
 * channel to 255 (fully opaque). Use plot_color() directly when you
 * need a translucent colour — for instance, a semi-transparent legend
 * background.
 *
 * @param r Red channel, 0..255.
 * @param g Green channel, 0..255.
 * @param b Blue channel, 0..255.
 *
 * @return PlotColor value with the requested RGB and alpha = 255. The
 *         function never fails.
 *
 * @code
 *     PlotColor accent = plot_color_rgb(255, 99, 71);   // tomato red
 *     plot_set_series_color(plot, 0, accent);
 * @endcode
 */
PlotColor plot_color_rgb(unsigned char r, unsigned char g, unsigned char b) {
    PLOT_LOG("[plot_color_rgb]: enter (r=%u, g=%u, b=%u)", (unsigned)r, (unsigned)g, (unsigned)b);
    return plot_color(r, g, b, 255);
}


/**
 * @brief Build a PlotColor from a packed 32-bit value.
 *
 * Unpacks @p rgb_or_rgba as 0xAARRGGBB. The top byte is treated as
 * alpha, the next byte as red, then green, then blue (least
 * significant). As a convenience for the common case of writing CSS-
 * style hex constants, a top byte of zero is interpreted as "no alpha
 * specified" and is silently replaced with 255 — so 0x336699 and
 * 0xFF336699 yield the same colour.
 *
 * @param rgb_or_rgba Packed colour:
 *                    - 0xRRGGBB form (top byte 0): treated as opaque.
 *                    - 0xAARRGGBB form (top byte != 0): used verbatim.
 *
 * @return The unpacked PlotColor. The function never fails.
 *
 * @note Because of the "top byte zero => opaque" rule, this function
 *       cannot produce a fully transparent colour. If you genuinely
 *       need alpha = 0 (e.g. for a transparent overlay), construct it
 *       with plot_color() directly.
 *
 * @code
 *     PlotColor blue   = plot_color_hex(0x336699);     // opaque
 *     PlotColor shaded = plot_color_hex(0x80FF0000);   // 50% red
 * @endcode
 */
PlotColor plot_color_hex(unsigned int rgb_or_rgba) {
    PLOT_LOG("[plot_color_hex]: enter (value=0x%08X)", rgb_or_rgba);

    unsigned char a = (unsigned char)((rgb_or_rgba >> 24) & 0xFF);
    unsigned char r = (unsigned char)((rgb_or_rgba >> 16) & 0xFF);
    unsigned char g = (unsigned char)((rgb_or_rgba >>  8) & 0xFF);
    unsigned char b = (unsigned char)( rgb_or_rgba        & 0xFF);

    if (a == 0) {
        a = 255;
    }

    return plot_color(r, g, b, a);
}


/**
 * @brief Replace the plot title; passing NULL clears it to "".
 *
 * Deep-copies @p title into the plot, then frees the previous title
 * string. Passing NULL stores the empty string "" (which renders as
 * a blank title bar) rather than leaving the field NULL — so the
 * renderer can always safely dereference @c plot->title.
 *
 * Ownership semantics: the caller may free its copy of @p title
 * immediately after this call returns. On allocation failure the
 * previous title is preserved unchanged.
 *
 * @param plot  Target plot. Must be non-NULL.
 * @param title New title text, or NULL to clear it.
 *
 * @return true on success; false if @p plot is NULL or the internal
 *         duplication failed (in which case the old title is kept).
 *
 * @code
 *     plot_set_title(plot, "Q4 Revenue (USD)");
 * @endcode
 */
bool plot_set_title(Plot* plot, const char* title) {
    PLOT_LOG("[plot_set_title]: enter (plot=%p, title=%s)", (void*)plot, title ? title : "(null)");
    if (!plot) {
        PLOT_LOG("[plot_set_title]: NULL plot -> false");
        return false;
    }

    char* dup = p_strdup(title ? title : "");
    if (!dup) {
        PLOT_LOG("[plot_set_title]: OOM dup -> false");
        return false;
    }
    free((char*)plot->title);
    plot->title = dup;

    PLOT_LOG("[plot_set_title]: exit -> true");
    return true;
}


/**
 * @brief Replace both axis labels in one call.
 *
 * Allocates fresh copies of @p xLabel and @p yLabel first; only if
 * both allocations succeed does the function free the previous labels
 * and install the new ones. This all-or-nothing strategy guarantees
 * that the plot is never left in a half-updated state.
 *
 * Passing NULL for either argument is allowed and stores the empty
 * string "" so the renderer can always safely dereference both label
 * pointers.
 *
 * @param plot   Target plot. Must be non-NULL.
 * @param xLabel New x-axis label, or NULL to clear it.
 * @param yLabel New y-axis label, or NULL to clear it.
 *
 * @return true on success; false if @p plot is NULL or either
 *         duplication failed. On failure both previous labels are
 *         preserved unchanged.
 *
 * @code
 *     plot_set_axis_labels(plot, "Time (s)", "Voltage (mV)");
 * @endcode
 */
bool plot_set_axis_labels(Plot* plot, const char* xLabel, const char* yLabel) {
    PLOT_LOG("[plot_set_axis_labels]: enter (plot=%p, xLabel=%s, yLabel=%s)", (void*)plot, xLabel ? xLabel : "(null)", yLabel ? yLabel : "(null)");
    if (!plot) {
        PLOT_LOG("[plot_set_axis_labels]: NULL plot -> false");
        return false;
    }

    char* x = p_strdup(xLabel ? xLabel : "");
    char* y = p_strdup(yLabel ? yLabel : "");

    if (!x || !y) {
        PLOT_LOG("[plot_set_axis_labels]: OOM -> false");
        free(x);
        free(y);
        return false;
    }

    free((char*)plot->xLabel);
    free((char*)plot->yLabel);

    plot->xLabel = x;
    plot->yLabel = y;

    PLOT_LOG("[plot_set_axis_labels]: exit -> true");
    return true;
}


/**
 * @brief Stricter sibling of plot_set_window_size() that validates
 *        inputs and reports success.
 *
 * Stores @p width and @p height verbatim on the plot. Unlike
 * plot_set_window_size(), which silently clamps small or zero sizes
 * to 100, this function rejects any non-positive dimension outright
 * by returning false without modifying the plot. Use this when you
 * want hard validation; use plot_set_window_size() when you want
 * lenient "do what I mean" behaviour.
 *
 * @param plot   Target plot. Must be non-NULL.
 * @param width  Desired canvas width in pixels. Must be > 0.
 * @param height Desired canvas height in pixels. Must be > 0.
 *
 * @return true on success; false on NULL plot or non-positive
 *         dimensions (the plot is left unchanged in that case).
 */
bool plot_set_size(Plot* plot, int width, int height) {
    PLOT_LOG("[plot_set_size]: enter (plot=%p, width=%d, height=%d)", (void*)plot, width, height);
    if (!plot || width <= 0 || height <= 0) {
        PLOT_LOG("[plot_set_size]: bad arg -> false");
        return false;
    }

    plot->width  = width;
    plot->height = height;

    PLOT_LOG("[plot_set_size]: exit -> true");
    return true;
}


/**
 * @brief Number of series currently attached to the plot.
 *
 * Returns the live seriesCount field. The result is bounded above by
 * the compile-time constant PLOT_MAX_SERIES; if it equals
 * PLOT_MAX_SERIES then any further plot_add_series() / plot_add_line()
 * / plot_add_bar() / plot_add_scatter() call will fail with -1.
 *
 * @param plot Plot to query. May be NULL.
 *
 * @return The series count, or 0 if @p plot is NULL.
 *
 * @code
 *     for (size_t i = 0; i < plot_get_series_count(plot); ++i) {
 *         printf("series %zu: %s\n", i, plot_get_series_label(plot, i));
 *     }
 * @endcode
 */
size_t plot_get_series_count(const Plot* plot) {
    PLOT_LOG("[plot_get_series_count]: enter (plot=%p)", (const void*)plot);
    size_t n = plot ? plot->seriesCount : 0;
    PLOT_LOG("[plot_get_series_count]: exit -> %zu", n);

    return n;
}


/**
 * @brief Remove every series, leaving the plot otherwise intact.
 *
 * Iterates each attached series, frees its xData / yData / label
 * buffers, zeroes the slot, and finally resets seriesCount to 0.
 * Styling fields (title, axis labels, colours, dimensions, legend /
 * grid visibility) are untouched, so the same Plot can be reused as
 * a fresh canvas without rebuilding it.
 *
 * @param plot Target plot. NULL is a no-op.
 *
 * @note If you also want to drop styling, just plot_destroy() and
 *       plot_create() again.
 */
void plot_clear_series(Plot* plot) {
    PLOT_LOG("[plot_clear_series]: enter (plot=%p, seriesCount=%zu)", (void*)plot, plot ? plot->seriesCount : (size_t)0);
    if (!plot) {
        PLOT_LOG("[plot_clear_series]: NULL plot -> noop");
        return;
    }

    for (size_t i = 0; i < plot->seriesCount; ++i) {
        free(plot->series[i].xData);
        free(plot->series[i].yData);
        free((char*)plot->series[i].label);

        plot->series[i].xData = NULL;
        plot->series[i].yData = NULL;
        plot->series[i].label = NULL;
        plot->series[i].dataSize = 0;
    }

    plot->seriesCount = 0;
    PLOT_LOG("[plot_clear_series]: exit");
}


/**
 * @brief Whether @p index addresses a valid series.
 *
 * Performs a bounds check against the plot's current series count.
 * Functionally equivalent to `(index < plot_get_series_count(plot))`
 * but also handles a NULL @p plot pointer gracefully.
 *
 * @param plot  Plot to query. May be NULL.
 * @param index Zero-based series index to test.
 *
 * @return true if @p plot is non-NULL and @p index is in range; false
 *         otherwise.
 */
bool plot_has_series(const Plot* plot, size_t index) {
    PLOT_LOG("[plot_has_series]: enter (plot=%p, index=%zu)", (const void*)plot, index);

    bool ok = plot && index < plot->seriesCount;
    PLOT_LOG("[plot_has_series]: exit -> %s", ok ? "true" : "false");

    return ok;
}


/**
 * @brief Convenience setter for the background colour.
 *
 * Overrides the colour passed to ClearBackground() at the start of
 * every frame. Useful when you want to keep most of the existing
 * theme but tweak a single colour rather than re-specifying all five
 * with plot_set_colors().
 *
 * @param plot Target plot. NULL is a no-op.
 * @param c    New background colour. Alpha is honoured.
 */
void plot_set_background_color(Plot* plot, PlotColor c) {
    PLOT_LOG("[plot_set_background_color]: enter (plot=%p)", (void*)plot);
    if (plot) {
        plot->bgColor = c;
    }

    PLOT_LOG("[plot_set_background_color]: exit");
}


/**
 * @brief Convenience setter for the grid line colour.
 *
 * Changes the colour of the 11x11 grid drawn behind the plotted
 * series. Has no effect when the grid is disabled via
 * plot_set_grid(false).
 *
 * @param plot Target plot. NULL is a no-op.
 * @param c    New grid line colour.
 */
void plot_set_grid_color(Plot* plot, PlotColor c) {
    PLOT_LOG("[plot_set_grid_color]: enter (plot=%p)", (void*)plot);
    if (plot) {
        plot->gridColor = c;
    }

    PLOT_LOG("[plot_set_grid_color]: exit");
}


/**
 * @brief Convenience setter for the axis (and frame) colour.
 *
 * The axis colour is reused as the colour of the x/y axis lines, the
 * plot title, both axis labels, the legend frame, the borders of bar
 * series, the outlines of scatter points, and the percentage labels on
 * pie slices. Changing it therefore visibly affects a lot more than
 * just the axes.
 *
 * @param plot Target plot. NULL is a no-op.
 * @param c    New axis / frame / text colour.
 */
void plot_set_axis_color(Plot* plot, PlotColor c) {
    PLOT_LOG("[plot_set_axis_color]: enter (plot=%p)", (void*)plot);
    if (plot) {
        plot->axisColor = c;
    }

    PLOT_LOG("[plot_set_axis_color]: exit");
}


/**
 * @brief Convenience setter for the legend background + text colours.
 *
 * Updates the legend's fill colour and the colour of its label text in
 * one call. The legend's border (drawn in the axis colour) and the
 * per-series colour swatches are unaffected.
 *
 * @param plot Target plot. NULL is a no-op.
 * @param bg   New legend background fill. Honour its alpha if you
 *             want a translucent legend on top of the plot.
 * @param text New legend label text colour.
 */
void plot_set_legend_colors(Plot* plot, PlotColor bg, PlotColor text) {
    PLOT_LOG("[plot_set_legend_colors]: enter (plot=%p)", (void*)plot);
    if (!plot) {
        PLOT_LOG("[plot_set_legend_colors]: NULL plot -> noop");
        return;
    }

    plot->legendBgColor   = bg;
    plot->legendTextColor = text;

    PLOT_LOG("[plot_set_legend_colors]: exit");
}


/**
 * @brief Add a line-chart series whose x axis is implicitly 0..n-1.
 *
 * Thin convenience wrapper around plot_add_series() with @c xData set
 * to NULL and @c pltype fixed to PLTYPE_LINE — appropriate when you
 * just want to draw a sequence of samples versus their index.
 *
 * Ownership semantics, error codes, and the "indices invalidated by
 * remove_series" caveat are exactly those of plot_add_series().
 *
 * @param plot  Target plot. Must be non-NULL.
 * @param y     Y values. Must point to @p n floats.
 * @param n     Number of points. Must be > 0.
 * @param label Legend label; deep-copied. May be NULL.
 * @param color Line / marker colour.
 *
 * @return Zero-based index of the new series, or -1 on bad input,
 *         full plot (PLOT_MAX_SERIES reached), or OOM.
 *
 * @code
 *     float y[] = {0.0f, 1.0f, 4.0f, 9.0f, 16.0f};
 *     plot_add_line(plot, y, 5, "x^2", plot_color_rgb(255, 0, 0));
 * @endcode
 */
int plot_add_line(Plot* plot, const float* y, size_t n, const char* label, PlotColor color) {
    PLOT_LOG("[plot_add_line]: enter (plot=%p, n=%zu, label=%s)", (void*)plot, n, label ? label : "(null)");

    return plot_add_series(plot, NULL, y, n, label, PLTYPE_LINE, color);
}


/**
 * @brief Add a scatter-plot series with explicit x and y arrays.
 *
 * Convenience wrapper around plot_add_series() that fixes @c pltype to
 * PLTYPE_SCATTER. Each point is drawn as a filled disc; pairs of
 * adjacent points are NOT connected. Both @p x and @p y are
 * deep-copied so the caller may free them immediately.
 *
 * @param plot  Target plot. Must be non-NULL.
 * @param x     X values. Must point to @p n floats. May be NULL, in
 *              which case implicit indices 0..n-1 are used.
 * @param y     Y values. Must point to @p n floats.
 * @param n     Number of points. Must be > 0.
 * @param label Legend label; deep-copied. May be NULL.
 * @param color Marker fill colour.
 *
 * @return Zero-based index of the new series, or -1 on failure (see
 *         plot_add_series() for the failure modes).
 *
 * @code
 *     plot_add_scatter(plot, xs, ys, n, "samples",
 *                      plot_color_rgb(40, 160, 40));
 * @endcode
 */
int plot_add_scatter(Plot* plot, const float* x, const float* y, size_t n, const char* label, PlotColor color) {
    PLOT_LOG("[plot_add_scatter]: enter (plot=%p, n=%zu, label=%s)", (void*)plot, n, label ? label : "(null)");

    return plot_add_series(plot, x, y, n, label, PLTYPE_SCATTER, color);
}


/**
 * @brief Add a bar-chart series whose x axis is implicitly 0..n-1.
 *
 * Convenience wrapper around plot_add_series() that fixes @c xData to
 * NULL and @c pltype to PLTYPE_BAR. The renderer draws one vertical
 * rectangle per sample, centred on the sample's x position and
 * stretching from the x axis up (or down) to its y value. Each bar
 * is outlined in the plot's axis colour for contrast.
 *
 * @param plot  Target plot. Must be non-NULL.
 * @param y     Bar heights. Must point to @p n floats.
 * @param n     Number of bars. Must be > 0.
 * @param label Legend label; deep-copied. May be NULL.
 * @param color Bar fill colour.
 *
 * @return Zero-based index of the new series, or -1 on failure (see
 *         plot_add_series() for failure modes).
 *
 * @code
 *     float counts[] = {12.0f, 34.0f, 28.0f, 41.0f};
 *     plot_add_bar(plot, counts, 4, "votes",
 *                  plot_color_rgb(102, 51, 153));
 * @endcode
 */
int plot_add_bar(Plot* plot, const float* y, size_t n, const char* label, PlotColor color) {
    PLOT_LOG("[plot_add_bar]: enter (plot=%p, n=%zu, label=%s)", (void*)plot, n, label ? label : "(null)");
    return plot_add_series(plot, NULL, y, n, label, PLTYPE_BAR, color);
}


/**
 * @brief Read back the legend label of the series at @p index.
 *
 * Returns a pointer to the series' internal label string. The pointer
 * is borrowed: the plot retains ownership and the caller must not free
 * it. The string remains valid only until the next operation that may
 * relocate or destroy the label — concretely plot_set_series_label(),
 * plot_remove_series(), plot_clear_series(), or plot_destroy().
 *
 * @param plot  Plot to query. May be NULL.
 * @param index Zero-based series index.
 *
 * @return Borrowed pointer to the label, or NULL when @p plot is NULL,
 *         @p index is out of range, or the series was added with a
 *         NULL label.
 *
 * @code
 *     const char* lbl = plot_get_series_label(plot, 0);
 *     printf("first series: %s\n", lbl ? lbl : "<unlabeled>");
 * @endcode
 */
const char* plot_get_series_label(const Plot* plot, size_t index) {
    PLOT_LOG("[plot_get_series_label]: enter (plot=%p, index=%zu)", (const void*)plot, index);
    if (!plot || index >= plot->seriesCount) {
        PLOT_LOG("[plot_get_series_label]: bad arg or OOR -> NULL");
        return NULL;
    }
    const char* lbl = plot->series[index].label;

    PLOT_LOG("[plot_get_series_label]: exit -> %s", lbl ? lbl : "(null)");
    return lbl;
}


/**
 * @brief Read back the drawing style of the series at @p index.
 *
 * Returns the PlotType the series was created with (or most recently
 * changed to via plot_set_series_type()). When the inputs are invalid
 * the function falls back to PLTYPE_LINE — callers that need to
 * distinguish "real PLTYPE_LINE" from "bad index" should validate the
 * index up front with plot_has_series().
 *
 * @param plot  Plot to query. May be NULL.
 * @param index Zero-based series index.
 *
 * @return The drawing style on success; PLTYPE_LINE if @p plot is NULL
 *         or @p index is out of range.
 */
PlotType plot_get_series_type(const Plot* plot, size_t index) {
    PLOT_LOG("[plot_get_series_type]: enter (plot=%p, index=%zu)", (const void*)plot, index);
    if (!plot || index >= plot->seriesCount) {
        PLOT_LOG("[plot_get_series_type]: bad arg or OOR -> PLTYPE_LINE");
        return PLTYPE_LINE;
    }
    PlotType t = plot->series[index].pltype;

    PLOT_LOG("[plot_get_series_type]: exit -> %d", (int)t);
    return t;
}


/**
 * @brief Read back the colour of the series at @p index.
 *
 * Returns the PlotColor stored for the series (initially set by
 * plot_add_series() and possibly overridden via
 * plot_set_series_color()). On invalid input the function returns
 * `{0, 0, 0, 0}` — fully transparent black — which is also a valid
 * (if unusual) colour, so prefer plot_has_series() to disambiguate.
 *
 * @param plot  Plot to query. May be NULL.
 * @param index Zero-based series index.
 *
 * @return The series' current colour, or transparent black on bad
 *         input.
 */
PlotColor plot_get_series_color(const Plot* plot, size_t index) {
    PLOT_LOG("[plot_get_series_color]: enter (plot=%p, index=%zu)", (const void*)plot, index);
    if (!plot || index >= plot->seriesCount) {
        PLOT_LOG("[plot_get_series_color]: bad arg or OOR -> transparent black");
        return plot_color(0, 0, 0, 0);
    }
    PLOT_LOG("[plot_get_series_color]: exit");

    return plot->series[index].color;
}


/**
 * @brief Number of data points in the series at @p index.
 *
 * Equal to the @c dataSize argument passed when the series was created
 * with plot_add_series() / plot_add_line() / plot_add_bar() /
 * plot_add_scatter() — or to the @c dataSize most recently supplied to
 * plot_update_series_data().
 *
 * @param plot  Plot to query. May be NULL.
 * @param index Zero-based series index.
 *
 * @return Number of points stored in the series, or 0 on NULL plot /
 *         out-of-range index.
 */
size_t plot_get_series_size(const Plot* plot, size_t index) {
    PLOT_LOG("[plot_get_series_size]: enter (plot=%p, index=%zu)", (const void*)plot, index);
    if (!plot || index >= plot->seriesCount) {
        PLOT_LOG("[plot_get_series_size]: bad arg or OOR -> 0");
        return 0;
    }

    size_t n = plot->series[index].dataSize;
    PLOT_LOG("[plot_get_series_size]: exit -> %zu", n);

    return n;
}


/**
 * @brief Replace the legend label of the series at @p index.
 *
 * Allocates a deep copy of @p label first; only when that succeeds is
 * the previous label freed and replaced. Passing NULL is a valid way
 * to clear the label entirely — the series will render in the legend
 * as `(series)` until a fresh label is set.
 *
 * Ownership semantics: the caller may free its copy of @p label
 * immediately after this call. On OOM the previous label is preserved.
 *
 * @param plot  Target plot. Must be non-NULL.
 * @param index Zero-based series index. Must be in range.
 * @param label New legend label, or NULL to clear.
 *
 * @return true on success; false if @p plot is NULL, @p index is out
 *         of range, or the internal duplication failed.
 *
 * @code
 *     plot_set_series_label(plot, 0, "filtered signal");
 * @endcode
 */
bool plot_set_series_label(Plot* plot, size_t index, const char* label) {
    PLOT_LOG("[plot_set_series_label]: enter (plot=%p, index=%zu, label=%s)", (void*)plot, index, label ? label : "(null)");
    if (!plot || index >= plot->seriesCount) {
        PLOT_LOG("[plot_set_series_label]: bad arg or OOR -> false");
        return false;
    }
    char* dup = NULL;

    if (label) {
        size_t n = strlen(label) + 1;
        dup = (char*)malloc(n);

        if (!dup) {
            PLOT_LOG("[plot_set_series_label]: OOM dup -> false");
            return false;
        }
        memcpy(dup, label, n);
    }
    free((char*)plot->series[index].label);
    plot->series[index].label = dup;

    PLOT_LOG("[plot_set_series_label]: exit -> true");
    return true;
}


/**
 * @brief Change the colour of the series at @p index.
 *
 * Overwrites the series' stored colour. The change takes effect the
 * very next time the plot is rendered — there is no global "redraw"
 * call to invoke because plot_draw() re-renders every frame.
 *
 * @param plot  Target plot. Must be non-NULL.
 * @param index Zero-based series index. Must be in range.
 * @param color New colour. Alpha is honoured.
 *
 * @return true on success; false if @p plot is NULL or @p index is
 *         out of range.
 */
bool plot_set_series_color(Plot* plot, size_t index, PlotColor color) {
    PLOT_LOG("[plot_set_series_color]: enter (plot=%p, index=%zu)", (void*)plot, index);
    if (!plot || index >= plot->seriesCount) {
        PLOT_LOG("[plot_set_series_color]: bad arg or OOR -> false");
        return false;
    }

    plot->series[index].color = color;
    PLOT_LOG("[plot_set_series_color]: exit -> true");

    return true;
}


/**
 * @brief Change the drawing style of the series at @p index.
 *
 * Reinterprets the same underlying x/y data as a different visual
 * style — for instance, swapping a line plot for a bar chart without
 * having to remove and re-add the series. The data, label, and colour
 * are preserved.
 *
 * @param plot  Target plot. Must be non-NULL.
 * @param index Zero-based series index. Must be in range.
 * @param type  New drawing style (PLTYPE_LINE, PLTYPE_BAR,
 *              PLTYPE_SCATTER, PLTYPE_PIE, PLTYPE_HISTOGRAM).
 *
 * @return true on success; false if @p plot is NULL or @p index is
 *         out of range.
 *
 * @note PLTYPE_PIE only makes sense for a single series at a time —
 *       multiple overlapping pies share the same canvas centre and
 *       will paint on top of each other.
 */
bool plot_set_series_type(Plot* plot, size_t index, PlotType type) {
    PLOT_LOG("[plot_set_series_type]: enter (plot=%p, index=%zu, type=%d)", (void*)plot, index, (int)type);
    if (!plot || index >= plot->seriesCount) {
        PLOT_LOG("[plot_set_series_type]: bad arg or OOR -> false");
        return false;
    }

    plot->series[index].pltype = type;
    PLOT_LOG("[plot_set_series_type]: exit -> true");

    return true;
}


/**
 * @brief Apply the built-in dark theme.
 *
 * Sets the five chrome colours in one call:
 *   - background:        @c 0x101418 (near-black slate),
 *   - grid lines:        @c 0x202830 (dim grey),
 *   - axes & text:       @c 0xC8D2DC (off-white),
 *   - legend background: @c 0x18202A (slightly lighter slate),
 *   - legend text:       @c 0xC8D2DC.
 *
 * Series colours are untouched, so high-saturation series colours
 * usually look great on top of this theme. Use plot_apply_theme_light()
 * for the inverse, or plot_set_colors() to install your own palette.
 *
 * @param plot Target plot. NULL is a no-op.
 *
 * @code
 *     plot_apply_theme_dark(plot);
 *     plot_draw(plot);
 * @endcode
 */
void plot_apply_theme_dark(Plot* plot) {
    PLOT_LOG("[plot_apply_theme_dark]: enter (plot=%p)", (void*)plot);
    if (!plot) {
        PLOT_LOG("[plot_apply_theme_dark]: NULL plot -> noop");
        return;
    }

    plot_set_background_color(plot, plot_color_hex(0x101418));
    plot_set_grid_color      (plot, plot_color_hex(0x202830));
    plot_set_axis_color      (plot, plot_color_hex(0xC8D2DC));
    plot_set_legend_colors   (plot, plot_color_hex(0x18202A), plot_color_hex(0xC8D2DC));

    PLOT_LOG("[plot_apply_theme_dark]: exit");
}


/**
 * @brief Apply the built-in light theme.
 *
 * Sets the five chrome colours in one call:
 *   - background:        @c 0xFFFFFF (pure white),
 *   - grid lines:        @c 0xDCDCDC (very pale grey),
 *   - axes & text:       @c 0x000000 (black),
 *   - legend background: @c 0xF5F5F5 (off-white),
 *   - legend text:       @c 0x000000.
 *
 * This is the same palette plot_create() installs by default, so
 * calling it on a freshly-created plot is a no-op — its main use is
 * to switch back to the light theme after temporarily applying the
 * dark one.
 *
 * @param plot Target plot. NULL is a no-op.
 */
void plot_apply_theme_light(Plot* plot) {
    PLOT_LOG("[plot_apply_theme_light]: enter (plot=%p)", (void*)plot);
    if (!plot) {
        PLOT_LOG("[plot_apply_theme_light]: NULL plot -> noop");
        return;
    }

    plot_set_background_color(plot, plot_color_hex(0xFFFFFF));
    plot_set_grid_color      (plot, plot_color_hex(0xDCDCDC));
    plot_set_axis_color      (plot, plot_color_hex(0x000000));
    plot_set_legend_colors   (plot, plot_color_hex(0xF5F5F5), plot_color_hex(0x000000));

    PLOT_LOG("[plot_apply_theme_light]: exit");
}


/**
 * @brief Minimum y of the series at @p index.
 *
 * Performs a single linear scan over the series' yData buffer and
 * writes the smallest value found into @p *out. The function is O(n)
 * in the series length and reads nothing else from @p plot.
 *
 * @param plot  Plot to query. Must be non-NULL.
 * @param index Zero-based series index. Must be in range.
 * @param out   Receives the minimum on success. Must be non-NULL. On
 *              failure the pointee is left unchanged.
 *
 * @return true on success; false if @p plot or @p out is NULL,
 *         @p index is out of range, or the series is empty.
 *
 * @code
 *     float lo;
 *     if (plot_series_min(plot, 0, &lo)) {
 *         printf("min = %g\n", (double)lo);
 *     }
 * @endcode
 */
bool plot_series_min(const Plot* plot, size_t index, float* out) {
    PLOT_LOG("[plot_series_min]: enter (plot=%p, index=%zu)", (const void*)plot, index);
    if (!plot || !out || index >= plot->seriesCount) {
        PLOT_LOG("[plot_series_min]: bad arg or OOR -> false");
        return false;
    }
    const PlotSeries* s = &plot->series[index];
    if (s->dataSize == 0) {
        PLOT_LOG("[plot_series_min]: empty series -> false");
        return false;
    }

    float m = s->yData[0];
    for (size_t i = 1; i < s->dataSize; ++i) {
        if (s->yData[i] < m) {
            m = s->yData[i];
        }
    }
    *out = m;
    PLOT_LOG("[plot_series_min]: exit -> true (min=%g)", (double)m);
    return true;
}


/**
 * @brief Maximum y of the series at @p index.
 *
 * Linear scan analogous to plot_series_min(); writes the largest
 * yData value of the targeted series into @p *out. The function is
 * O(n) in the series length.
 *
 * @param plot  Plot to query. Must be non-NULL.
 * @param index Zero-based series index. Must be in range.
 * @param out   Receives the maximum on success. Must be non-NULL.
 *
 * @return true on success; false if @p plot or @p out is NULL,
 *         @p index is out of range, or the series is empty.
 */
bool plot_series_max(const Plot* plot, size_t index, float* out) {
    PLOT_LOG("[plot_series_max]: enter (plot=%p, index=%zu)", (const void*)plot, index);
    if (!plot || !out || index >= plot->seriesCount) {
        PLOT_LOG("[plot_series_max]: bad arg or OOR -> false");
        return false;
    }

    const PlotSeries* s = &plot->series[index];
    if (s->dataSize == 0) {
        PLOT_LOG("[plot_series_max]: empty series -> false");
        return false;
    }

    float m = s->yData[0];
    for (size_t i = 1; i < s->dataSize; ++i) {
        if (s->yData[i] > m) {
            m = s->yData[i];
        }
    }

    *out = m;
    PLOT_LOG("[plot_series_max]: exit -> true (max=%g)", (double)m);

    return true;
}


/**
 * @brief Sum of y values in the series at @p index.
 *
 * Accumulates the series' yData buffer in a @c double accumulator and
 * stores the @c float-truncated result in @p *out. Using a double
 * accumulator improves accuracy for long series of small floats,
 * though the final cast can still lose precision when the result
 * exceeds @c FLT_MAX or has more than ~7 significant digits.
 *
 * The function is intentionally tolerant of empty series: an empty
 * series sums to 0 and the call still returns true. Compare with
 * plot_series_mean(), which rejects empty series to avoid division
 * by zero.
 *
 * @param plot  Plot to query. Must be non-NULL.
 * @param index Zero-based series index. Must be in range.
 * @param out   Receives the sum. Must be non-NULL.
 *
 * @return true on success (including the empty-series case where the
 *         sum is 0); false if @p plot or @p out is NULL or @p index
 *         is out of range.
 */
bool plot_series_sum(const Plot* plot, size_t index, float* out) {
    PLOT_LOG("[plot_series_sum]: enter (plot=%p, index=%zu)", (const void*)plot, index);
    if (!plot || !out || index >= plot->seriesCount) {
        PLOT_LOG("[plot_series_sum]: bad arg or OOR -> false");
        return false;
    }

    const PlotSeries* s = &plot->series[index];
    double acc = 0.0;
    for (size_t i = 0; i < s->dataSize; ++i) {
        acc += (double)s->yData[i];
    }
    *out = (float)acc;

    PLOT_LOG("[plot_series_sum]: exit -> true (sum=%g)", acc);
    return true;
}


/**
 * @brief Arithmetic mean of y values in the series at @p index.
 *
 * Computes the sum of the series in a @c double accumulator and
 * divides by the series length, then casts the result back to
 * @c float. Empty series are rejected (returning false) rather than
 * producing a NaN from a 0/0 division.
 *
 * @param plot  Plot to query. Must be non-NULL.
 * @param index Zero-based series index. Must be in range.
 * @param out   Receives the mean on success. Must be non-NULL.
 *
 * @return true on success; false if @p plot or @p out is NULL,
 *         @p index is out of range, or the series is empty.
 *
 * @code
 *     float avg;
 *     if (plot_series_mean(plot, 0, &avg)) {
 *         printf("series 0 averages %g\n", (double)avg);
 *     }
 * @endcode
 */
bool plot_series_mean(const Plot* plot, size_t index, float* out) {
    PLOT_LOG("[plot_series_mean]: enter (plot=%p, index=%zu)", (const void*)plot, index);
    if (!plot || !out || index >= plot->seriesCount) {
        PLOT_LOG("[plot_series_mean]: bad arg or OOR -> false");
        return false;
    }

    const PlotSeries* s = &plot->series[index];
    if (s->dataSize == 0) {
        PLOT_LOG("[plot_series_mean]: empty series -> false");
        return false;
    }

    double acc = 0.0;
    for (size_t i = 0; i < s->dataSize; ++i) {
        acc += (double)s->yData[i];
    }
    *out = (float)(acc / (double)s->dataSize);

    PLOT_LOG("[plot_series_mean]: exit -> true (mean=%g)", acc / (double)s->dataSize);
    return true;
}


/**
 * @brief Fill @p out with @p n linearly spaced values between @p start
 *        and @p end (inclusive at both ends).
 *
 * Equivalent to NumPy's `linspace(start, end, n)`: the resulting array
 * has @c out[0] == start and @c out[n-1] == end (modulo float
 * rounding), with the remaining @p n-2 values evenly distributed
 * between them. Special-cased:
 *   - @p n == 0   -> rejected, returns false.
 *   - @p n == 1   -> only writes @c out[0] = start.
 *   - @p n >= 2   -> uses step `(end - start) / (n - 1)`, evaluated in
 *                    @c double for accuracy, then cast back to float.
 *
 * Handy for constructing the x array of a synthetic plot:
 * @code
 *     float x[200], y[200];
 *     plot_linspace(0.0f, 6.283f, 200, x);
 *     for (size_t i = 0; i < 200; ++i) {
 *         y[i] = sinf(x[i]);
 *     }
 *     plot_add_series(plot, x, y, 200, "sin(x)",
 *                     PLTYPE_LINE, plot_color_rgb(0, 102, 204));
 * @endcode
 *
 * @param start First value to write.
 * @param end   Last value to write (when @p n >= 2).
 * @param n     Number of points to emit.
 * @param out   Destination buffer, must point to at least @p n floats.
 *              Must be non-NULL.
 *
 * @return true on success; false if @p out is NULL or @p n is 0.
 *
 * @note When @p start > @p end the step is negative and the values
 *       descend — the function does not swap or sort.
 */
bool plot_linspace(float start, float end, size_t n, float* out) {
    PLOT_LOG("[plot_linspace]: enter (start=%g, end=%g, n=%zu)", (double)start, (double)end, n);
    if (!out || n == 0) {
        PLOT_LOG("[plot_linspace]: bad arg -> false");
        return false;
    }
    if (n == 1) {
        out[0] = start;
        PLOT_LOG("[plot_linspace]: exit -> true (n==1, single value)");
        return true;
    }

    double step = ((double)end - (double)start) / (double)(n - 1);
    for (size_t i = 0; i < n; ++i) {
        out[i] = (float)((double)start + step * (double)i);
    }

    PLOT_LOG("[plot_linspace]: exit -> true (step=%g)", step);
    return true;
}


/**
 * @brief Dump every series of the plot to @p filename in CSV form.
 *
 * Writes a CSV file with one row per data point and one column per
 * series. The first column is named @c x and is built as follows:
 *   - if the plot's first series has an explicit @c xData array, the
 *     value at row r is taken from `series[0].xData[r]`;
 *   - otherwise the integer row index r is written (matching the
 *     implicit-index convention used by the renderer).
 *
 * Subsequent columns are the @c yData values of each series in
 * insertion order; the column header is the series' label, or
 * `series_N` (where N is the index) when the label is NULL or empty.
 *
 * If series have different lengths, the file uses the longest series
 * as the row count and leaves shorter series' missing cells empty
 * ("ragged CSV"). All numeric values are formatted with `%g` for
 * compactness.
 *
 * Suitable for hand-off to spreadsheets, Pandas, or any other
 * downstream tool that wants the raw data the plot is showing.
 *
 * @param plot     Source plot. Must be non-NULL and contain at least
 *                 one series.
 * @param filename Output path. Must be non-NULL. The file is opened
 *                 with `fopen(filename, "w")` and any previous
 *                 contents are overwritten.
 *
 * @return true on success; false on NULL plot/filename, empty plot
 *         (no series), or if the file could not be opened for
 *         writing.
 *
 * @code
 *     if (!plot_export_csv(plot, "out.csv")) {
 *         perror("plot_export_csv");
 *     }
 * @endcode
 */
bool plot_export_csv(const Plot* plot, const char* filename) {
    PLOT_LOG("[plot_export_csv]: enter (plot=%p, filename=%s)", (const void*)plot, filename ? filename : "(null)");
    if (!plot || !filename) {
        PLOT_LOG("[plot_export_csv]: bad arg -> false");
        return false;
    }
    if (plot->seriesCount == 0) {
        PLOT_LOG("[plot_export_csv]: no data -> false");
        return false;
    }

    FILE* f = fopen(filename, "w");
    if (!f) {
        PLOT_LOG("[plot_export_csv]: cannot open %s -> false", filename);
        return false;
    }

    /* Length of the longest series == number of rows. */
    size_t rows = 0;
    for (size_t s = 0; s < plot->seriesCount; ++s) {
        if (plot->series[s].dataSize > rows) {
            rows = plot->series[s].dataSize;
        }
    }

    /* Header. */
    fprintf(f, "x");
    for (size_t s = 0; s < plot->seriesCount; ++s) {
        const char* lbl = plot->series[s].label;
        if (lbl && *lbl) {
            fprintf(f, ",%s", lbl);
        }
        else {            
            fprintf(f, ",series_%zu", s);
        }
    }
    fputc('\n', f);

    /* Rows. */
    for (size_t r = 0; r < rows; ++r) {
        /* x column uses implicit index unless the first series has its own. */
        const PlotSeries* first = &plot->series[0];
        if (first->xData && r < first->dataSize) {
            fprintf(f, "%g", (double)first->xData[r]);
        } 
        else {
            fprintf(f, "%zu", r);
        }
        for (size_t s = 0; s < plot->seriesCount; ++s) {
            const PlotSeries* ser = &plot->series[s];
            if (r < ser->dataSize) {
                fprintf(f, ",%g", (double)ser->yData[r]);
            }
            else {                  
                fputc(',', f);
            }
        }
        fputc('\n', f);
    }

    fclose(f);
    PLOT_LOG("[plot_export_csv]: exit -> true (rows=%zu, file=%s)", rows, filename);
    return true;
}


/**
 * @brief Set the pixel sizes used for the title and the axis labels.
 *
 * Overrides the defaults (title 28 px, axis labels 20 px). A non-positive
 * value leaves that size unchanged, so you can adjust just one.
 *
 * @param plot      The plot. NULL is a no-op.
 * @param titleSize New title font size in pixels (<= 0 keeps the current value).
 * @param labelSize New axis-label font size in pixels (<= 0 keeps the current value).
 */
void plot_set_font_size(Plot* plot, int titleSize, int labelSize) {
    if (!plot) {
        return;
    }
    if (titleSize > 0) {
        plot->titleFontSize = titleSize;
    }
    if (labelSize > 0) {
        plot->labelFontSize = labelSize;
    }
    PLOT_LOG("[plot_set_font_size]: title=%d label=%d", plot->titleFontSize, plot->labelFontSize);
}


/**
 * @brief Add a histogram series (convenience wrapper for `PLTYPE_HISTOGRAM`).
 *
 * The values are binned and drawn as a histogram. Equivalent to
 * `plot_add_series(plot, NULL, values, n, label, PLTYPE_HISTOGRAM, color)`.
 *
 * @param plot   The plot.
 * @param values The raw samples to bin.
 * @param n      Number of samples.
 * @param label  Series label (copied by the plot). May be NULL.
 * @param color  Series color.
 * @return The new series index, or -1 on failure (full plot / bad input / OOM).
 */
int plot_add_histogram(Plot* plot, const float* values, size_t n, const char* label, PlotColor color) {
    PLOT_LOG("[plot_add_histogram]: enter (plot=%p, n=%zu)", (void*)plot, n);
    return plot_add_series(plot, NULL, values, n, label, PLTYPE_HISTOGRAM, color);
}


/**
 * @brief Add a pie-chart series (convenience wrapper for `PLTYPE_PIE`).
 *
 * Each value becomes a slice sized in proportion to the total. Equivalent to
 * `plot_add_series(plot, NULL, values, n, label, PLTYPE_PIE, color)`.
 *
 * @param plot   The plot.
 * @param values Slice magnitudes (non-negative).
 * @param n      Number of slices.
 * @param label  Series label (copied by the plot). May be NULL.
 * @param color  Base color.
 * @return The new series index, or -1 on failure.
 */
int plot_add_pie(Plot* plot, const float* values, size_t n, const char* label, PlotColor color) {
    PLOT_LOG("[plot_add_pie]: enter (plot=%p, n=%zu)", (void*)plot, n);
    return plot_add_series(plot, NULL, values, n, label, PLTYPE_PIE, color);
}


/**
 * @brief Get the plot's output dimensions, in pixels.
 *
 * The read-side counterpart of `plot_set_size`.
 *
 * @param plot   The plot. NULL is a no-op.
 * @param width  If non-NULL, receives the width.
 * @param height If non-NULL, receives the height.
 */
void plot_get_size(const Plot* plot, int* width, int* height) {
    if (!plot) {
        return;
    }
    if (width) {
        *width = plot->width;
    }
    if (height) {
        *height = plot->height;
    }
}


/**
 * @brief Population standard deviation of a series' y-values.
 *
 * Companion to `plot_series_mean` / `plot_series_sum`. Uses the population
 * formula (divides by N).
 *
 * @param plot  The plot.
 * @param index Series index.
 * @param out   Receives the standard deviation. Must not be NULL.
 * @return `true` on success, `false` on bad arguments, an out-of-range index,
 *         or an empty series.
 */
bool plot_series_stddev(const Plot* plot, size_t index, float* out) {
    PLOT_LOG("[plot_series_stddev]: enter (plot=%p, index=%zu)", (const void*)plot, index);
    if (!plot || !out || index >= plot->seriesCount) {
        return false;
    }
    const PlotSeries* s = &plot->series[index];
    if (s->dataSize == 0) {
        return false;
    }
    double mean = 0.0;
    for (size_t i = 0; i < s->dataSize; ++i) {
        mean += (double)s->yData[i];
    }
    mean /= (double)s->dataSize;

    double var = 0.0;
    for (size_t i = 0; i < s->dataSize; ++i) {
        double d = (double)s->yData[i] - mean;
        var += d * d;
    }
    var /= (double)s->dataSize;

    *out = (float)sqrt(var);
    PLOT_LOG("[plot_series_stddev]: exit -> true (stddev=%g)", sqrt(var));
    return true;
}


/* =================================================================== */
/* Axis range control                                                  */
/* =================================================================== */

/**
 * @brief Pin the x-axis to a fixed [min, max] range.
 *
 * By default each axis auto-scales to the data on every render. Pinning a
 * range keeps the scale constant — essential for dashboards that compare
 * snapshots over time, for pinning a bar chart's baseline to 0, or for
 * zooming/clipping. The range affects every subsequent `plot_draw` and
 * `plot_export_image`. Data points outside the range are mapped outside the
 * plot area (and clipped by the renderer). Call `plot_autoscale` to revert.
 *
 * @param plot Plot to configure. Must be non-NULL.
 * @param min  Lower bound of the x-axis.
 * @param max  Upper bound of the x-axis. Must be strictly greater than @p min.
 * @return true if the range was applied; false if @p plot is NULL or the
 *         range is degenerate (`min >= max`, or either bound is NaN).
 */
bool plot_set_xlim(Plot* plot, float min, float max) {
    PLOT_LOG("[plot_set_xlim]: enter (min=%g, max=%g)", (double)min, (double)max);
    if (!plot || !(min < max)) {   /* `!(min < max)` also rejects NaN bounds */
        PLOT_LOG("[plot_set_xlim]: bad arg or degenerate range -> false");
        return false;
    }
    plot->xMinManual   = min;
    plot->xMaxManual   = max;
    plot->xRangeManual = true;
    return true;
}


/**
 * @brief Pin the y-axis to a fixed [min, max] range.
 *
 * The y-axis counterpart to `plot_set_xlim`; see that function for the full
 * contract. The two axes are independent — you may pin one and leave the
 * other auto-scaling.
 *
 * @param plot Plot to configure. Must be non-NULL.
 * @param min  Lower bound of the y-axis.
 * @param max  Upper bound of the y-axis. Must be strictly greater than @p min.
 * @return true if the range was applied; false if @p plot is NULL or the
 *         range is degenerate (`min >= max`, or either bound is NaN).
 */
bool plot_set_ylim(Plot* plot, float min, float max) {
    PLOT_LOG("[plot_set_ylim]: enter (min=%g, max=%g)", (double)min, (double)max);
    if (!plot || !(min < max)) {
        PLOT_LOG("[plot_set_ylim]: bad arg or degenerate range -> false");
        return false;
    }
    plot->yMinManual   = min;
    plot->yMaxManual   = max;
    plot->yRangeManual = true;
    return true;
}


/**
 * @brief Revert both axes to automatic scaling.
 *
 * Clears any range previously pinned with `plot_set_xlim` / `plot_set_ylim`,
 * so the next render auto-scales to the data again. NULL is a no-op.
 *
 * @param plot Plot to reset. May be NULL.
 */
void plot_autoscale(Plot* plot) {
    PLOT_LOG("[plot_autoscale]: enter (plot=%p)", (void*)plot);
    if (!plot) {
        return;
    }
    plot->xRangeManual = false;
    plot->yRangeManual = false;
}


/**
 * @brief Read back the effective x-axis range that rendering will use.
 *
 * Reports the pinned range if `plot_set_xlim` was called, otherwise the range
 * auto-computed from the series data (with the same flat-data widening the
 * renderer applies). Lets you inspect or persist the scale without a graphics
 * context.
 *
 * @param plot   Plot to query. Must be non-NULL.
 * @param outMin Receives the lower bound. Must be non-NULL.
 * @param outMax Receives the upper bound. Must be non-NULL.
 * @return true on success; false if any argument is NULL, or if the axis has
 *         no pinned range and the plot holds no data to scale from.
 */
bool plot_get_xlim(const Plot* plot, float* outMin, float* outMax) {
    PLOT_LOG("[plot_get_xlim]: enter (plot=%p)", (const void*)plot);
    if (!plot || !outMin || !outMax) {
        return false;
    }
    float xmn, xmx, ymn, ymx;
    compute_extents(plot, &xmn, &xmx, &ymn, &ymx);
    if (xmn > xmx) {   /* no manual range and no data to auto-scale from */
        return false;
    }
    *outMin = xmn;
    *outMax = xmx;
    return true;
}


/**
 * @brief Read back the effective y-axis range that rendering will use.
 *
 * The y-axis counterpart to `plot_get_xlim`; see that function for the full
 * contract.
 *
 * @param plot   Plot to query. Must be non-NULL.
 * @param outMin Receives the lower bound. Must be non-NULL.
 * @param outMax Receives the upper bound. Must be non-NULL.
 * @return true on success; false if any argument is NULL, or if the axis has
 *         no pinned range and the plot holds no data to scale from.
 */
bool plot_get_ylim(const Plot* plot, float* outMin, float* outMax) {
    PLOT_LOG("[plot_get_ylim]: enter (plot=%p)", (const void*)plot);
    if (!plot || !outMin || !outMax) {
        return false;
    }
    float xmn, xmx, ymn, ymx;
    compute_extents(plot, &xmn, &xmx, &ymn, &ymx);
    if (ymn > ymx) {
        return false;
    }
    *outMin = ymn;
    *outMax = ymx;
    return true;
}
