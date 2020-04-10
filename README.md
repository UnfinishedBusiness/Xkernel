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

# Hello World Program
```
function setup()
{
    console.log("This function is called once!\n");
}
function loop()
{
    console.log("This function is called every program cycle!\n);
}
```
Run Hello World Program by ./Xkernel hellow_world.js

# Windows Build Instructions
- Install MSYS2
- Open MSYS2 terminal and run "pacman -Syu". The terminal will ask you to close it after the update. Open it back up once it's updated.
- pacman -S mingw-w64-x86_64-toolchain
- pacman -S git
- pacman -S mingw64/mingw-w64-x86_64-glfw
- pacman -S mingw64/mingw-w64-x86_64-freeglut
- PATH=$PATH:/mingw64/bin
- mkdir /c/Xmotion
- cd /c/Xmotion
- git clone https://github.com/UnfinishedBusiness/Xkernel
- cd /c/Xmotion/Xkernel
- mingw32-make.exe

# Linux and Mac Build Instructions
- Use package manager to install c/c++ build essentials, glfw-dev, and glew-dev.
- mkdir $HOME/Xmotion
- cd $HOME/Xmotion
- git clone https://github.com/UnfinishedBusiness/Xkernel
- cd $HOME/Xmotion/Xkernel
- make

# Preamble
I own [BadApple Machine & Fab](https://badappleproducts.com), our main business is manufacturing light-duty entry-level CNC plasma cutters. My goal in business is to lower costs for these types of machines without compromising quality or needed features. All of our machines have Teknic AC brushless servos, Rack and Pinion drives, Leadscrew Z, Floating Head, AVTHC and Waterpan. I've a great deal of time creating this control package (The circuit board we use in our pruduction machines, the software, the firmware, etc) because while Mach3, LinuxCNC, Flashcut, and a handfull of controls that already exists work great, they are all expensive and fairly complicated. We used to use a LinuxCNC based control on our productions machines with Messa hardware which is a great way to go but expensive and fairly difficult for us to add new features (We're always listening to what customers would like to see), also expensive and time consuming to replace (as opposed to now switching out a refurbished laptop and installing software). By making this group of software open source and available to hobbyist around the world who are instested in using DIY delopment boards like the Arduino UNO (ATMega328P) to build their CNC motion control project, I hope I can help you get your machine up and running with less time and money spent. Feel free to submit pull requests, I'd appreciate any help I can get. Thanks
Checkout our other projects
- [JetCAD](https://jetcad.io) (2D CAD in the cloud, geared for plasma design, offline version in the works)
- [Xkernel](https://github.com/UnfinishedBusiness/Xkernel) (Powerfull Cross-Platform Javascript VM the runs code that looks and feels like Arduino code)
- [XmotionFirmware](https://github.com/UnfinishedBusiness/XmotionFirmware) (GRBL based motion control for CNC plasma cutting, builtin arc voltage torch height control)
- [ncPilot](https://github.com/UnfinishedBusiness/ncPilot) (Front-End for XmotionFirmware. Runs on just about anything hardware or operating system wise. Has built-in Gcode Viewer, Live DRO, Click and Go way points, ctrl-click viewer program jump-ins)
