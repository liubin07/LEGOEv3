# ev3programs

These programs can operate LEGO ev3 by using ev3dev c++ API. To compile these source code, you need a ev3dev c++ API build environment. Please refer the following links for more details about ev3dev c++ API.

https://github.com/ddemidov/ev3dev-lang-cpp

## How to build
When you get ev3dev c++ API compiled, you can make a folder under ev3dev-lang-cpp(ex. myprogram), and put these c++ source code and CMakeLists.txt to this folder.

Then you can run the following build commands.

```
mkdir build
cd build
cmake .. -DEV3DEV_PLATFORM=EV3
make
```

At last, put the binaries to you ev3 device, have a fun.
