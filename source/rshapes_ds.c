#include "../../../../../../msys64/opt/wonderful/thirdparty/blocksds/core/libs/libnds/include/gl2d.h"
#include "../../../../../../msys64/opt/wonderful/thirdparty/blocksds/core/libs/libnds/include/nds/arm9/video.h"
#include "rcore_ds.h"
#include <nds.h>
#include "rshapes_ds.h"
#include <math.h>


// Created by Nmsou on 04/08/2026.
//
//static Texture2D texShapes = { 1, 1, 1, 1, 7 };                // Texture used on shapes drawing (white pixel loaded by rlgl)
static Rectangle texShapesRec = { 0.0f, 0.0f, 1.0f, 1.0f };

void DrawPixel(int posX, int posY, Color color)
{
    glPutPixel(posX,posY, RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};                                                   // Draw a pixel using geometry [Can be slow, use with care]
void DrawPixelV(Vector2 position, Color color)
{
    glPutPixel(position.x,position.y, RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};                                                    // Draw a pixel using geometry (Vector version) [Can be slow, use with care]
void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color)
{
    glLine(startPosX,startPosY,endPosX,endPosY, RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};                // Draw a line
void DrawLineV(Vector2 startPos, Vector2 endPos, Color color)
{
    glLine(startPos.x,startPos.y,endPos.x,endPos.y, RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};                                     // Draw a line (using gl lines)
void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color)
{
        //todo dont vibecode
    s32 x0 = floattof32(startPos.x);
    s32 y0 = floattof32(startPos.y);
    s32 x1 = floattof32(endPos.x);
    s32 y1 = floattof32(endPos.y);
    s32 halfThick = floattof32(thick * 0.5f);

    s32 dx = x1 - x0;
    s32 dy = y1 - y0;

    s32 lenSq = mulf32(dx, dx) + mulf32(dy, dy);
    //if (lenSq == 0) return;

    s32 len = sqrtf32(lenSq);

    s32 nx = -divf32(mulf32(dy, halfThick), len);
    s32 ny =  divf32(mulf32(dx, halfThick), len);

    s32 ax = x0 + nx, ay = y0 + ny;
    s32 bx = x0 - nx, by = y0 - ny;
    s32 cx = x1 - nx, cy = y1 - ny;
    s32 ex = x1 + nx, ey = y1 + ny;

    u16 col = RGB15(color.r >> 3, color.g >> 3, color.b >> 3);

    glTriangleFilled(f32toint(ax), f32toint(ay), f32toint(bx), f32toint(by), f32toint(cx), f32toint(cy), col);
    glTriangleFilled(f32toint(ax), f32toint(ay), f32toint(cx), f32toint(cy), f32toint(ex), f32toint(ey), col);
}          // Draw a line
void DrawLineStrip(const Vector2 *points, int pointCount, Color color)
{
    if (pointCount < 2) return; // Security check


    for (int i = 0; i < pointCount - 1; i++)
    {

        DrawLineV(points[i],points[i + i],color);
    }
};                            // Draw lines sequence (using gl lines)

void DrawLineDashed(Vector2 startPos, Vector2 endPos, int dashSize, int spaceSize, Color color)
{
    float dx = endPos.x - startPos.x;
    float dy = endPos.y - startPos.y;
    float totalLen = sqrtf(dx*dx + dy*dy);

    if (totalLen <= 0.0f) return;

    float dirX = dx / totalLen;
    float dirY = dy / totalLen;

    int patternLen = dashSize + spaceSize;
    if (patternLen <= 0) return;

    float traveled = 0.0f;

    while (traveled < totalLen)
    {
        float dashEnd = traveled + (float)dashSize;
        if (dashEnd > totalLen) dashEnd = totalLen;

        Vector2 segStart = {
            startPos.x + dirX * traveled,
            startPos.y + dirY * traveled
        };
        Vector2 segEnd = {
            startPos.x + dirX * dashEnd,
            startPos.y + dirY * dashEnd
        };

        DrawLineV(segStart, segEnd, color);

        traveled += (float)patternLen;
    }
}// Draw a dashed line
void DrawCircle(int centerX, int centerY, float radius, Color color)
{
    //todo dont vibecode
    s32 radiusF32 = floattof32(radius);
    s32 radiusSq = mulf32(radiusF32, radiusF32);
    int r = f32toint(radiusF32);

    u16 col = RGB15(color.r >> 3, color.g >> 3, color.b >> 3);

    for (int dy = -r; dy <= r; dy++)
    {
        s32 dyF = inttof32(dy);
        s32 dySq = mulf32(dyF, dyF);
        s32 halfWSq = radiusSq - dySq;
        if (halfWSq < 0) halfWSq = 0;

        int halfW = f32toint(sqrtf32(halfWSq));

        int y = centerY + dy;
        glBoxFilled(centerX - halfW, y, centerX + halfW, y + 1, col);
    }
}
void DrawCircleV(Vector2 center, float radius, Color color)
{
    DrawCircle(center.x,center.y,radius,color);
};
/*

void DrawCircleSector(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color); // Draw a piece of a circle
void DrawCircleSectorLines(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color); // Draw circle sector outline*/
void DrawCircleLines(int centerX, int centerY, float radius, Color color)
{
    int x = 0;
    int y = (int)(radius);
    int d = 3 - 2 * (int)(radius);

    while (x <= y)
    {
        DrawPixel(centerX + x, centerY + y,color);
        DrawPixel(centerX - x, centerY + y,color);
        DrawPixel(centerX + x, centerY - y,color);
        DrawPixel(centerX - x, centerY - y,color);
        DrawPixel(centerX + y, centerY + x,color);
        DrawPixel(centerX - y, centerY + x,color);
        DrawPixel(centerX + y, centerY - x,color);
        DrawPixel(centerX - y, centerY - x,color);

        x++;
        if(d< 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            y = y - 1;
            d = d + 4 * (x - y) + 10;
        }
    }
} // Draw circle outline
void DrawCircleLinesV(Vector2 center, float radius, Color color)
{
    DrawCircleLines(center.x,center.y,radius,color);
};                                // Draw circle outline (Vector version)

void DrawCircleGradient(Vector2 center, float radius, Color inner, Color outer)
{

    s32 radiusF32 = floattof32(radius); //

    u16 innerCol = RGB15(inner.r >> 3, inner.g >> 3, inner.b >> 3);
    u16 outerCol = RGB15(outer.r >> 3, outer.g >> 3, outer.b >> 3);

    s16 prevAngle = 0;
    int prevX = (int)center.x + f32toint(mulf32(radiusF32, cosLerp(prevAngle)));
    int prevY = (int)center.y + f32toint(mulf32(radiusF32, sinLerp(prevAngle)));

    for (int i = 1; i <= 36; i++)
    {
        s16 angle = (s16)((i * DEGREES_IN_CIRCLE) / 36);

        int x = (int)center.x + f32toint(mulf32(radiusF32, cosLerp(angle)));
        int y = (int)center.y + f32toint(mulf32(radiusF32, sinLerp(angle)));

        glTriangleFilledGradient((int)center.x, (int)center.y, prevX, prevY, x, y, innerCol, outerCol, outerCol);

        prevX = x;
        prevY = y;
    }
}


void DrawEllipseV(Vector2 center, float radiusH, float radiusV, Color color)
{
    const int segments = 36; // matches raylib's original 360/10 = 36 steps

    s32 radiusHF32 = floattof32(radiusH);
    s32 radiusVF32 = floattof32(radiusV);
    int cx = (int)center.x;
    int cy = (int)center.y;

    u16 col = RGB15(color.r >> 3, color.g >> 3, color.b >> 3);

    s16 prevAngle = 0;
    int prevX = cx + f32toint(mulf32(radiusHF32, cosLerp(prevAngle)));
    int prevY = cy + f32toint(mulf32(radiusVF32, sinLerp(prevAngle)));

    for (int i = 1; i <= segments; i++)
    {
        s16 angle = (s16)((i * DEGREES_IN_CIRCLE) / segments);

        int x = cx + f32toint(mulf32(radiusHF32, cosLerp(angle)));
        int y = cy + f32toint(mulf32(radiusVF32, sinLerp(angle)));

        glTriangleFilled(cx, cy, prevX, prevY, x, y, col);

        prevX = x;
        prevY = y;
    }
}
// Draw ellipse
void DrawEllipse(int centerX,int centerY, float radiusH, float radiusV, Color color)
{
    DrawEllipseV((Vector2){centerX,centerY},radiusH,radiusV,color);
};                      // Draw ellipse (Vector version)
void DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, Color color);// Draw ellipse outline
void DrawEllipseLinesV(Vector2 center, float radiusH, float radiusV, Color color);                 // Draw ellipse outline (Vector version)
void DrawRing(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color); // Draw ring
void DrawRingLines(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color); // Draw ring outline


void DrawRectangle(int posX, int posY, int width, int height, Color color)
{
    glBoxFilled(posX,posY,posX + width, posY + height,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};                        // Draw a color-filled rectangle
void DrawRectangleV(Vector2 position, Vector2 size, Color color)
{
    glBoxFilled(position.x,position.y,position.x + size.x,position.y + size.y,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};                                  // Draw a color-filled rectangle (Vector version)
void DrawRectangleRec(Rectangle rec, Color color)
{
    glBoxFilled(rec.x,rec.y,rec.x + rec.width,rec.y + rec.height,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};                                                 // Draw a color-filled rectangle
void DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color);                 // Draw a color-filled rectangle with pro parameters
void DrawRectangleGradientV(int posX, int posY, int width, int height, Color top, Color bottom)
{
    int coltop = RGB15(top.r >> 3, top.g >> 3, top.b >> 3);
    int colbot = RGB15(bottom.r >> 3, bottom.g >> 3, bottom.b >> 3);
    glBoxFilledGradient(posX,posY,posX + width, posY + height,coltop,colbot,coltop,colbot);
};   // Draw a vertical-gradient-filled rectangle
void DrawRectangleGradientH(int posX, int posY, int width, int height, Color left, Color right)
{
    int coleft = RGB15(left.r >> 3, left.g >> 3, left.b >> 3);
    int colright = RGB15(right.r >> 3, right.g >> 3, right.b >> 3);
    glBoxFilledGradient(posX,posY,posX + width, posY + height,coleft,coleft,colright,colright);
};   // Draw a horizontal-gradient-filled rectangle
void DrawRectangleGradientEx(Rectangle rec, Color topLeft, Color bottomLeft, Color bottomRight, Color topRight)
{
    int col1 = RGB15(topLeft.r >> 3, topLeft.g >> 3, topLeft.b >> 3);
    int col2 = RGB15(bottomLeft.r >> 3, bottomLeft.g >> 3, bottomLeft.b >> 3);
    int col3 = RGB15(bottomRight.r >> 3, bottomRight.g >> 3, bottomRight.b >> 3);;
    int col4 = RGB15(topRight.r >> 3, topRight.g >> 3, topRight.b >> 3);
    glBoxFilledGradient(rec.x,rec.y,rec.x + rec.width,rec.y + rec.height,col1,col2,col3,col4);
}; // Draw a gradient-filled rectangle with custom vertex colors
void DrawRectangleLines(int posX, int posY, int width, int height, Color color)
{
    glBox(posX,posY,posX + width,posY + height, RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};                   // Draw rectangle outline
void DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color)
{
    //TODO IMPLEMENT
};                       // Draw rectangle outline with extended parameters
void DrawRectangleRoundedLines(Rectangle rec, float roundness, int segments, Color color)
{
    DrawLine(rec.x + 1, rec.y, rec.x + rec.width - 1, rec.y, color);
    DrawLine(rec.x + rec.width, rec.y + 1, rec.x + rec.width, rec.y + rec.height - 1, color);
    DrawLine(rec.x + 1, rec.y + rec.height, rec.x + rec.width - 1, rec.y + rec.height, color);
    DrawLine(rec.x, rec.y + 1, rec.x, rec.y + rec.height - 1, color);
}        // Draw rectangle with rounded edges// Draw rectangle lines with rounded edges
void DrawRectangleRoundedLinesEx(Rectangle rec, float roundness, int segments, float lineThick, Color color)
{
    //TODO IMPLEMENT
}; // Draw rectangle with rounded edges outline
void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color)
{
    glTriangleFilled(v1.x,v1.y,v2.x,v2.y,v3.x,v3.y,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};                                // Draw a color-filled triangle (vertex in counter-clockwise order!)
void DrawTriangleLines(Vector2 v1, Vector2 v2, Vector2 v3, Color color)
{
    glTriangle(v1.x,v1.y,v2.x,v2.y,v3.x,v3.y,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};                           // Draw triangle outline (vertex in counter-clockwise order!)
void DrawTriangleFan(const Vector2 *points, int pointCount, Color color);                          // Draw a triangle fan defined by points (first vertex is the center)
void DrawTriangleStrip(const Vector2 *points, int pointCount, Color color);                        // Draw a triangle strip defined by points
void DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color);               // Draw a regular polygon (Vector version)
void DrawPolyLines(Vector2 center, int sides, float radius, float rotation, Color color);          // Draw a polygon outline of n sides
void DrawPolyLinesEx(Vector2 center, int sides, float radius, float rotation, float lineThick, Color color); // Draw a polygon outline of n sides with extended parameters
