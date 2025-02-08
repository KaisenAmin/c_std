#include "plot.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

static void findMinMax(Plot* plot, float* xMin, float* xMax, float* yMin, float* yMax) {
    if (!plot || !yMin || !yMax) {
        return;
    }

    *yMin = *yMax = plot->yData[0];

    for (size_t i = 1; i < plot->dataSize; i++) {
        if (plot->yData[i] < *yMin) *yMin = plot->yData[i];
        if (plot->yData[i] > *yMax) *yMax = plot->yData[i];
    }

    if (*yMax == *yMin) {
        *yMin -= 1.0;
        *yMax += 1.0;
    }
    if (plot->pltype == PLTYPE_PIE) {
        *xMin = *xMax = 0;  // Ignore x-axis for Pie
    } 
    else if (plot->xData) {
        *xMin = plot->xData[0];
        *xMax = plot->xData[0];

        for (size_t i = 1; i < plot->dataSize; i++) {
            if (plot->xData[i] < *xMin) 
                *xMin = plot->xData[i];
            if (plot->xData[i] > *xMax) 
                *xMax = plot->xData[i];
        }
    }
}

static void compute_moving_average(Plot* plot, size_t window) {
    if (!plot || !plot->yData || window == 0 || window > plot->dataSize) {
        PLOT_LOG("[compute_moving_average]: Invalid plot or window size.");
        return;
    }
    float* originalData = (float*)malloc(plot->dataSize * sizeof(float));
    if (!originalData) {
        PLOT_LOG("[compute_moving_average]: Memory allocation failed.");
        return;
    }
    memcpy(originalData, plot->yData, plot->dataSize * sizeof(float));

    float* smoothedData = (float*)malloc(plot->dataSize * sizeof(float));
    if (!smoothedData) {
        PLOT_LOG("[compute_moving_average]: Memory allocation failed.");
        free(originalData);
        return;
    }

    for (size_t i = 0; i < plot->dataSize; i++) {
        float sum = 0.0;
        size_t count = 0;
        for (size_t j = (i >= window - 1 ? i - window + 1 : 0); j <= i; j++) {
            sum += originalData[j];
            count++;
        }
        smoothedData[i] = sum / count;
    }
    memcpy(plot->yData, smoothedData, plot->dataSize * sizeof(float));

    for (size_t i = 0; i < plot->dataSize; i++) {
        PLOT_LOG("[DEBUG] Smoothed yData[%zu] = %.2f", i, plot->yData[i]);
    }

    free(originalData);
    free(smoothedData);

    PLOT_LOG("[compute_moving_average]: Successfully applied moving average.");
}

Plot* plot_create(size_t dataSize, const char* title, const char* xLable, const char* yLable) {
    if (dataSize == 0) {
        PLOT_LOG("[plot_create]: dataSize must be > 0");
        return NULL;
    }

    Plot* plot = (Plot*) malloc(sizeof(Plot));

    if (!plot) {
        PLOT_LOG("[plot_create]: Failed to allocate memory for plot.");
        return NULL;
    }

    plot->xData = (float*) malloc(sizeof(float) * dataSize);
    plot->yData = (float*) malloc(sizeof(float) * dataSize);

    if (!plot->xData || !plot->yData) {
        PLOT_LOG("[plot_create]: Failed to allocate memory for data arrays.");
        free(plot->xData);
        free(plot->yData);
        free(plot);
        plot = NULL;
        return NULL;
    }

    plot->dataSize = dataSize;
    plot->xLable = xLable? strdup(xLable): strdup("X Axis");
    plot->yLable = yLable? strdup(yLable): strdup("Y Axis");
    plot->title = title? strdup(title): strdup("Untitled Plot");
    plot->height = 600;
    plot->width = 800;
    plot->pltype = PLTYPE_LINE;

    PLOT_LOG("[plot_create]: Plot Initialization Successfully.");

    return plot;
}

void plot_destroy(Plot* plot) {
    if (plot) {
        free(plot->xData);
        free(plot->yData);
        free((char*)plot->xLable);  
        free((char*)plot->yLable);
        free((char*)plot->title);
        free(plot);

        PLOT_LOG("[plot_destroy]: Plot destroyed successfully.");
    }
}

void plot_set_data(Plot* plot, const float* xData, const float* yData) {
    if (!plot) {
        return;
    }

    if (plot->pltype == PLTYPE_HISTOGRAM) {
        plot->xData = NULL;  // Histograms don't require xData
    } 
    else if (plot->pltype != PLTYPE_PIE) {
        if (!xData) {
            PLOT_LOG("[plot_set_data]: xData is NULL but required for this plot type.");
            return;
        }
        memcpy(plot->xData, xData, plot->dataSize * sizeof(float));
    }

    if (yData) {
        memcpy(plot->yData, yData, plot->dataSize * sizeof(float));
    } 
    else {
        PLOT_LOG("[plot_set_data]: yData is NULL, cannot plot.");
        return;
    }
}

