/* 
    data structures 
*/

#include <stddef.h>
#include "rlgl.h"
#include "rglfontstash.h"

#include <math.h>

#ifndef DEG2RAD
    #define DEG2RAD (M_PI/180.0f)
#endif

typedef struct point { 
    int x, y;
} point;

typedef struct rect { 
    int x, y, w, h;
} rect;

typedef struct circle { 
    int x, y, d;
} circle;

typedef struct color { 
    unsigned char r, g, b, a; 
} color;

/*
    functions
*/

void glPrerequisites(RGFW_window* win, rect r, color c) {
    rlColor4ub(c.r, c.g, c.b, c.a);
    rlMatrixMode(RL_PROJECTION);
    rlLoadIdentity();
    rlPushMatrix();

    rlOrtho(0, win->w, win->h, 0, -win->w, win->w);

    rlMatrixMode(RL_MODELVIEW);
}

void drawRect(rect r, color c, RGFW_window* win) {
    rlSetTexture(1);

    glPrerequisites(win, r, c);   
        rlBegin(RL_QUADS); // start drawing the rect
            rlVertex2f(r.x, r.y);

            rlVertex2f(r.x, r.y + r.h);

            rlVertex2f(r.x + r.w, r.y + r.h);
                
            rlVertex2f(r.x + r.w, r.y);
        rlEnd();
    rlPopMatrix();

    rlSetTexture(0);
}


void drawPolygon(rect o, unsigned int sides, color col, RGFW_window* win) {
    int i;


    o.w /= 2;
    o.h /= 2;

    o.x += o.w;
    o.y += o.h;

    double t = 0;
    float centralAngle = 0;
    
    rlSetTexture(-1);
    glPrerequisites(win, o, col);
        rlBegin(RL_QUADS);
            for (i = 0; i < sides; i++) {
                    float rad = (360 / sides * i) * (M_PI / 180.0);
                    float tx = (float)cos(rad) * 0.5 + 0.5;
                    float ty = (float)sin(rad) * 0.5 + 0.5;
                    rlTexCoord2f(0.5, 0.5);
                    rlColor4ub(col.r, col.g, col.b, col.a);
                    rlVertex2f(o.x, o.y);

                    rlTexCoord2f(ty, 0);
                    rlColor4ub(col.r, col.g, col.b, col.a);
                    rlVertex2f(o.x + sinf(DEG2RAD * centralAngle) * o.w, o.y + cosf(DEG2RAD * centralAngle) * o.h);

                    centralAngle += 360.0f/(float)sides;

                    rlTexCoord2f(ty, tx);
                    rlColor4ub(col.r, col.g, col.b, col.a);
                    rlVertex2f(o.x + sinf(DEG2RAD * centralAngle) * o.w, o.y + cosf(DEG2RAD * centralAngle) * o.h);
            }
        rlEnd();
    rlPopMatrix();

    rlSetTexture(0);
}

void drawText(char* text, circle c, color col, RGFW_window* win, FONScontext* ctx, int fonsFont) {
    if (text == NULL || text[0] == '\0')
        return;

    int w = fonsTextBounds(ctx, c.x, c.y, text, NULL, NULL);

    fonsClearState(ctx);

    fonsSetSize(ctx, c.d);
    fonsSetFont(ctx, fonsFont);

    glPrerequisites(win, (rect) {c.x, c.y + (c.d - (c.d/4)), w, c.d}, col);

    fonsSetColor(ctx, glfonsRGBA(col.r, col.b, col.g, col.a));

    fonsDrawText(ctx, c.x, c.y + (c.d - (c.d / 4)), text, NULL);

    rlPopMatrix();

    fonsSetSpacing(ctx, 0);
}