#include <gl2d.h>
#include <nds.h>
#include <stdarg.h>
typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;

typedef struct
{
    //todo fill
}Image;

typedef struct
{
 //todo fill
}FilePathList;
typedef struct
{
    bool pollEvents;
    u32 lastTicks;
    float frameTime;
    bool windowReady;
}dsCore;

typedef enum {
    LOG_ALL = 0,
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL,
    LOG_NONE
} TraceLogLevel;



void TRACELOG(int logType, const char *text, ...)
{
    
    va_list args;
    va_start(args, text);
    vprintf(text, args);
    va_end(args);

    printf("\n");
}


dsCore DS;
void InitWindow(int width, int height, const char* title)
{
    videoSetMode(MODE_0_3D); //https://mtheall.com/banks.html#A=TS0&B=TS1&C=TS2&D=TS3&E=TPAL&F=TPAL4&G=TPAL5&H=SBG0&I=SOBJ
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_TEXTURE_SLOT0);
    vramSetBankB(VRAM_B_TEXTURE_SLOT1);
    vramSetBankC(VRAM_C_TEXTURE_SLOT2);
    vramSetBankD(VRAM_D_TEXTURE_SLOT3);
    vramSetBankE(VRAM_E_TEX_PALETTE);
    vramSetBankF(VRAM_F_TEX_PALETTE_SLOT4);
    vramSetBankG(VRAM_G_TEX_PALETTE_SLOT5);
    vramSetBankH(VRAM_H_SUB_BG);
    vramSetBankI(VRAM_I_SUB_SPRITE);

    consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
    glScreen2D();
    cpuStartTiming(0);
    DS.lastTicks = cpuGetTiming();
    consoleDemoInit();
    DS.lastTicks = 0;

    DS.windowReady = true;
}

void CloseWindow(void)
{
    //? i guess unload everything
}

bool WindowShouldClose(void)
{
    //?

}

// here for compatibility -- best to not use
bool IsWindowReady(void){ return DS.windowReady; }                                   // Check if window has been initialized successfully
bool IsWindowFullscreen(void){ return true; }                              // Check if window is currently fullscreen
bool IsWindowHidden(void){ return false; }                                  // Check if window is currently hidden
bool IsWindowMinimized(void){ return false; }                           // Check if window is currently minimized
bool IsWindowMaximized(void) { return true; }                               // Check if window is currently maximized
bool IsWindowFocused(void) { return true; }                                // Check if window is currently focused
bool IsWindowResized(void) { return false; }
bool IsWindowState(unsigned int flag);//check flags
void SetWindowState(unsigned int flags);
void ClearWindowState(unsigned int flags);
void ToggleFullscreen(void);                                // Toggle window state: fullscreen/windowed, resizes monitor to match window resolution
void ToggleBorderlessWindowed(void);                        // Toggle window state: borderless windowed, resizes window to match monitor resolution
void MaximizeWindow(void);                                  // Set window state: maximized, if resizable
void MinimizeWindow(void);                                  // Set window state: minimized, if resizable
void RestoreWindow(void);                                   // Restore window from being minimized/maximized
void SetWindowIcon(Image image);                            // Set icon for window (single image, RGBA 32bit)
void SetWindowIcons(Image *images, int count);              // Set icon for window (multiple images, RGBA 32bit)
void SetWindowTitle(const char *title);                     // Set title for window
void SetWindowPosition(int x, int y);                       // Set window position on screen
void SetWindowMonitor(int monitor);                         // Set monitor for the current window
void SetWindowMinSize(int width, int height);               // Set window minimum dimensions (for FLAG_WINDOW_RESIZABLE)
void SetWindowMaxSize(int width, int height);               // Set window maximum dimensions (for FLAG_WINDOW_RESIZABLE)
void SetWindowSize(int width, int height);                  // Set window dimensions
void SetWindowOpacity(float opacity);                       // Set window opacity [0.0f..1.0f]
void SetWindowFocused(void);                                // Set window focused
void *GetWindowHandle(void);

