# Xkernel
Javascript VM for Xmotion Software

# What is Xkernel?
Xkernel is a piece of software that allows me to create high-performance and cross-platform OpenGL applications with a simple
javascript scripting interface which lookgs and feels like Arduino Wiring Code. Xkernel is compiling in Linux, MacOS, and
Windows (Via MSYS2) and is very stable.

# Why not Node.js or Electron?
Node.js and Electron is great but I ran into significant performance issues and a handfull of bugs when using WebGL with electron.
Xkernel has bindings that allows greating a GLFW OpenGL window very easily and drawing inside of it is even easier. The idea is to
let Xkernel do all the heavy lifting on the C and C++ side of things rather then requiring (slower) javascript code to loop arrays
to render object. All the rendered entities are added via binding and are stored on the native side so that the javascript side
simply has to create the layout and check inputs and outputs (Create dialog windows, check for button presses, update rendered text, ext)

# Binding Classes
- console (console.log for stdout printing)
- dxf (parse and create dxf files. Uses DXF LIB which is included statically)
- file_dialog (used for creating a file open/save dialog and is cross-platform)
- file (used for file i/o and listing directorys)
- gcode (built in gcode parser)
- geometry (a class that does geometry calculations)
- gui (a class that handles all the gui stuff. Creates dialogs and widgets via IMGUI for simple cross-compatibility. draws dialogs inside OpenGL)
- http (a class that makes http calls accesable from Javascript)
- ini (a class that makes parsing and writing ini files easy)
- motion_controller (a class that houses the serial communication protocal for XmotionFirmware with auto-connect. Makes it easy to create CNC control interface like [ncPilot](https://github.com/UnfinishedBusiness/ncPilot)
- render (a class that makes rendering lines, arcs, rectangles and other geometry directly to the opengl viewer easy via javascript)
- serial (a class that makes serial communication easy via javascript. All the cross-platform stuff is handled at compile time of Xkernel)
- time (a class that makes getting timestamps and time measurement easy, also alows for easy arduino style millis() or micros() timers to be used in javascript)

# Windows Build Instruction
- Install MSYS2
- Open MSYS2 terminal and run "pacman -Syu". The terminal will ask you to close it after the update. Open it back up once it's updated.
- pacman -S mingw-w64-x86_64-toolchain
- pacman -S git
- pacman -S mingw64/mingw-w64-x86_64-glfw
- pacman -S mingw64/mingw-w64-x86_64-freeglut
- PATH=$PATH:/mingw64/bin
- cd /to/Xkernel/
- mingw32-make.exe
