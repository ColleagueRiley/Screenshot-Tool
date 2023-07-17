# Super-Lightweight-Screenshot-Tool
![alt text](https://github.com/ColleagueRiley/screenshot-tool/blob/main/logo.png?raw=true)
![alt text](https://github.com/ColleagueRiley/screenshot-tool/blob/main/Screenshot.png?raw=true)

![workflow](https://github.com/ColleagueRiley/Screenshot-Tool/actions/workflows/linux.yml/badge.svg)

Just as the name implies, it is a **SUPER** lightweight screenshot tool.
Not only is the code itself lightweight, but the libraries it relies on are lightweight too!

They're so lightweight infact, that they're all included in this project

# Dependencies
Other than the single header files, the screenshot tool requires you to have some native libraries installed.

Most of these libraries should already be installed on your system.

These include, libGL (opengl), libX11 (xlib) and libGLX (glX).

If you do not have any one of these installed, your package manager will have some package for it.

# fonts 
By default the program chooses the first font it can find from this list,

```
DejaVuSans.ttf
LiberationSans-Regular.ttf
DroidSans.ttf
Roboto-Regular.ttf
Ubuntu-Regular.ttf
NotoSans-Regular.ttf
NotoSansNerdFontPropo-Regular.ttf
```

in paths, ./, ~/.fonts and /usr/share/fonts/TTF


Thre's a decent chance one of these fonts is already be installed on your system .

## However, if you do not have any of these fonts installed you can either

A) Install a font package that includes one of these fonts

B) Find [a legal copy of] one of the fonts somewhere on the internet and manually install it

C) Edit the array in `include/fonts.h` to include a font you do have installed and [re]compile

If a font is not found, the program will crash it print the error

"No font file found"

# Icon / logo.png
If the icon file is not found at ./logo.png, the program checks if the icon file is at
/usr/share/icons/r-screenshot-tool/logo.png

running `sudo make install` automatically copies `logo.png` to `/usr/share/icons/r-screenshot-tool/logo.png` and `screenshot-tool` to `/usr/bin`

If the icon file is not found, the program prints the warning "Warning : logo.png not found"

Otherwise the program runs as normal, just without an icon.

# Documentation
### toggle  
`-e` :   Capture entire screen [on by default]\  
`-a` :   Capture active window\
`-s` :   Capture selected region\
`-c` :   Capture cursor [on by default]\
`-b` :   Capture border [on by default]

### other   
`-d` :   Delay [seconds]\
`-l` :   Light mode\
`-n` :   Screenshot now [no gui]\
`-N` :   Skip preview window\
`-C` :   Set color to [hex code] (eg. -C F60033)\
`-o` :   Where to put the file [directory/path], defaults to `./` if nothing is added\ 
`-h` :   Display this information

# Credits

# Sili
[Sili](https://github.com/EimaMei/sili-toolchain) is a single-header-file C toolchain library that aims at modernizing C without becoming bloatware nor infringing on C's elegance\
Written by [EimaMei](https://github.com/EimaMei)

# Stb
[Stb](https://github.com/nothings/stb/) is a very popular collection of single-header files. They're very lightweight, portable and convienet to use.

In this project I use `stb_image` `stb_truetype` and `stb_image_write`

# RLGL
[RLGL](https://github.com/raysan5/raylib/blob/master/src/rlgl.h) is a single header file OpenGL Abstaction layer\
It does version abstaction and allows modern OpenGL rendering to be done using the old pipeline system

It's made as a backend for [raylib](https://github.com/raysan5/raylib/) but can also be used as a standalone single-header-file!

# Fontstash
[Fontstash](https://github.com/memononen/fontstash) is an abstaction layer over `stb_truetype` that makes font rendering a lot easier. 

Fontstash itself doesn't render anything but instead requires a seperate graphics backend. 

For the backend, I use `rlglfontstash` which is my edited version of `glfontstash.h` to support rlgl.

Fontstash is written by [memononen](https://github.com/memononen), however the version I use\
has been slightly edited and optimized by me.

# RGFW
[RGFW](https://github.com/ColleagueRiley/RGFW) is a simple lightweight single-header abstaction layer over X11, WinAPI and Silicon created by me[!].\
It's meant to be an single-header and lightweight alternative for GLFW which uses an event queue rather than callbacks inorder to make it easier to work with.

It's mainly meant for creating GUI Libraries or simple GUI applications. 