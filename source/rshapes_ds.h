//
// Created by Nmsou on 04/08/2026.
//

#ifndef RAYLIB_DS_RSHAPES_DS_H
#define RAYLIB_DS_RSHAPES_DS_H
#include "rcore_ds.h"
typedef struct
{
    int x;
    int y;
    int width;
    int height;
}Rectangle;

typedef struct
{
    int a;
}Texture2D;

    // Basic shapes drawing functions
    void DrawPixel(int posX, int posY, Color color);                                                   // Draw a pixel using geometry [Can be slow, use with care]
    void DrawPixelV(Vector2 position, Color color);                                                    // Draw a pixel using geometry (Vector version) [Can be slow, use with care]
    void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color);                // Draw a line
    void DrawLineV(Vector2 startPos, Vector2 endPos, Color color);                                     // Draw a line (using gl lines)
    void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color);                       // Draw a line (using triangles/quads)
    void DrawLineStrip(const Vector2 *points, int pointCount, Color color);                            // Draw lines sequence (using gl lines)
    void DrawLineBezier(Vector2 startPos, Vector2 endPos, float thick, Color color);                   // Draw line segment cubic-bezier in-out interpolation
    void DrawLineDashed(Vector2 startPos, Vector2 endPos, int dashSize, int spaceSize, Color color);   // Draw a dashed line
    void DrawCircle(int centerX, int centerY, float radius, Color color);                              // Draw a color-filled circle
    void DrawCircleV(Vector2 center, float radius, Color color);                                       // Draw a color-filled circle (Vector version)
    void DrawCircleGradient(Vector2 center, float radius, Color inner, Color outer);                   // Draw a gradient-filled circle
    void DrawCircleSector(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color); // Draw a piece of a circle
    void DrawCircleSectorLines(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color); // Draw circle sector outline
    void DrawCircleLines(int centerX, int centerY, float radius, Color color);                         // Draw circle outline
    void DrawCircleLinesV(Vector2 center, float radius, Color color);                                  // Draw circle outline (Vector version)
    void DrawEllipse(int centerX, int centerY, float radiusH, float radiusV, Color color);             // Draw ellipse
    void DrawEllipseV(Vector2 center, float radiusH, float radiusV, Color color);                      // Draw ellipse (Vector version)
    void DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, Color color);        // Draw ellipse outline
    void DrawEllipseLinesV(Vector2 center, float radiusH, float radiusV, Color color);                 // Draw ellipse outline (Vector version)
    void DrawRing(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color); // Draw ring
    void DrawRingLines(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color); // Draw ring outline
    void DrawRectangle(int posX, int posY, int width, int height, Color color);                        // Draw a color-filled rectangle
    void DrawRectangleV(Vector2 position, Vector2 size, Color color);                                  // Draw a color-filled rectangle (Vector version)
    void DrawRectangleRec(Rectangle rec, Color color);                                                 // Draw a color-filled rectangle
    void DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color);                 // Draw a color-filled rectangle with pro parameters
    void DrawRectangleGradientV(int posX, int posY, int width, int height, Color top, Color bottom);   // Draw a vertical-gradient-filled rectangle
    void DrawRectangleGradientH(int posX, int posY, int width, int height, Color left, Color right);   // Draw a horizontal-gradient-filled rectangle
    void DrawRectangleGradientEx(Rectangle rec, Color topLeft, Color bottomLeft, Color bottomRight, Color topRight); //Draw a gradient-filled rectangle with custom vertex colors
    void DrawRectangleLines(int posX, int posY, int width, int height, Color color);
    void DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color);// Draw rectangle outline with extended parameters
    void DrawRectangleRounded(Rectangle rec, float roundness, int segments, Color color);              // Draw rectangle with rounded edges
    void DrawRectangleRoundedLines(Rectangle rec, float roundness, int segments, Color color);         // Draw rectangle lines with rounded edges
    void DrawRectangleRoundedLinesEx(Rectangle rec, float roundness, int segments, float lineThick, Color color); // Draw rectangle with rounded edges outline
    void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color);                                // Draw a color-filled triangle (vertex in counter-clockwise order!)
    void DrawTriangleLines(Vector2 v1, Vector2 v2, Vector2 v3, Color color);                           // Draw triangle outline (vertex in counter-clockwise order!)
    void DrawTriangleFan(const Vector2 *points, int pointCount, Color color);                          // Draw a triangle fan defined by points (first vertex is the center)
    void DrawTriangleStrip(const Vector2 *points, int pointCount, Color color);                        // Draw a triangle strip defined by points
    void DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color);               // Draw a regular polygon (Vector version)
    void DrawPolyLines(Vector2 center, int sides, float radius, float rotation, Color color);          // Draw a polygon outline of n sides
    void DrawPolyLinesEx(Vector2 center, int sides, float radius, float rotation, float lineThick, Color color); // Draw a polygon outline of n sides with extended parameters


#endif //RAYLIB_DS_RSHAPES_DS_H