int getScreenWidth()
{
    return 256;
}

int GetScreenHeight()
{
    return 192;
}

int GetMonitorCount(void)
{
    return 1;
}


void EnableEventWaiting(void)
{
    DS.pollEvents = true;
}
void DisableEventWaiting(void)
{
    DS.pollEvents = false;
}

//cursor ones for compatibility?? best to just not compile? maybe just make it act as touchscreen (lmb = left half, rmb = right half)?
void ShowCursor(void);                                      // Shows cursor
void HideCursor(void);                                      // Hides cursor
bool IsCursorHidden(void);                                  // Check if cursor is not visible
void EnableCursor(void);                                    // Enables cursor (unlock cursor)
void DisableCursor(void);                                   // Disables cursor (lock cursor)
bool IsCursorOnScreen(void);

void RlSwapScreens()
{
    lcdSwap();
}

void ClearBackground(Color color)
{
    BG_PALETTE[0] = RGB15(color.r,color.g,color.b);
    //consoleClear(); todo se if this stays?
}

void BeginDrawing()
{
    swiWaitForVBlank();
    glBegin2D();
}

void SetTargetFPS(int fps)
{
    //non functional, only here for compatibility
}
void EndDrawing()
{
    glEnd2D();
    glFlush(0);
    u32 currentTicks = cpuGetTiming();
    u32 elapsed = currentTicks - DS.lastTicks;
    DS.lastTicks = currentTicks;
    DS.frameTime = (float)elapsed / BUS_CLOCK;
}

float GetFrameTime()
{
    return DS.frameTime;
}

float GetTime()
{
    u32  ticks = cpuGetTiming();
    return ticks / BUS_CLOCK;
}

void SetRandomSeed(unsigned int seed)
{
    srand(seed);
}
int GetRandomValue(int min, int max)
{
    if (min > max) {
        int tmp = max;
        max = min;
        min = tmp;
    }
    int value = (rand()%(abs(max - min) + 1) + min);

    return value;
}

void TakeScreenshot(const char* filename) //only captures top screen,
{
    vramSetBankD(VRAM_D_LCD);
    REG_DISPCAPCNT = DCAP_BANK(DCAP_BANK_VRAM_D) | DCAP_SIZE(DCAP_SIZE_256x192) | DCAP_MODE(DCAP_SRC_A(DCAP_SRC_A_COMPOSITED))  | DCAP_ENABLE;
    swiWaitForVBlank();
    u16* screen = VRAM_D;
    //todo just capture using y * 256 * x (either rgb555 or bgr555)
    vramSetBankD(VRAM_D_TEXTURE_SLOT3);
}

void *MemAlloc(unsigned int size)
{
    // would malloc(size) just work??? doubt it
    malloc(size);
}
void *MemRealloc(void *ptr, unsigned int size)
{

}
void MemFree(void *ptr)
{
    free(ptr);
}


