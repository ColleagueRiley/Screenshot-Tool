#define FONTSTASH_IMPLEMENTATION
#define SI_ALLOCATOR_UNDEFINE

#define SI_PAIR_UNDEFINE
#define SI_OPTIONAL_UNDEFINE
#define SI_BUFFER_UNDEFINE
#define SI_THREAD_UNDEFINE
#define SI_BIT_UNDEFINE
#define SI_PERFORMANCE_UNDEFINE

#include <assert.h>
#include <stb_image.h>
#include <RGFW.h>
#include <draw.h>
#include <fonts.h>
#include <sili.h>
#include <stb_image_write.h>

#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/*
TODO:

- screenshot active window
- screenshot selected rectangle
- show mouse

CLI

POST:

debloat text renderer

version control using rlgl to support older opengl versions
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

color bg, highlightColor, alt, textColor; 

inline bool previewWindow(RGFW_window* win, unsigned char* arr, rect r, button* buttons, FONScontext* ctx, int font) {
    XMapWindow(win->display, (Window)win->window);

    int i;

    unsigned int texture = loadTexture(arr, r);

    buttons[9].toggle = false;
    buttons[9].s = none;

    while (true) {
        while (RGFW_window_checkEvent(win)) {
            if (win->event.type == RGFW_quit || win->event.keyCode == RGFW_Escape)
                return false;
        
            for (i = 8; i < 10; i++) {
                updateButton(&buttons[i], win->event);
        
                if (buttons[i].s == pressed && si_between(i, 8, 9))
                    return (i == 8) ? false : true;
            }
        }

        rlSetTexture(texture);
        drawRect((rect){5, 5, 290, 170}, (color){255, 255, 255, 255}, true, win);

        for (i = 8; i < 10; i++) {
            color col = alt;

            if (buttons[i].s == hovered)
                col = (color){50, 50, 50, 255};
            else if (buttons[i].s == pressed || (buttons[i].toggle && i < 5))
                col = highlightColor;

            drawRect(buttons[i].r, col, false, win);

            circle c = {buttons[i].r.x + (20 * (i < 5)) + 2, buttons[i].r.y + 5, 10};

            drawText(buttons[i].text, c, textColor, win, ctx, font);
        }

        rlClearScreenBuffers();
        rlDrawRenderBatchActive();

        RGFW_window_swapBuffers(win);
        
        rlClearColor(bg.r, bg.g, bg.b, bg.a);
    }
}

