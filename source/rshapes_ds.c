#include "../../../../../../msys64/opt/wonderful/thirdparty/blocksds/core/libs/libnds/include/gl2d.h"
#include "../../../../../../msys64/opt/wonderful/thirdparty/blocksds/core/libs/libnds/include/nds/arm9/video.h"
#include "rcore_ds.h"
#include <nds.h>
#include "rshapes_ds.h"
#include <math.h>
#include <float.h>



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
    //DrawCircleSector((Vector2){ (float)centerX, (float)centerY }, radius, 0, 360, 36, color);
    int x = 0;
    int y = (int)(radius);
    int d = 3 - 2 * (int)(radius);

    while (x <= y)
    {

        DrawLine(centerX - x, centerY + y,centerX + x, centerY + y,color);
        DrawLine(centerX - x, centerY - y, centerX + x, centerY - y,color);
        DrawLine(centerX - y, centerY + x, centerX + y, centerY + x,color);
        DrawLine(centerX -y , centerY - x, centerX + y,centerY - x,color);

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
    s32 radiusF32 = floattof32(radius);
    for (int i = 0; i < 360; i += 10)
    {
        int x1 = (int)center.x + f32toint(mulf32(cosLerp(degreesToAngle(i + 10)),radiusF32));
        int x2 = (int)center.x + f32toint(mulf32(cosLerp(degreesToAngle(i)),radiusF32));
        int y1 = (int)center.y + f32toint(mulf32(sinLerp(degreesToAngle(i + 10)),radiusF32));
        int y2 = (int)center.y + f32toint(mulf32(sinLerp(degreesToAngle(i)),radiusF32));
        glTriangleFilledGradient(center.x,center.y,x1,y1,x2,y2,RGB15(inner.r >> 3, inner.g >> 3, inner.b >> 3),RGB15(outer.r >> 3, outer.g >> 3, outer.b >> 3),RGB15(outer.r >> 3, outer.g >> 3, outer.b >> 3));
    }
}

void DrawCircleSectorLines(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color); // Draw circle sector outline*/



void DrawEllipseV(Vector2 center, float radiusH, float radiusV, Color color)
{
    if (radiusH <= 0.0f || radiusV <= 0.0f) return;

    int hh = (int)radiusV * (int)radiusV;
    int ww = (int)radiusH * (int)radiusH;
    int hhww = hh * ww;
    int x0 = (int)radiusH;
    int dx = 0;

    u16 col = RGB15(color.r >> 3, color.g >> 3, color.b >> 3);
    int cx = (int)center.x;
    int cy = (int)center.y;

    glBoxFilled(cx - (int)radiusH, cy, cx + (int)radiusH, cy + 1, col);

    for (int y = 1; y <= (int)radiusV; y++)
    {
        int x1 = x0 - (dx - 1);
        for ( ; x1 > 0; x1--)
            if (x1*x1*hh + y*y*ww <= hhww)
                break;
        dx = x0 - x1;
        x0 = x1;

        if (x0 > 0)
        {
            glBoxFilled(cx - x0, cy - y, cx + x0, cy - y + 1, col);
            glBoxFilled(cx - x0, cy + y, cx + x0, cy + y + 1, col);
        }
        else
        {
            DrawPixel(cx, cy - y, color);
            DrawPixel(cx, cy + y, color);
        }
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
void drawEllipsePoints(int cx,int cy, int x, int y, Color color)
{
    DrawPixel(cx + x, cy + y, color);
    DrawPixel(cx - x, cy + y, color);
    DrawPixel(cx + x, cy - y, color);
    DrawPixel(cx - x, cy - y, color);
}
void DrawEllipseLinesV(Vector2 center, float radiusH, float radiusV, Color color)
{
    //https://dai.fmph.uniba.sk/upload/0/01/Ellipse.pdf
    int x = (int)radiusH;
    int y = 0;
    int xChange, yChange, ellipseError, twoASquare,twoBSquare,stoppingX,stoppingY;
    twoASquare = 2 * (int)radiusH * (int)(radiusH);
    twoBSquare = 2 * (int)radiusV * (int)radiusV;

    xChange = (int)radiusV * (int)radiusV * (1 - 2 * (int)radiusH);
    yChange = (int)radiusH * (int)radiusH;
    ellipseError = 0;
    stoppingX = twoBSquare * radiusH;
    stoppingY = 0;

    while (stoppingX >= stoppingY)
    {
        drawEllipsePoints((int)center.x,(int)center.y,x,y,color);
        y++;
        stoppingY += twoASquare;
        ellipseError += yChange;
        yChange += twoASquare;

        if ((2 * ellipseError + xChange) > 0)
        {
            x--;
            stoppingX -= twoBSquare;
            ellipseError += xChange;
            xChange += twoBSquare;
        }
    }
    x = 0;
    y = (int)radiusV;
    xChange = (int)radiusV * (int)radiusV;
    yChange = (int)radiusH * (int)radiusH * (1 - 2 * (int)radiusV);
    ellipseError = 0;
    stoppingX =  0;
    stoppingY = twoASquare * radiusV;

    while (stoppingX <= stoppingY)
    {
        drawEllipsePoints((int)center.x,(int)center.y,x,y,color);
        x++;
        stoppingX += twoBSquare;
        ellipseError += xChange;
        xChange += twoBSquare;
        if ((2 * ellipseError + yChange) > 0)
        {
            y--;
            stoppingY -= twoASquare;
            ellipseError += yChange;
            yChange += twoASquare;
        }
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

//todo finish shapes

//todo deal with floats later -- best to just use aabb for now
bool CheckCollisionPointRec(Vector2 point, Rectangle rec)
{
    bool collision = false;

    if ((point.x >= rec.x) && (point.x < (rec.x + rec.width)) && (point.y >= rec.y) && (point.y < (rec.y + rec.height))) collision = true;

    return collision;
}

// Check if point is inside circle
bool CheckCollisionPointCircle(Vector2 point, Vector2 center, float radius)
{
    bool collision = false;

    float distanceSquared = (point.x - center.x)*(point.x - center.x) + (point.y - center.y)*(point.y - center.y);

    if (distanceSquared <= radius*radius) collision = true;

    return collision;
}

// Check if point is inside a triangle defined by three points (p1, p2, p3)
bool CheckCollisionPointTriangle(Vector2 point, Vector2 p1, Vector2 p2, Vector2 p3)
{
    bool collision = false;

    float alpha = ((p2.y - p3.y)*(point.x - p3.x) + (p3.x - p2.x)*(point.y - p3.y)) /
                  ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));

    float beta = ((p3.y - p1.y)*(point.x - p3.x) + (p1.x - p3.x)*(point.y - p3.y)) /
                 ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));

    float gamma = 1.0f - alpha - beta;

    if ((alpha > 0) && (beta > 0) && (gamma > 0)) collision = true;

    return collision;
}