unsigned char *LoadFileData(const char *fileName, int *dataSize)
{
    unsigned char *data = NULL;
    *dataSize = 0;

    if (fileName != NULL)
    {
        //if (loadFileData) return loadFileData(fileName, dataSize); tf?

        FILE *file = fopen(fileName, "rb");

        if (file != NULL)
        {
            // WARNING: On binary streams SEEK_END could not be found,
            // using fseek() and ftell() could not work in some (rare) cases
            fseek(file, 0, SEEK_END);
            int size = ftell(file);     // WARNING: ftell() returns 'long int', maximum size returned is INT_MAX (2147483647 bytes)
            fseek(file, 0, SEEK_SET);

            if (size > 0)
            {
                data = (unsigned char *)calloc(size, sizeof(unsigned char));

                if (data != NULL)
                {
                    // NOTE: fread() returns number of read elements instead of bytes, so reading [1 byte, size elements]
                    size_t count = fread(data, sizeof(unsigned char), size, file);

                    // WARNING: fread() returns a size_t value, usually 'unsigned int' (32bit compilation) and 'unsigned long long' (64bit compilation)
                    // dataSize is unified along raylib as a 'int' type, so, for file-sizes >INT_MAX (2147483647 bytes) there is a limitation
                    if (count > 2147483647)
                    {
                        printf("FILEIO: [%s] File is bigger than 2147483647 bytes, avoid using LoadFileData()", fileName);

                        free(data);
                        data = NULL;
                    }
                    else
                    {
                        *dataSize = (int)count;

                        if ((*dataSize) != size) printf("FILEIO: [%s] File partially loaded (%i bytes out of %i)", fileName, *dataSize, size);
                        else printf("FILEIO: [%s] File loaded successfully", fileName);
                    }
                }
                else printf("FILEIO: [%s] Failed to allocated memory for file reading", fileName);
            }
            else printf("FILEIO: [%s] Failed to read file", fileName);

            fclose(file);
        }
        else printf("FILEIO: [%s] Failed to open file", fileName);
    }
    else printf("FILEIO: File name provided is not valid");

    return data;
}

