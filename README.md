# Usage Guide

Start by selecting `Use this template` > `Create a new Repository` and setup a repository. Next clone the repository, and replace the information at the start of the `CMakeLists.txt` file.

```cmake
project(Amethyst-Template CXX ASM_NASM) # Replace `Amethyst-Template` with the name of your mod
set(MOD_VERSION "0.1.0") # Replace this with the version number of your mod
set(MOD_AUTHOR "FrederoxDev") # Replace this with your name
```

To build this, you need to have also build AmethystAPI seperately