// Check if point is within a polygon described by array of vertices
// NOTE: Based on http://jeffreythompson.org/collision-detection/poly-point.php
bool CheckCollisionPointPoly(Vector2 point, const Vector2 *points, int pointCount)
{
    bool collision = false;

    if (pointCount > 2)
    {
        for (int i = 0, j = pointCount - 1; i < pointCount; j = i++)
        {
            if ((points[i].y > point.y) != (points[j].y > point.y) &&
                (point.x < (points[j].x - points[i].x)*(point.y - points[i].y)/(points[j].y - points[i].y) + points[i].x))
            {
                collision = !collision;
            }
        }
    }

    return collision;
}

// Check collision between two rectangles
bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2)
{
    bool collision = false;

    if ((rec1.x < (rec2.x + rec2.width) && (rec1.x + rec1.width) > rec2.x) &&
        (rec1.y < (rec2.y + rec2.height) && (rec1.y + rec1.height) > rec2.y)) collision = true;

    return collision;
}

// Check collision between two circles
bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
{
    bool collision = false;

    float dx = center2.x - center1.x;      // X distance between centers
    float dy = center2.y - center1.y;      // Y distance between centers

    float distanceSquared = dx*dx + dy*dy; // Distance between centers squared
    float radiusSum = radius1 + radius2;

    collision = (distanceSquared <= (radiusSum*radiusSum));

    return collision;
}

// Check collision between circle and rectangle
// NOTE: Reviewed version to take into account corner limit case
bool CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec)
{
    bool collision = false;

    float recCenterX = rec.x + rec.width/2.0f;
    float recCenterY = rec.y + rec.height/2.0f;

    float dx = fabsf(center.x - recCenterX);
    float dy = fabsf(center.y - recCenterY);

    if ((dx <= (rec.width/2.0f + radius)) && (dy <= (rec.height/2.0f + radius)))
    {
        if (dx <= (rec.width/2.0f)) collision = true;
        else if (dy <= (rec.height/2.0f)) collision = true;
        else
        {
            float cornerDistanceSq = (dx - rec.width/2.0f)*(dx - rec.width/2.0f) +
                (dy - rec.height/2.0f)*(dy - rec.height/2.0f);

            collision = (cornerDistanceSq <= (radius*radius));
        }
    }

    return collision;
}