void UnloadFileData(unsigned char *data)
{
    free(data);
}// Unload file data allocated by LoadFileData()
bool SaveFileData(const char *fileName, const void *data, int dataSize)
{
    bool result = false;

    if (fileName != NULL)
    {
        //if (saveFileData) return saveFileData(fileName, data, dataSize); ??

        FILE *file = fopen(fileName, "wb");

        if (file != NULL)
        {
            // WARNING: fwrite() returns a size_t value, usually 'unsigned int' (32bit compilation) and 'unsigned long long' (64bit compilation)
            // and expects a size_t input value but as dataSize is limited to INT_MAX (2147483647 bytes), there shouldn't be a problem
            int count = (int)fwrite(data, sizeof(unsigned char), dataSize, file);

            if (count == 0) printf("FILEIO: [%s] Failed to write file", fileName);
            else if (count != dataSize) printf("FILEIO: [%s] File partially written", fileName);
            else printf("FILEIO: [%s] File saved successfully", fileName);

            int closed = fclose(file);
            if (closed == 0) result = true;
        }
        else printf("FILEIO: [%s] Failed to open file", fileName);
    }
    else printf("FILEIO: File name provided is not valid");

    return result;
} // Save data to file from byte array (write), returns true on success
bool ExportDataAsCode(const unsigned char *data, int dataSize, const char *fileName)
{
    bool result = false;

#ifndef TEXT_BYTES_PER_LINE
    #define TEXT_BYTES_PER_LINE     20
#endif

    // NOTE: Text data buffer size is estimated considering raw data size in bytes
    // and requiring 6 char bytes for every byte: "0x00, "
    char *txtData = (char *)calloc(dataSize*6 + 2000, sizeof(char));

    int byteCount = 0;
    byteCount += sprintf(txtData + byteCount, "////////////////////////////////////////////////////////////////////////////////////////\n");
    byteCount += sprintf(txtData + byteCount, "//                                                                                    //\n");
    byteCount += sprintf(txtData + byteCount, "// DataAsCode exporter v1.0 - Raw data exported as an array of bytes                  //\n");
    byteCount += sprintf(txtData + byteCount, "//                                                                                    //\n");
    byteCount += sprintf(txtData + byteCount, "// more info and bugs-report:  github.com/raysan5/raylib                              //\n");
    byteCount += sprintf(txtData + byteCount, "// feedback and support:       ray[at]raylib.com                                      //\n");
    byteCount += sprintf(txtData + byteCount, "//                                                                                    //\n");
    byteCount += sprintf(txtData + byteCount, "// Copyright (c) 2022-2026 Ramon Santamaria (@raysan5)                                //\n");
    byteCount += sprintf(txtData + byteCount, "//                                                                                    //\n");
    byteCount += sprintf(txtData + byteCount, "////////////////////////////////////////////////////////////////////////////////////////\n\n");

    // Get file name from path
    char varFileName[256] = { 0 };
    snprintf(varFileName, 256, "%s", GetFileNameWithoutExt(fileName));
    for (int i = 0; varFileName[i] != '\0'; i++)
    {
        // Convert variable name to uppercase
        if ((varFileName[i] >= 'a') && (varFileName[i] <= 'z')) { varFileName[i] = varFileName[i] - 32; }
        // Replace non valid character for C identifier with '_'
        else if (varFileName[i] == '.' || varFileName[i] == '-' || varFileName[i] == '?' || varFileName[i] == '!' || varFileName[i] == '+') { varFileName[i] = '_'; }
    }

    byteCount += sprintf(txtData + byteCount, "#define %s_DATA_SIZE     %i\n\n", varFileName, dataSize);

    byteCount += sprintf(txtData + byteCount, "static unsigned char %s_DATA[%s_DATA_SIZE] = { ", varFileName, varFileName);
    for (int i = 0; i < (dataSize - 1); i++) byteCount += sprintf(txtData + byteCount, ((i%TEXT_BYTES_PER_LINE == 0)? "0x%x,\n" : "0x%x, "), data[i]);
    byteCount += sprintf(txtData + byteCount, "0x%x };\n", data[dataSize - 1]);

    // NOTE: Text data size exported is determined by '\0' (NULL) character
    result = SaveFileText(fileName, txtData);

    free(txtData);

    if (result != 0) printf("FILEIO: [%s] Data as code exported successfully", fileName);
    else printf("FILEIO: [%s] Failed to export data as code", fileName);

    return result;
} // Export data to code (.h), returns true on success
char *LoadFileText(const char *fileName)
{
    char *text = NULL;

    if (fileName != NULL)
    {
       // if (loadFileText) return loadFileText(fileName);

        FILE *file = fopen(fileName, "rt");

        if (file != NULL)
        {
            // WARNING: When reading a file as 'text' file,
            // text mode causes carriage return-linefeed translation...
            // ...but using fseek() should return correct byte-offset
            fseek(file, 0, SEEK_END);
            unsigned int size = (unsigned int)ftell(file);
            fseek(file, 0, SEEK_SET);

            if (size > 0)
            {
                text = (char *)calloc(size + 1, sizeof(char));

                if (text != NULL)
                {
                    unsigned int count = (unsigned int)fread(text, sizeof(char), size, file);

                    // WARNING: \r\n is converted to \n on reading, so,
                    // read bytes count gets reduced by the number of lines
                    if (count < size) text = (char *)realloc(text, count + 1);

                    // Zero-terminate the string
                    text[count] = '\0';

                    printf("FILEIO: [%s] Text file loaded successfully", fileName);
                }
                else printf("FILEIO: [%s] Failed to allocated memory for file reading", fileName);
            }
            else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to read text file", fileName);

            fclose(file);
        }
        else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to open text file", fileName);
    }
    else TRACELOG(LOG_WARNING, "FILEIO: File name provided is not valid");

    return text;
}

// Load text data from file (read), returns a '\0' terminated string
void UnloadFileText(char *text);                              // Unload file text data allocated by LoadFileText()
bool SaveFileText(const char *fileName, const char *text);    // Save text data to file (write), string must be '\0' terminated, returns true on success

// File access custom callbacks
// WARNING: Callbacks setup is intended for advanced users
void SetLoadFileDataCallback(void* callback);  // Set custom file binary data loader
void SetSaveFileDataCallback(void* callback);  // Set custom file binary data saver
void SetLoadFileTextCallback(void* callback);  // Set custom file text data loader
void SetSaveFileTextCallback(void* callback);  // Set custom file text data saver

