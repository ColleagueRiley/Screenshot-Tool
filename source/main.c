#define FONTSTASH_IMPLEMENTATION
#define SI_ALLOCATOR_UNDEFINE


#include <assert.h>
#include <stb_image.h>
#include <RGFW.h>
#include <draw.h>
#include <xss.h>
#include <fonts.h>
#include <sili.h>

#include <unistd.h>

/*
TODO:

- screenshot active window
- screenshot selected rectangle
- show mouse
- select font already installed on the system

CLI

POST:

make sure I only link the parts of sili I actually need

debloat text renderer

version control using rlgl to support older opengl versions

save last use options
color options

- arg for light/dark mode
- arg for default options
- dark for highlight color
*/

bool rectCollidePoint(rect r, point p) {
    return (p.x >= r.x &&  p.x <= r.x + r.w && p.y >= r.y && p.y <= r.y + r.h);
}

typedef enum {
    none = 0,
    hovered,
    pressed,
} buttonStatus;

typedef struct button {
    char* text;
    rect r;
    buttonStatus s;
    bool toggle; /* for toggle buttons */
} button;

void updateButton(button* b, RGFW_Event e) {
    switch (e.type) {
        case RGFW_mouseButtonPressed:
            if (rectCollidePoint(b->r, (point){e.x, e.y})) {
                if (b->s != pressed)
                    b->toggle = !b->toggle;
                b->s = pressed;
            }
            break;
        case RGFW_mouseButtonReleased:
            if (b->s == pressed && rectCollidePoint(b->r, (point){e.x, e.y}))
                b->s = hovered;
            else
                b->s = none;
            break;
        case RGFW_mousePosChanged:
            if (rectCollidePoint(b->r, (point){e.x, e.y}))
                b->s = hovered;
            else
                b->s = none;
            break;
        default: break;
    }
}