bool CheckCollisionLines(Vector2 startPos1, Vector2 endPos1, Vector2 startPos2, Vector2 endPos2, Vector2 *collisionPoint)
{
    bool collision = false;

    float rx = endPos1.x - startPos1.x;
    float ry = endPos1.y - startPos1.y;
    float sx = endPos2.x - startPos2.x;
    float sy = endPos2.y - startPos2.y;

    float div = rx*sy - ry*sx;

    if (fabsf(div) >= FLT_EPSILON)
    {
        float s12x = startPos2.x - startPos1.x;
        float s12y = startPos2.y - startPos1.y;

        float t = (s12x*sy - s12y*sx)/div;
        float u = (s12x*ry - s12y*rx)/div;

        if ((0.0f <= t) && (t <= 1.0f) && (0.0f <= u) && (u <= 1.0f))
        {
            if (collisionPoint)
            {
                collisionPoint->x = startPos1.x + t*rx;
                collisionPoint->y = startPos1.y + t*ry;
            }

            collision = true;
        }
    }

    return collision;
}

// Check if point belongs to line created between two points [p1] and [p2] with defined margin in pixels [threshold]
bool CheckCollisionPointLine(Vector2 point, Vector2 p1, Vector2 p2, int threshold)
{
    bool collision = false;

    float dxc = point.x - p1.x;
    float dyc = point.y - p1.y;
    float dxl = p2.x - p1.x;
    float dyl = p2.y - p1.y;
    float cross = dxc*dyl - dyc*dxl;

    if (fabsf(cross) < (threshold*fmaxf(fabsf(dxl), fabsf(dyl))))
    {
        if (fabsf(dxl) >= fabsf(dyl)) collision = (dxl > 0)? ((p1.x <= point.x) && (point.x <= p2.x)) : ((p2.x <= point.x) && (point.x <= p1.x));
        else collision = (dyl > 0)? ((p1.y <= point.y) && (point.y <= p2.y)) : ((p2.y <= point.y) && (point.y <= p1.y));
    }

    return collision;
}

// Check if circle collides with a line created between two points [p1] and [p2]
bool CheckCollisionCircleLine(Vector2 center, float radius, Vector2 p1, Vector2 p2)
{
    bool collision = false;

    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;

    if ((fabsf(dx) + fabsf(dy)) <= FLT_EPSILON)
    {
        collision = CheckCollisionCircles(p1, 0, center, radius);
    }
    else
    {
        float lengthSQ = ((dx*dx) + (dy*dy));
        float dotProduct = (((center.x - p1.x)*(p2.x - p1.x)) + ((center.y - p1.y)*(p2.y - p1.y)))/(lengthSQ);

        if (dotProduct > 1.0f) dotProduct = 1.0f;
        else if (dotProduct < 0.0f) dotProduct = 0.0f;

        float dx2 = (p1.x - (dotProduct*(dx))) - center.x;
        float dy2 = (p1.y - (dotProduct*(dy))) - center.y;
        float distanceSQ = ((dx2*dx2) + (dy2*dy2));

        if (distanceSQ <= radius*radius) collision = true;
    }

    return collision;
}

// Get collision rectangle for two rectangles collision
Rectangle GetCollisionRec(Rectangle rec1, Rectangle rec2)
{
    Rectangle overlap = { 0 };

    float left = (rec1.x > rec2.x)? rec1.x : rec2.x;
    float right1 = rec1.x + rec1.width;
    float right2 = rec2.x + rec2.width;
    float right = (right1 < right2)? right1 : right2;
    float top = (rec1.y > rec2.y)? rec1.y : rec2.y;
    float bottom1 = rec1.y + rec1.height;
    float bottom2 = rec2.y + rec2.height;
    float bottom = (bottom1 < bottom2)? bottom1 : bottom2;

    if ((left < right) && (top < bottom))
    {
        overlap.x = left;
        overlap.y = top;
        overlap.width = right - left;
        overlap.height = bottom - top;
    }

    return overlap;
}

//----------------------------------------------------------------------------------
// Module Internal Functions Definition
//----------------------------------------------------------------------------------

// Cubic easing in-out
// NOTE: Used by DrawLineBezier() only
static float EaseCubicInOut(float t, float b, float c, float d)
{
    float result = 0.0f;

    if ((t /= 0.5f*d) < 1) result = 0.5f*c*t*t*t + b;
    else
    {
        t -= 2;
        result = 0.5f*c*(t*t*t + 2.0f) + b;
    }

    return result;
}