void plot_draw(Plot* plot) {
    if (!plot || (!plot->yData) || ((plot->pltype != PLTYPE_PIE && plot->pltype != PLTYPE_HISTOGRAM) && !plot->xData)) {
        PLOT_LOG("[plot_draw]: Invalid plot or data is NULL.");
        return;
    }
    
    const int padding = 50;
    InitWindow(plot->width, plot->height, plot->title);
    SetTargetFPS(60);

    float xMin = 0, xMax = 0, yMin = 0, yMax = 0;
    findMinMax(plot, &xMin, &xMax, &yMin, &yMax);

    float xRange = (xMax - xMin) ? (xMax - xMin) : 1;
    float yRange = (yMax - yMin) ? (yMax - yMin) : 1;
    Font font = GetFontDefault();  

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw Axes
        DrawLine(padding, plot->height - padding, plot->width - padding, plot->height - padding, BLACK);  // X-axis
        DrawLine(padding, plot->height - padding, padding, padding, BLACK);  // Y-axis

        // Draw X-Axis Label
        DrawText(plot->xLable, plot->width / 2 - MeasureText(plot->xLable, 20) / 2, plot->height - padding + 10, 20, DARKGRAY);

        // Draw Y-Axis Label (Vertical)
        Vector2 yLabelPos = {10, plot->height / 2 + MeasureText(plot->yLable, 20) / 2}; // Center Y label
        DrawTextPro(font, plot->yLable, yLabelPos, (Vector2){0, 0}, -90, 20, 2, DARKGRAY);

        // Draw Title
        DrawText(plot->title, plot->width / 2 - MeasureText(plot->title, 24) / 2, 10, 24, BLACK);

        if (plot->pltype == PLTYPE_LINE) {
            // Draw Line Chart
            for (size_t i = 0; i < plot->dataSize - 1; i++) {
                int x1 = padding + (int)(((plot->xData[i] - xMin) / xRange) * (plot->width - 2 * padding));
                int y1 = plot->height - padding - (int)(((plot->yData[i] - yMin) / yRange) * (plot->height - 2 * padding));
                int x2 = padding + (int)(((plot->xData[i + 1] - xMin) / xRange) * (plot->width - 2 * padding));
                int y2 = plot->height - padding - (int)(((plot->yData[i + 1] - yMin) / yRange) * (plot->height - 2 * padding));

                DrawLine(x1, y1, x2, y2, BLUE);
                DrawCircle(x1, y1, 4, RED); 
            }
        }
        else if (plot->pltype == PLTYPE_BAR) {
            // Draw Bar Chart
            int barWidth = (plot->width - 2 * padding) / plot->dataSize - 5;
            for (size_t i = 0; i < plot->dataSize; i++) {
                int x = padding + (int)(((plot->xData[i] - xMin) / xRange) * (plot->width - 2 * padding));
                int y = plot->height - padding - (int)(((plot->yData[i] - yMin) / yRange) * (plot->height - 2 * padding));
                int height = (plot->height - padding) - y;

                DrawRectangle(x - barWidth / 2, y, barWidth, height, BLUE);
                DrawRectangleLines(x - barWidth / 2, y, barWidth, height, BLACK);
            }
        }
        else if (plot->pltype == PLTYPE_SCATTER) {
            // Draw Scatter Plot
            for (size_t i = 0; i < plot->dataSize; i++) {
                int x = padding + (int)(((plot->xData[i] - xMin) / xRange) * (plot->width - 2 * padding));
                int y = plot->height - padding - (int)(((plot->yData[i] - yMin) / yRange) * (plot->height - 2 * padding));

                DrawCircle(x, y, 6, RED);  // Larger dots for scatter points
                DrawCircleLines(x, y, 6, BLACK);  // Outline for better visibility
            }
        }
        else if (plot->pltype == PLTYPE_PIE) {
            float total = 0;
            
            for (size_t i = 0; i < plot->dataSize; i++) {
                if (plot->yData[i] > 0) total += plot->yData[i];
            }
            
            // If no valid data, return early
            if (total <= 0) {
                PLOT_LOG("[PLTYPE_PIE]: No valid data to plot.");
                DrawText("No valid data for Pie Chart", plot->width / 2 - 100, plot->height / 2, 20, RED);
                return;
            }

            Vector2 center = { plot->width / 2, plot->height / 2 };
            float radius = (plot->height < plot->width ? plot->height : plot->width) / 3;

            // Predefined color palette (supports up to 20 slices)
            Color colors[] = { RED, GREEN, BLUE, ORANGE, PURPLE, YELLOW, PINK, DARKGRAY, LIME, BROWN,
                            MAROON, GOLD, SKYBLUE, VIOLET, MAGENTA, DARKBLUE, BEIGE, LIGHTGRAY, DARKGREEN };
            int numColors = sizeof(colors) / sizeof(colors[0]);
            float startAngle = 0;

            for (size_t i = 0; i < plot->dataSize; i++) {
                if (plot->yData[i] <= 0) {
                    continue;  // Skip invalid values
                }

                float sliceAngle = (plot->yData[i] / total) * 360.0f;
                if (sliceAngle < 0.1f) { 
                    continue;  // Avoid rendering very tiny slices
                }

                // Draw pie slice
                DrawCircleSector(center, radius, startAngle, startAngle + sliceAngle, 50, colors[i % numColors]);

                // Compute label position (offset from center)
                float midAngle = startAngle + sliceAngle / 2.0f;
                float labelX = center.x + cosf(DEG2RAD * midAngle) * (radius * 0.6f);
                float labelY = center.y + sinf(DEG2RAD * midAngle) * (radius * 0.6f);

                // Format percentage label
                char label[10];
                snprintf(label, sizeof(label), "%.1f%%", (plot->yData[i] / total) * 100);

                // Adjust label positioning
                int labelWidth = MeasureText(label, 16);
                DrawText(label, (int)(labelX - labelWidth / 2), (int)labelY, 16, BLACK);

                startAngle += sliceAngle;
            }

            // Draw Title at the top
            DrawText(plot->title, plot->width / 2 - MeasureText(plot->title, 24) / 2, 10, 24, BLACK);
        }
        else if (plot->pltype == PLTYPE_HISTOGRAM) {
            findMinMax(plot, NULL, NULL, &yMin, &yMax);
            
            size_t numBins = 10;  
            float binWidth = (yMax - yMin) / numBins;
            if (binWidth == 0) binWidth = 1e-6;  
            if (yMax == yMin) {
                yMin -= 1.0;  
                yMax += 1.0;
            }

            PLOT_LOG("[DEBUG] yMin: %.2f, yMax: %.2f", yMin, yMax);

            int binCounts[numBins];
            memset(binCounts, 0, sizeof(binCounts));
        
            // Assign values to bins
            for (size_t i = 0; i < plot->dataSize; i++) {
                int binIndex = (plot->yData[i] - yMin) / binWidth;
                if (binIndex < 0) binIndex = 0;
                if (binIndex >= numBins) binIndex = numBins - 1;
                binCounts[binIndex]++;
            }
        
            // Find the max 
            float maxCount = 0;
            for (size_t i = 0; i < numBins; i++) {
                if (binCounts[i] > maxCount) {
                    maxCount = binCounts[i];
                }
            }
        
            float barWidth = (plot->width - 2 * padding) / numBins;
        
            // Draw histogram bars
            for (size_t i = 0; i < numBins; i++) {
                float barHeight = (binCounts[i] / maxCount) * (plot->height - 150);
                int x = padding + i * barWidth;
                int y = plot->height - padding - barHeight;
        
                DrawRectangle(x, y, barWidth - 5, barHeight, BLUE);
                DrawRectangleLines(x, y, barWidth - 5, barHeight, BLACK);
            }
        }
        if (plot->pltype == PLTYPE_MOVING_AVERAGE) {
            static bool applied = false;
            if (!applied) {
                size_t windowSize = 3;
                compute_moving_average(plot, windowSize);
                applied = true;
            }
        
            for (size_t i = 0; i < plot->dataSize - 1; i++) {
                int x1 = padding + (int)(((plot->xData[i] - xMin) / xRange) * (plot->width - 2 * padding));
                int y1 = plot->height - padding - (int)(((plot->yData[i] - yMin) / yRange) * (plot->height - 2 * padding));
                int x2 = padding + (int)(((plot->xData[i + 1] - xMin) / xRange) * (plot->width - 2 * padding));
                int y2 = plot->height - padding - (int)(((plot->yData[i + 1] - yMin) / yRange) * (plot->height - 2 * padding));
        
                DrawLine(x1, y1, x2, y2, GREEN);
                DrawCircle(x1, y1, 4, RED);
            }
        }
        
        
        EndDrawing();
    }

    CloseWindow();
}

