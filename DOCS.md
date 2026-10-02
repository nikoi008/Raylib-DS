# Docs
>This is a section that talks about useful raylib-ds specific stuff that you can make the most of :D

## Vram allocation  
Currently the vram is solely allocated to the 3D engine for textures and palettes, however you can change these banks freely at any moment.  
Only banks H and I are used for the bottom screen console  
[This](https://mtheall.com/banks.html#A=TS0&B=TS1&C=TS2&D=TS3&E=TPAL&F=TPAL4&G=TPAL5&H=SBG0&I=SOBJ) website is great for seeing what you want to use vram for
>This in theory means that any bottom screen library can work alongside raylib ds, but I didn't test this yet  

## videoGL
Raylib DS uses gl2d and the videoGL to render things on the screen.  
This means you can also use those libraries if you want specific control.  
This is what has been done with the text engine, which is why text drawn always renders over sprites  
> I am planning to expose the depth buffer to prevent this  

## Animated Sprites  
This is very useful if you want to pack multiple frames of PO2 images in one texture, as it saves you from doing unneccessary padding which saves VRAM and RAM.  

If you have a 32x320 image of 10 32x32 frames, you can call `LoadImageAnim("test.png",10);` which you then follow with `LoadTextureAnimFromImage(image);`.  
> Alternatively you can load the image normally and just write the frames in image.frames

Then when drawing, you use DrawTextureAnim and specify the frame

### Limitations
The frames are split into individual textures, meaning that DrawTextureRec will not work.  
The textures are divided as height/frames, so make sure your width is a power of 2 and that it is a long strip


## Sound
Sound is the least stable part of my library, but its technically usable. It uses the aslib port from VNDS, which is why it uses wavpack instead of helix. Im planning to change it to helix when I can since .wv files are really big.  

Its relatively limited, since there are only 16 usable channels, and 1 is reserved for .wv playback.


## Filesystem
Raylib DS has 2 different filesystems; NitroFS and FAT

### NitroFS
NitroFS packs all your game assets into the rom, and is usually supported by most fractions. In order to store your files in nitrofs, just put them in the `nitrofs` folder and compile your program  
To access files in nitrofs you must prefix your directory with `nitro:/`
> Keep in mind that nitrofs is read only
### FAT
Raylib DS also allows you to store your files in your SD card and access them by prefixing with `fat:/`. Fat is more flexible, since you can read and write contents, though some flashcarts don't support it