int FileRename(const char *fileName, const char *fileRename); // Rename file (if exists)
int FileRemove(const char *fileName);                         // Remove file (if exists)
int FileCopy(const char *srcPath, const char *dstPath);       // Copy file from one path to another, dstPath created if it doesn't exist
int FileMove(const char *srcPath, const char *dstPath);       // Move file from one directory to another, dstPath created if it doesn't exist
int FileTextReplace(const char *fileName, const char *search, const char *replacement); // Replace text in an existing file
int FileTextFindIndex(const char *fileName, const char *search); // Find text in existing file
bool FileExists(const char *fileName);                        // Check if file exists
bool DirectoryExists(const char *dirPath);                    // Check if a directory path exists
bool IsFileExtension(const char *fileName, const char *ext);  // Check file extension (recommended include point: .png, .wav)
int GetFileLength(const char *fileName);                      // Get file length in bytes (NOTE: GetFileSize() conflicts with windows.h)
long GetFileModTime(const char *fileName);                    // Get file modification time (last write time)
const char *GetFileExtension(const char *fileName);           // Get pointer to extension for a filename string (includes dot: '.png')
const char *GetFileName(const char *filePath);                // Get pointer to filename for a path string
const char *GetFileNameWithoutExt(const char *filePath);      // Get filename string without extension (uses static string)
const char *GetDirectoryPath(const char *filePath);           // Get full path for a given fileName with path (uses static string)
const char *GetPrevDirectoryPath(const char *dirPath);        // Get previous directory path for a given path (uses static string)
const char *GetWorkingDirectory(void);                        // Get current working directory (uses static string)
const char *GetApplicationDirectory(void);                    // Get the directory of the running application (uses static string)
int MakeDirectory(const char *dirPath);                       // Create directories (including full path requested), returns 0 on success
bool ChangeDirectory(const char *dirPath);                    // Change working directory, return true on success
bool IsPathFile(const char *path);                            // Check if a given path is a file or a directory
bool IsFileNameValid(const char *fileName);                   // Check if fileName is valid for the platform/OS
FilePathList LoadDirectoryFiles(const char *dirPath);         // Load directory filepaths, files and directories, no subdirs scan
FilePathList LoadDirectoryFilesEx(const char *basePath, const char *filter, bool scanSubdirs); // Load directory filepaths with extension filtering and subdir scan; some filters available: "*.*", "FILES*", "DIRS*"
void UnloadDirectoryFiles(FilePathList files);                // Unload filepaths
bool IsFileDropped(void);                                     // Check if a file has been dropped into window
FilePathList LoadDroppedFiles(void);                          // Load dropped filepaths
void UnloadDroppedFiles(FilePathList files);                  // Unload dropped filepaths
unsigned int GetDirectoryFileCount(const char *dirPath);      // Get the file count in a directory
unsigned int GetDirectoryFileCountEx(const char *basePath, const char *filter, bool scanSubdirs); // Get the file count in a directory with extension filtering and recursive directory scan. Use 'DIR' in the filter string to include directories in the result

// Compression/Encoding functionality
unsigned char *CompressData(const unsigned char *data, int dataSize, int *compDataSize);        // Compress data (DEFLATE algorithm), memory must be MemFree()
unsigned char *DecompressData(const unsigned char *compData, int compDataSize, int *dataSize);  // Decompress data (DEFLATE algorithm), memory must be MemFree()
char *EncodeDataBase64(const unsigned char *data, int dataSize, int *outputSize);               // Encode data to Base64 string (includes NULL terminator), memory must be MemFree()
unsigned char *DecodeDataBase64(const char *text, int *outputSize);                             // Decode Base64 string (expected NULL terminated), memory must be MemFree()
unsigned int ComputeCRC32(unsigned char *data, int dataSize); // Compute CRC32 hash code
unsigned int *ComputeMD5(unsigned char *data, int dataSize);  // Compute MD5 hash code, returns static int[4] (16 bytes)
unsigned int *ComputeSHA1(unsigned char *data, int dataSize); // Compute SHA1 hash code, returns static int[5] (20 bytes)
unsigned int *ComputeSHA256(unsigned char *data, int dataSize); // Compute SHA256 hash code, returns static int[8] (32 bytes)