void plot_set_window_size(Plot* plot, int width, int height) {
    if (!plot) {
        PLOT_LOG("[plot_set_window_size]: plot is NULL");
        return;
    }
    if (width <= 200 || height <= 200) {
        PLOT_LOG("[plot_set_window_size]: Invalid window size. Minimum is 200x200.");
        return;
    }

    plot->height = height;
    plot->width = width;
    PLOT_LOG("[plot_set_window_size]: Window size set to %dx%d", width, height);
}   

void plot_set_plot_type(Plot* plot, PlotType pltype) {
    if (!plot) {
        PLOT_LOG("[plot_set_plot_type]: plot is NULL");
        return;
    }

    switch (pltype) {
        case PLTYPE_LINE:
            plot->pltype = PLTYPE_LINE;
            break;
        case PLTYPE_BAR:
            plot->pltype = PLTYPE_BAR;
            break;
        case PLTYPE_SCATTER:
            plot->pltype = PLTYPE_SCATTER;
            break;
        case PLTYPE_PIE:
            plot->pltype = PLTYPE_PIE;
            break;
        case PLTYPE_HISTOGRAM:
            plot->pltype = PLTYPE_HISTOGRAM;
            break;
        case PLTYPE_MOVING_AVERAGE:
            plot->pltype = PLTYPE_MOVING_AVERAGE;
            break;
        default:
            break;
    }
    PLOT_LOG("[plot_set_plot_type]: Pltype is %d", pltype);
}