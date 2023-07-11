/* 
    sources for single headers files    

    this is to make compile time faster
    when debugging

    not that compile time is awful by default
*/


#ifdef _WIN32
#undef __unix__
#undef __linux__
#endif

#define RLGL_IMPLEMENTATION
#define RGFW_IMPLEMENTATION 
#define RGFW_NO_JOYSTICK_CODES
#define SI_IMPLEMENTATION 

#define GLFONTSTASH_IMPLEMENTATION
#define FONTSTASH_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define SI_ALLOCATOR_UNDEFINE


#include "rlgl.h" /* lightweight OpenGL abstaction layer */
#include "RGFW.h" /* lightweight GUI framework library */
#include "rglfontstash.h" /* font rendering library on top of stb_truetype */
#include "sili.h" /* lightweight toolchain library (strings, arrays, file I/O, ect) */