int main(int argc, char** argv) {
    int i;
    unsigned char delay = 0; /* 0 - 60 delay time for screenshot */
    bool lightMode = false, preview = true;

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

    RGFW_window*  win = RGFW_createWindowPointer("screenshot-tool", 0, 0, 300, 200, RGFW_NO_RESIZE);

    rect screenshot;
    unsigned int border_height;

    char* save_path = "";

    highlightColor = (color){185, 42, 162, 255};

    XWindowAttributes attrs;
    XGetWindowAttributes(win->display, XDefaultRootWindow(win->display), &attrs);

    screenshot = (rect){0, 0, attrs.width, attrs.height + border_height};

    XGetWindowAttributes(win->display, (Window)win->window, &attrs);
    border_height = attrs.y;

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
                case 'C': {
                    i++;
                    
                    if (i >= argc) {
                        printf("Missing hex value\n");
                        return 1;
                    }

                    int j;
                    for (j = 0; j < 6; j += 2) {
                        if (!argv[i][j] || !argv[i][j + 1]) {
                            printf("The hex code should have 6 digits\n");
                            return -1;
                        }

                        char hexstring[3] = {argv[i][j], argv[i][j + 1], '\0'};
                        
                        switch (j) {
                            case 0:
                                highlightColor.r = strtol(hexstring, NULL, 16);
                                break;
                            case 1:
                                highlightColor.g = strtol(hexstring, NULL, 16);
                                break;
                            default:
                                highlightColor.b = strtol(hexstring, NULL, 16);
                                break;
                        }
                    }
                    break;
                }
                case 'o':
                    i++;
                    
                    if (i >= argc || (i < argc && !si_path_exists(argv[i]))) {
                        printf("Missing path or path doesn't exist\n");
                        return 1;
                    }
                
                    save_path = argv[i];

                    break;
                case 'N':
                    preview = false;
                    break;
                case 'n':
                    preview = false;
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
                            "   -d :   Delay [seconds]\n"
                            "   -l :   Light mode\n"
                            "   -n :   Screenshot now [no gui]\n"
                            "   -N :   Skip preview window\n"
                            "   -C :   Set color to [hex code] (eg. -C F60033)\n"
                            "   -o :   Where to put the file [directory/path], defaults to `./` if nothing is added\n" 
                            "   -h :   Display this information\n"
                        );
                    return 0;
                default: break;
            }
    }
    
    char* icon_path = "logo.png";

    if (!si_path_exists(icon_path))
        icon_path = "/usr/share/icons/r-screenshot-tool/logo.png";

    if (si_path_exists(icon_path)) {
        int w, h, c;
        unsigned char* icon = stbi_load(icon_path, &w, &h, &c, 0);
        RGFW_window_setIcon(win, icon, w, h, c);
        free(icon);
    }
    else
        printf("Warning : logo.png not found\n");

    /* init rlgl for graphics */
    rlLoadExtensions((void*)RGFW_getProcAddress);
    rlglInit(win->w, win->h);

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
    FONScontext* ctx = glfonsCreate(500, 500, 1);
    
    int font_ttf_index;
    for (font_ttf_index = 0; font_ttf_index < (sizeof(font_ttfs)/sizeof(char*)) && !si_path_exists(font_ttfs[font_ttf_index]); font_ttf_index++);

    if (!si_path_exists(font_ttfs[font_ttf_index])) {
        printf("No font file found\n");
        return 1;
    }

    int font = fonsAddFont(ctx, "sans", font_ttfs[font_ttf_index]);
    
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
                case RGFW_keyPressed:
                    if (win->event.keyCode != RGFW_Escape)
                        break;
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

        buttons[5].text = (char*)si_u64_to_cstr(delay);

        /* draw buttons */
        for (i = 0; i < buttonCount; i++) {
            color col = alt;

            if (buttons[i].s == hovered && !buttons[i].toggle)
                col = (color){50, 50, 50, 255};
            else if (buttons[i].s == pressed || (buttons[i].toggle && i < 5))
                col = highlightColor;

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
                drawRect(buttons[i].r, col, false, win);

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
        XUnmapWindow(win->display, (Window)win->window);
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

            XGetInputFocus(win->display, (Window*)&win->window, &r);

            assert(win->window != NULL);

            XWindowAttributes a;
            XGetWindowAttributes(win->display, (Window)win->window, &a);

            if (buttons[4].toggle)
                screenshot = (rect){a.x, a.y - border_height, a.width, a.height + border_height};
            else 
                screenshot = (rect){a.x, a.y, a.width, a.height};
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

            const char* indexStr = si_u64_to_cstr(index);
            si_string_append(&filename, indexStr);

            if (si_path_exists(filename))
                si_string_erase(&filename, eindex, si_string_len(filename) + 1);
        }
        
        si_string_append(&filename, ".png");

        size_t save_path_len = strlen(save_path);

        if (save_path_len) {
            if (save_path[save_path_len - 1] != '/')
                si_string_insert(&filename, "/", 0);
            si_string_insert(&filename, save_path, 0);
        }
        
        /* convert XImage to Bitmap */
        unsigned char* bitmap = malloc(sizeof(unsigned char) * screenshot.w * screenshot.h * 3);
        
        unsigned int x, y;

        for(x = 0; x < screenshot.w; x++)
            for(y = 0; y < screenshot.h; y++) {
                unsigned long pixel = XGetPixel(img, screenshot.x + x, screenshot.y + y);

                bitmap[(x + screenshot.w * y) * 3] = ((pixel & img->red_mask) >> 16);
                bitmap[(x + screenshot.w * y) * 3 + 1] = ((pixel & img->green_mask) >> 8);
                bitmap[(x + screenshot.w * y) * 3 + 2] = (pixel & img->blue_mask);
            }

        XFree(img); /* free ximage */

        /* if preview is true, check if the user wants to save the image before saving it */
        if (!preview || (preview && previewWindow(win, bitmap, screenshot, buttons, ctx, font)))
            stbi_write_png(filename, screenshot.w, screenshot.h, 3, bitmap, screenshot.w * 3);
        
        free(bitmap); /* free bitmap */

        win->window = NULL;
    }

    /* free any leftover data */
    
    if (ctx) {
        fonsDeleteInternal(ctx);
        rlglClose();
    }

    RGFW_window_close(win);
}