int main(int argc, char** argv) {
    int i;
    unsigned char delay = 0; /* 0 - 60 delay time for screenshot */
    bool lightMode = false;

    const int buttonCount = 10;
    button buttons[] = { 
        /* region toggle buttons [only one can be on at at time] */
        {"Entire screen", 40, 20, 20, 20, 0, true},
        {"Active window", 40, 50, 20, 20},
        {"Select a region", 40, 80, 20, 20},
        /* option buttons */
        {"Capture cursor", 40, 140, 20, 20, 0, true},
        {"Capture window border", 40, 170, 20, 20, 0, true},
        /* delay buttons */
        {"0", 180, 20, 20, 20},
        {"_", 200, 20, 20, 20}, /* _ looks better than - */
        {"+", 220, 20, 20, 20},
        /* other */
        {"Cancel", 200, 178, 40, 20},
        {"OK", 250, 178, 40, 20}
    };

    FONScontext* ctx = NULL;

    RGFW_window*  win = RGFW_createWindowPointer("screenshot-tool", 0, 0, 300, 200, RGFW_NO_RESIZE);
    rect screenshot;
    unsigned int border_height;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-')
            switch (argv[i][1]) {
                case 'e':
                    buttons[0].toggle = !buttons[0].toggle;
                    break;
                case 'a':
                    buttons[1].toggle = !buttons[1].toggle;
                    break;
                case 's':
                    buttons[2].toggle = !buttons[2].toggle;
                    break;
                case 'c':
                    buttons[3].toggle = !buttons[3].toggle;
                    break;
                case 'b':
                    buttons[4].toggle = !buttons[4].toggle;
                    break;
                case 'd':
                    delay = si_cstr_to_i64(argv[i + 1]);
                    break;
                case 'l':
                    lightMode = true;
                    break;
                case 'n':
                    XWindowAttributes attrs;
                    XGetWindowAttributes(win->display, win->window, &attrs);

                    border_height = attrs.y;

                    screenshot = (rect){0, 0, attrs.width, attrs.height};

                    goto SCREENSHOT;
                case 'h':
                    printf( "toggle:\n"  
                            "   -e   Capture entire screen [on by default]\n"  
                            "   -a   Capture active window\n"   
                            "   -s   Capture selected region\n"   
                            "   -c   Capture cursor [on by default]\n"   
                            "   -b   Capture border [on by default]\n"
                            "   \n"
                            "other:\n"   
                            "   -d Delay [seconds]\n"   
                            "   -l Light mode\n"  
                            "   -n Screenshot now [no gui]\n"
                            "   -h Display this information\n"
                        );
                    return 0;
                default: break;
            }
    }
    
    if (si_path_exists("logo.png")) {
        int w, h, c;
        unsigned char* icon = stbi_load("logo.png", &w, &h, &c, 0);
        RGFW_window_setIcon(win, icon, w, h, c);
        free(icon);
    }
    else
        printf("Warning : logo.png not found\n");

    /* init rlgl for graphics */
    rlLoadExtensions((void*)RGFW_getProcAddress);
    rlglInit(win->w, win->h);

    color bg, alt, textColor;

    if (lightMode) {
        bg = (color){227, 227, 227, 255};
        alt = (color){252, 252, 252, 255};
        textColor = (color){0, 0, 0, 255};
    } else {
        bg = (color){34, 34, 34, 255};
        alt = (color){40, 40, 40, 255};
        textColor = (color){200, 200, 200, 255};
    }

    rlClearColor(bg.r, bg.g, bg.b, bg.a);

    /* center window */
    int* screenSize = RGFW_window_screenSize(win);
    
    win->x = (screenSize[0] + win->w) / 4;
    win->y = (screenSize[1] + win->h) / 8;

    /* init text renderer */
    ctx = glfonsCreate(500, 500, 1);
    
    int font_ttf_index;

    for (font_ttf_index = 0; font_ttf_index < (7 * 3) && !si_path_exists(font_ttfs[font_ttf_index]); font_ttf_index++);

    if (!si_path_exists(font_ttfs[font_ttf_index])) {
        printf("No font file found\n");
        return 1;
    }

    int font = fonsAddFont(ctx, "sans", font_ttfs[font_ttf_index]);
    
    XWindowAttributes attrs;
    XGetWindowAttributes(win->display, DefaultRootWindow(win->display), &attrs);
    
    XGetWindowAttributes(win->display, win->window, &attrs);

    border_height = attrs.y;

    screenshot = (rect){0, 0, attrs.width, attrs.height};

    bool running = true;

    while (running) {
        /*
            check/handle events until there are none left to handle
            
            this is to prevent any event lag     
        */

        while(RGFW_window_checkEvent(win) != NULL) {
            for (i = 0; i < buttonCount; i++) {
                updateButton(&buttons[i], win->event);
                
                if (buttons[i].s != pressed)
                    continue;

                if (si_between(i, 6, 7)) {
                    delay += (i == 6) ? -1 : 1;

                    delay = (delay == 61) ? 0 : (delay == 255) ? 60 : delay;
                }

                else if (i == 8) {
                    running = false;
                
                    break;
                }

                else if (i == 9)
                    goto SCREENSHOT;
                
                else if (si_between(i, 0, 2))
                    for (i = 0; i < 3; i++)
                        buttons[i].toggle = (buttons[i].s == pressed);
            }

            switch (win->event.type) {
                case RGFW_quit:
                    running = false;
                    break;

                default: break;
            }
        }

        /* draw headers */
        drawText("Region", (circle){30, 0, 20}, textColor, win, ctx, font);
        drawText("Options", (circle){30, 110, 20}, textColor, win, ctx, font);
        drawText("Delay", (circle){180, 0, 20}, textColor, win, ctx, font);

        buttons[5].text = si_u64_to_cstr(delay);

        /* draw buttons */
        for (i = 0; i < buttonCount; i++) {
            color col = alt;

            if (buttons[i].s == hovered && !buttons[i].toggle)
                col = (color){50, 50, 50, 255};
            else if (buttons[i].s == pressed || (buttons[i].toggle && i < 5))
                col = (color){185, 42, 162, 255};

            if (i < 5) {
                drawPolygon(buttons[i].r, 360, 
                            (i >= 3) ? col : alt,
                            win
                );

                if (i < 3) {
                    rect r = buttons[i].r;
                    drawPolygon((rect){r.x + (r.w/4), r.y + (r.h/4), r.w / 2, r.h / 2}, 360, col, win);
                }
            }
            else
                drawRect(buttons[i].r, col, win);

            circle c = {buttons[i].r.x + (20 * (i < 5)) + 2, buttons[i].r.y + 5, 10};

            if (i == 6) 
                c = (circle){c.x + 4, c.y - 4, c.d}; /* adjust the _ */

            drawText(buttons[i].text, c, textColor, win, ctx, font);
        }

        rlClearScreenBuffers();
        rlDrawRenderBatchActive();

        RGFW_window_swapBuffers(win);
        
        rlClearColor(bg.r, bg.g, bg.b, bg.a);
    }


    if (false) { SCREENSHOT:
        XUnmapWindow(win->display, win->window);
        XFlush(win->display);   

        /* wait for window to close and for panel to hide */
        XEvent e;
        while (e.type != UnmapNotify)
            XNextEvent(win->display, &e);

        if (!delay)
            usleep(700000);
        else if (delay)
            sleep(delay);

        if (buttons[1].toggle) {
            int r;

            XGetInputFocus(win->display, &win->window, &r);

            assert(win->window != NULL);

            XWindowAttributes a;
            XGetWindowAttributes(win->display, win->window, &a);

            if (buttons[4].toggle)
                screenshot = (rect){a.x, a.y, a.width, a.height};
            else
                screenshot = (rect){a.x, a.y - border_height, a.width, a.height + border_height};
        }

        else if (buttons[2].toggle) {

        }

        XImage* img = XGetImage(win->display, DefaultRootWindow(win->display), 0, 0, screenshot.w, screenshot.h, AllPlanes, ZPixmap);
        
        time_t t = time(NULL);
        struct tm* now = localtime(&t);

        
        siString filename = si_string_make("Screenshot_");
        si_string_append(&filename, si_u64_to_cstr(now->tm_year + 1900));
        si_string_push(&filename, '_');
        si_string_append(&filename, si_u64_to_cstr(now->tm_mon + 1));
        si_string_push(&filename, '_');
        si_string_append(&filename, si_u64_to_cstr(now->tm_mday));
        si_string_push(&filename, '_');
        si_string_append(&filename, si_u64_to_cstr(now->tm_hour));
        si_string_push(&filename, '_');
        si_string_append(&filename, si_u64_to_cstr(now->tm_min));
        si_string_push(&filename, '_');
        si_string_append(&filename, si_u64_to_cstr(now->tm_sec));
        //siString filename = si_string_make_fmt("Screenshot_%d_%d_%d_%d_%d_%d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);


        unsigned int index = 0;

        while (si_path_exists(filename)) {
            index++;

            int eindex = si_string_len(filename) - 1;

            si_string_push(&filename, '_');

            char* indexStr = si_u64_to_cstr(index);
            si_string_append(&filename, indexStr);

            if (si_path_exists(filename))
                si_string_erase(&filename, eindex, si_string_len(indexStr) + 1);
        }
        
        si_string_append(&filename, ".png");

        screenshot_to_stream(img, filename, PNG, screenshot.x, screenshot.y, screenshot.w, screenshot.h);

        XFree(img);
        win->window = NULL;
    }
    
    if (ctx) {
        /* free any leftover data */
        fonsDeleteInternal(ctx);
        rlglClose();
    }

    RGFW_window_close(win);
}