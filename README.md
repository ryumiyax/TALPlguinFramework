# TALPlguinFramework
A Template make it easier to create plugins for TaikoArcadeLoader

# How to use
Rename `plugin` from `CMakeList.txt`, `exports.def`, `build.yml` to your own plugin name

Read the comments section of each exported function patiently  
decide which functions you need to use  
and delete the functions you don't use from `dllmain.cpp` and `exports.def`

implement your own logic and make it by cmake or github actions  
enjoy developing

Do not forget write your own Readme for your project

## Build
```bash
# Load the MSVC environment (Change this to your actual vcvarsall.bat path)
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

# Configure the build folder (this is only needed the first time)
cmake -B build -S . -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release

# Build TaikoArcadeLoader
cmake --build build --config Release --target plugin
```