#include "../../../../../../msys64/opt/wonderful/thirdparty/blocksds/core/libs/libnds/include/gl2d.h"
#include "../../../../../../msys64/opt/wonderful/thirdparty/blocksds/core/libs/libnds/include/nds/arm9/video.h"
#include "rcore_ds.h"
#include <nds.h>
#include "rshapes_ds.h"
#include <math.h>




void DrawPixel(int posX, int posY, Color color)
{
    glPutPixel(posX,posY, RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};
void DrawPixelV(Vector2 position, Color color)
{
    glPutPixel(position.x,position.y, RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};



void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color)
{
    glLine(startPosX,startPosY,endPosX,endPosY, RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};
void DrawLineV(Vector2 startPos, Vector2 endPos, Color color)
{
    glLine(startPos.x,startPos.y,endPos.x,endPos.y, RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};
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
}
void DrawLineStrip(const Vector2 *points, int pointCount, Color color)
{
    if (pointCount < 2) return; // Security check


    for (int i = 0; i < pointCount - 1; i++)
    {

        DrawLineV(points[i],points[i + 1],color);
    }
};
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
}



void DrawCircle(int centerX, int centerY, float radius, Color color)
{
    DrawCircleSector((Vector2){ (float)centerX, (float)centerY }, radius, 0, 360, 36, color);

}
void DrawCircleV(Vector2 center, float radius, Color color)
{
    DrawCircle(center.x,center.y,radius,color);
};


void DrawCircleSector(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color)
{
    if (startAngle == endAngle) return;
    if (radius <= 0.0f) radius = 0.1f;  // Avoid div by zero

    // Function expects (endAngle > startAngle)
    if (endAngle < startAngle)
    {
        // Swap values
        float tmp = startAngle;
        startAngle = endAngle;
        endAngle = tmp;
    }

    //int minSegments = (int)ceilf((endAngle - startAngle)/90);

    //f (segments < minSegments)
    //{
    //    // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
    //    float th = acosf(2*powf(1 - SMOOTH_CIRCLE_ERROR_RATE/radius, 2) - 1);
    //    segments = (int)ceilf((endAngle - startAngle)*(2*PI/th)/360.0f);

    //if (segments <= 0) segments = minSegments;
    //}

    float stepLength = (endAngle - startAngle)/(float)segments;
    float angle = startAngle;
    s32 radiusF32 = floattof32(radius);

        for (int i = 0; i < segments; i++)
        {
            //rlColor4ub(color.r, color.g, color.b, color.a);

            //rlVertex2f(center.x, center.y);
            //rlVertex2f(center.x + cosf(DEG2RAD*(angle + stepLength))*radius, center.y + sinf(DEG2RAD*(angle + stepLength))*radius);
            int x1 = (int)center.x + f32toint(mulf32(cosLerp(degreesToAngle(angle + stepLength)),radiusF32));
            int y1 = (int)center.y + f32toint(mulf32(sinLerp(degreesToAngle(angle + stepLength)),radiusF32));
            //rlVertex2f(center.x + cosf(DEG2RAD*angle)*radius, center.y + sinf(DEG2RAD*angle)*radius);
            int x2  = (int)center.x + f32toint(mulf32(cosLerp(degreesToAngle(angle)),radiusF32));
            int y2 = (int)center.y + f32toint(mulf32(sinLerp(degreesToAngle(angle)),radiusF32));

            glTriangleFilled((int)center.x,(int)center.y,x1,y1,x2,y2,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
            angle += stepLength;
        }
}
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
}
void DrawCircleLinesV(Vector2 center, float radius, Color color)
{
    DrawCircleLines(center.x,center.y,radius,color);
};
void DrawCircleGradient(Vector2 center, float radius, Color inner, Color outer)
{

    s32 radiusF32 = floattof32(radius); //todo dont vibecode

}
/*void DrawCircleSector(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color); // Draw a piece of a circle
void DrawCircleSectorLines(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color); // Draw circle sector outline*/



void DrawEllipseV(Vector2 center, float radiusH, float radiusV, Color color)
{
    s32 radiusHF32 = floattof32(radiusH);
    s32 radiusVF32 = floattof32(radiusV);


    for (int i = 0; i < 360; i += 10)
    {
        int x1 = (int)center.x + f32toint(mulf32(cosLerp(degreesToAngle(i + 10)),radiusHF32));
        int x2 = (int)center.x + f32toint(mulf32(cosLerp(degreesToAngle(i)),radiusHF32));
        int y1 = (int)center.y + f32toint(mulf32(sinLerp(degreesToAngle(i + 10)),radiusVF32));
        int y2 = (int)center.y + f32toint(mulf32(sinLerp(degreesToAngle(i)),radiusVF32));

        glTriangleFilled(center.x,center.y,x1,y1,x2,y2,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
    }
}
void DrawEllipse(int centerX,int centerY, float radiusH, float radiusV, Color color)
{
    DrawEllipseV((Vector2){centerX,centerY},radiusH,radiusV,color);
};
void DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, Color color)
{
    DrawEllipseV((Vector2){centerX,centerY},radiusH,radiusV,color);
};
void DrawEllipseLinesV(Vector2 center, float radiusH, float radiusV, Color color)
{
    s32 radiusHF32 = floattof32(radiusH);
    s32 radiusVF32 = floattof32(radiusV);

    for (int i = 0; i < 360; i += 10)
    {
        int x1 = (int)center.x + f32toint(mulf32(cosLerp(degreesToAngle(i + 10)),radiusHF32));
        int x2 = (int)center.x + f32toint(mulf32(cosLerp(degreesToAngle(i)),radiusHF32));
        int y1 = (int)center.y + f32toint(mulf32(sinLerp(degreesToAngle(i + 10)),radiusVF32));
        int y2 = (int)center.y + f32toint(mulf32(sinLerp(degreesToAngle(i)),radiusVF32));

        glLine(x1,y1,x2,y2,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
    }
};



void DrawRing(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color);
void DrawRingLines(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color);



void DrawRectangle(int posX, int posY, int width, int height, Color color)
{
    glBoxFilled(posX,posY,posX + width, posY + height,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};
void DrawRectangleV(Vector2 position, Vector2 size, Color color)
{
    glBoxFilled(position.x,position.y,position.x + size.x,position.y + size.y,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};
void DrawRectangleRec(Rectangle rec, Color color)
{
    glBoxFilled(rec.x,rec.y,rec.x + rec.width,rec.y + rec.height,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};
void DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color);
void DrawRectangleGradientV(int posX, int posY, int width, int height, Color top, Color bottom)
{
    int coltop = RGB15(top.r >> 3, top.g >> 3, top.b >> 3);
    int colbot = RGB15(bottom.r >> 3, bottom.g >> 3, bottom.b >> 3);
    glBoxFilledGradient(posX,posY,posX + width, posY + height,coltop,colbot,coltop,colbot);
};
void DrawRectangleGradientH(int posX, int posY, int width, int height, Color left, Color right)
{
    int coleft = RGB15(left.r >> 3, left.g >> 3, left.b >> 3);
    int colright = RGB15(right.r >> 3, right.g >> 3, right.b >> 3);
    glBoxFilledGradient(posX,posY,posX + width, posY + height,coleft,coleft,colright,colright);
};
void DrawRectangleGradientEx(Rectangle rec, Color topLeft, Color bottomLeft, Color bottomRight, Color topRight)
{
    int col1 = RGB15(topLeft.r >> 3, topLeft.g >> 3, topLeft.b >> 3);
    int col2 = RGB15(bottomLeft.r >> 3, bottomLeft.g >> 3, bottomLeft.b >> 3);
    int col3 = RGB15(bottomRight.r >> 3, bottomRight.g >> 3, bottomRight.b >> 3);;
    int col4 = RGB15(topRight.r >> 3, topRight.g >> 3, topRight.b >> 3);
    glBoxFilledGradient(rec.x,rec.y,rec.x + rec.width,rec.y + rec.height,col1,col2,col3,col4);
};
void DrawRectangleLines(int posX, int posY, int width, int height, Color color)
{
    glBox(posX,posY + height,posX + width,posY , RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};
void DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color)
{
    //TODO IMPLEMENT
};
void DrawRectangleRoundedLines(Rectangle rec, float roundness, int segments, Color color)
{
    DrawLine(rec.x + 1, rec.y, rec.x + rec.width - 1, rec.y, color);
    DrawLine(rec.x + rec.width, rec.y + 1, rec.x + rec.width, rec.y + rec.height - 1, color);
    DrawLine(rec.x + 1, rec.y + rec.height, rec.x + rec.width - 1, rec.y + rec.height, color);
    DrawLine(rec.x, rec.y + 1, rec.x, rec.y + rec.height - 1, color);
}
void DrawRectangleRoundedLinesEx(Rectangle rec, float roundness, int segments, float lineThick, Color color)
{
    //TODO IMPLEMENT
};



void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color)
{
    glTriangleFilled(v1.x,v1.y,v2.x,v2.y,v3.x,v3.y,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};
void DrawTriangleLines(Vector2 v1, Vector2 v2, Vector2 v3, Color color)
{
    glTriangle(v1.x,v1.y,v2.x,v2.y,v3.x,v3.y,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
};
void DrawTriangleFan(const Vector2 *points, int pointCount, Color color)
{
    if (pointCount < 3) return;

    for (int i = 1; i < pointCount - 1; i++)
    {

        glTriangleFilled((int)points[0].x, (int) points[0].y, (int)points[i].x, (int)points[i].y, (int)points[i + 1].x, (int)points[i + 1].y, RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
    }
}
void DrawTriangleStrip(const Vector2 *points, int pointCount, Color color)
{
    if (pointCount >= 3)
    {

        for (int i = 2; i < pointCount; i++)
        {
            if ((i%2) == 0)
            {
                glTriangleFilled((int)points[i].x, (int)points[i].y,(int)points[i - 2].x, (int)points[i - 2].y,(int)points[i - 1].x, (int)points[i - 1].y,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
            }
            else
            {
                glTriangleFilled((int)points[i].x,(int)points[i].y,(int)points[i - 1].x,(int)points[i - 1].y,(int)points[i - 2].x,(int)points[i - 2].y,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
            }
        }
    }
}



void DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color)
{
    if (sides < 3) sides = 3;
    s32 radiusF32 = floattof32(radius);
    s16 centralAngle = (s16)(((int)rotation * DEGREES_IN_CIRCLE) / 360);
    s16 angleStep = (s16)(DEGREES_IN_CIRCLE / sides);

    for (int i = 0; i < sides; i++)
    {
        int x1 = (int)(center.x) + f32toint(mulf32(cosLerp(centralAngle),radiusF32));
        int x2 = (int )(center.x) + f32toint(mulf32(cosLerp(centralAngle + angleStep),radiusF32));

        int y1 = (int)(center.y) + f32toint(mulf32(sinLerp(centralAngle),radiusF32));
        int y2 = (int)(center.y) + f32toint(mulf32(sinLerp(centralAngle + angleStep),radiusF32));
        glTriangleFilled(center.x,center.y,x1,y1,x2,y2,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
        centralAngle += angleStep;

    }
}

void DrawPolyLines(Vector2 center, int sides, float radius, float rotation, Color color)
{
    if (sides < 3) sides = 3;
    s32 radiusF32 = floattof32(radius);
    s16 centralAngle = (s16)(((int)rotation * DEGREES_IN_CIRCLE) / 360);
    s16 angleStep = (s16)(DEGREES_IN_CIRCLE / sides);

    for (int i = 0; i < sides; i++)
    {
        int x1 = (int)(center.x) + f32toint(mulf32(cosLerp(centralAngle),radiusF32));
        int x2 = (int )(center.x) + f32toint(mulf32(cosLerp(centralAngle + angleStep),radiusF32));

        int y1 = (int)(center.y) + f32toint(mulf32(sinLerp(centralAngle),radiusF32));
        int y2 = (int)(center.y) + f32toint(mulf32(sinLerp(centralAngle + angleStep),radiusF32));
        glLine(x1,y1,x2,y2,RGB15(color.r >> 3, color.g >> 3, color.b >> 3));
        centralAngle += angleStep;

    }
}
