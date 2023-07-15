# Super-Lightweight-Screenshot-Tool
![alt text](https://github.com/ColleagueRiley/screenshot-tool/blob/main/logo.png?raw=true)
![alt text](https://github.com/ColleagueRiley/screenshot-tool/blob/main/Screenshot.png?raw=true)

Just as the name implies, it is a **SUPER** lightweight screenshot tool.
Not only is the code itself lightweight, but the libraries it relies on are lightweight too!

They're so lightweight infact, that they're all included in this project

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
`-h` :   Display this information

# XSS
[XSS](https://github.com/ferhatgec/xss/tree/master) is a simple command line based screenshot tool created by [Ferhat Geçdoğan](https://github.com/ferhatgec/xss/tree/master) 
under the MIT license. 

I am using a motified version of a function included in the source code of xss. This code is in the header file `include/xss.h`