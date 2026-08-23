# Raylib-DS

![alt text](imgs/1.png) 
Graphics programming for the ds has never gotten easier!  
Raylib-DS is a port of raylib to the nintendo ds, taking advantage of modern libraries along with the DS's 3d engine!  
It currently is in a very early state but there will be ongoing updates!  

## Example 
Here is all the code you need to get something showing on the ds!
```
#include "rcore_ds.h"
#include "rshapes_ds.h"

int main(void){
    InitWindow(256,192,"my first program!");
    printf("Hello world!\n");
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground((Color){0,0,0});
        DrawRectangle(100,100,20,20,(Color){255,0,0});
        EndDrawing();
    }
    return 0;

}
```
![alt text](imgs/2.png)


## Compatibility
Currently I am trying to get as much of raylib to work as possible, but some things the ds just can't do, so here is a list of everything that has been added and stuff that may be added in the future!

### Already added
* File handling, input and other initialisation/drawing/misc functions in rcore
* Lines,triangles,rectangles,circles,elipses and polygons as well as their special versions in rshapes
* Texture loading (only compatible with p6 formatted ppm files for now) and animated textures
* Audio loading (16 channels for .wav files, 1 channel per song including stereo audio for now) and ability to play 1 mp3 file as a background track

### Planned compatibility
* All of the shapes in rshapes
* Allowance for more imag/audio file types to be used
* rtext for text drawing on the top screen
* More freedom for the user to use other libraries on the bottom screen

### Unlikely compatibility
* Rendertextures
* Camera2d/3d
* rmodels
* vr stuff and mouse


## How to install

First you would need to install [Blocksds](https://blocksds.skylyrac.net/docs/setup/) and then install [Palib](https://codeberg.org/SkyLyrac/palib) for the audio library

if you dont have these aliases then add them using 
``` 
export BLOCKSDS=/opt/wonderful/thirdparty/blocksds/core
export BLOCKSDSEXT=/opt/wonderful/thirdparty/blocksds/external 
```
since the makefiles rely on it

Then just open up your projects' directory, use the template makefile in the repo, and type make `make`!

# License
This project is licensed under the zlib/libpng license
