#include <gl2d.h>
#include <nds.h>
#include <stdarg.h>
#include "rcore_ds.h"
#include <fat.h>

#define MAX_FILEPATH_LENGTH      256
#define FILE_FILTER_TAG_ALL        "*.*"
#define FILE_FILTER_TAG_DIR_ONLY   "DIRS*"
#define FILE_FILTER_TAG_FILE_ONLY  "FILES*"

typedef void (*TraceLogCallback)(int logLevel, const char *text, va_list args); // Logging: Redirect trace log messages
typedef unsigned char *(*LoadFileDataCallback)(const char *fileName, int *dataSize); // FileIO: Load binary data
typedef bool (*SaveFileDataCallback)(const char *fileName, const void *data, int dataSize); // FileIO: Save binary data
typedef char *(*LoadFileTextCallback)(const char *fileName);                  // FileIO: Load text data
typedef bool (*SaveFileTextCallback)(const char *fileName, const char *text); // FileIO: Save text data


dsCore DS;
static TraceLogCallback traceLog = NULL;            // TraceLog callback function pointer
static LoadFileDataCallback loadFileData = NULL;    // LoadFileData callback function pointer
static SaveFileDataCallback saveFileData = NULL;    // SaveFileText callback function pointer
static LoadFileTextCallback loadFileText = NULL;    // LoadFileText callback function pointer
static SaveFileTextCallback saveFileText = NULL;    // SaveFileText callback function pointer

void TRACELOG(int logType, const char *text, ...)
{

    va_list args;
    va_start(args, text);
    vprintf(text, args);
    va_end(args);

    printf("\n");
}
void SetTraceLogCallback(TraceLogCallback callback)
{
    traceLog = callback;
}


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
    if (!fatInitDefault())
    {
        TRACELOG(LOG_ALL,"FAT: SD CARD NOT LOADED");
    }
    DS.lastTicks = 0;

    DS.windowReady = true;

    DS.currentMainScreen = 0;
}

void CloseWindow(void)
{
    //? i guess unload everything
}

bool WindowShouldClose(void)
{
    //?
    return false;
}

// here for compatibility -- best to not use
bool IsWindowReady(void){ return DS.windowReady; }                                   // Check if window has been initialized successfully
bool IsWindowFullscreen(void){ return true; }                              // Check if window is currently fullscreen
bool IsWindowHidden(void){ return false; }                                  // Check if window is currently hidden
bool IsWindowMinimized(void){ return false; }                           // Check if window is currently minimized
bool IsWindowMaximized(void) { return true; }                               // Check if window is currently maximized
bool IsWindowFocused(void) { return true; }                                // Check if window is currently focused
bool IsWindowResized(void) { return false; }
bool IsWindowState(unsigned int flag){return 1;};//check flags
void SetWindowState(unsigned int flags){};
void ClearWindowState(unsigned int flags){};
void ToggleFullscreen(void){};                                // Toggle window state: fullscreen/windowed, resizes monitor to match window resolution
void ToggleBorderlessWindowed(void){};                        // Toggle window state: borderless windowed, resizes window to match monitor resolution
void MaximizeWindow(void){};                                  // Set window state: maximized, if resizable
void MinimizeWindow(void){};                                  // Set window state: minimized, if resizable
void RestoreWindow(void){};                                   // Restore window from being minimized/maximized
void SetWindowIcon(Image image){return;};                            // Set icon for window (single image, RGBA 32bit)
void SetWindowIcons(Image *images, int count){return;};              // Set icon for window (multiple images, RGBA 32bit)
void SetWindowTitle(const char *title){return;};                     // Set title for window
void SetWindowPosition(int x, int y){return;};                       // Set window position on screen
void SetWindowMonitor(int monitor)
{
    if (DS.currentMainScreen == 0 && monitor == 1)
    {
        lcdSwap();
        DS.currentMainScreen = 1;
    }
    else if (DS.currentMainScreen == 1 && monitor == 0)
    {
        lcdSwap();
        DS.currentMainScreen = 0;
    }
    else
    {
        TRACELOG(LOG_DEBUG,"SCREEN: INVALID MONITOR");
    }

};                         // Set monitor for the current window
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
//kinda doable? keep sprite here, load whn cursor is enabled?
void ShowCursor(void);                                      // Shows cursor
void HideCursor(void);                                      // Hides cursor
bool IsCursorHidden(void);                                  // Check if cursor is not visible
void EnableCursor(void);                                    // Enables cursor (unlock cursor)
void DisableCursor(void);                                   // Disables cursor (lock cursor)
bool IsCursorOnScreen(void);

void RlSwapScreens()
{
    lcdSwap();
    DS.currentMainScreen = !DS.currentMainScreen;
}

void ClearBackground(Color color)
{
    glClearColor(color.r >> 3, color.g >> 3, color.b >> 3, 31);
    BG_PALETTE[0] = RGB15(color.r >> 3, color.g >> 3, color.b >> 3);;
    BG_PALETTE_SUB[0] = RGB15(color.r >> 3, color.g >> 3, color.b >> 3);
    //consoleClear(); todo se if this stays?
}

void BeginDrawing()
{
    scanKeys();
    touchRead(&DS.touchpos);
    swiWaitForVBlank();
    glBegin2D();
}

void SetTargetFPS(int fps)
{
    return;
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
    u32 ticks = cpuGetTiming();
    return ticks / BUS_CLOCK;
}

int GetFPS()
{
    return 60; //todo implement
}

void SwapScreenBuffer(void)
{
    swiWaitForVBlank();
};                      // Swap back buffer with front buffer (screen drawing)
void PollInputEvents(void)
{
    scanKeys();
    touchRead(&DS.touchpos);
}                       // Register all input events
void WaitTime(double seconds)
{

    signed long int cycles = (s32)(seconds * 67130000.0);
    if (cycles > 0)
    {
        swiDelay(cycles);
    }
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
    void *ptr = calloc(size, 1);
    return ptr;
}
void *MemRealloc(void *ptr, unsigned int size)
{
    void *ret = realloc(ptr, size);
    return ret;
}// Internal memory reallocator
void MemFree(void *ptr)
{
    free(ptr);
}// Internal memory free


unsigned char *LoadFileData(const char *fileName, int *dataSize)
{
    unsigned char *data = NULL;
    *dataSize = 0;

    if (fileName != NULL)
    {
        if (loadFileData) return loadFileData(fileName, dataSize);

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
                        TRACELOG(LOG_WARNING, "FILEIO: [%s] File is bigger than 2147483647 bytes, avoid using LoadFileData()", fileName);

                        free(data);
                        data = NULL;
                    }
                    else
                    {
                        *dataSize = (int)count;

                        if ((*dataSize) != size) TRACELOG(LOG_WARNING, "FILEIO: [%s] File partially loaded (%i bytes out of %i)", fileName, *dataSize, size);
                        else TRACELOG(LOG_INFO, "FILEIO: [%s] File loaded successfully", fileName);
                    }
                }
                else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to allocated memory for file reading", fileName);
            }
            else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to read file", fileName);

            fclose(file);
        }
        else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to open file", fileName);
    }
    else TRACELOG(LOG_WARNING, "FILEIO: File name provided is not valid");

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
        if (saveFileData) return saveFileData(fileName, data, dataSize);

        FILE *file = fopen(fileName, "wb");

        if (file != NULL)
        {
            // WARNING: fwrite() returns a size_t value, usually 'unsigned int' (32bit compilation) and 'unsigned long long' (64bit compilation)
            // and expects a size_t input value but as dataSize is limited to INT_MAX (2147483647 bytes), there shouldn't be a problem
            int count = (int)fwrite(data, sizeof(unsigned char), dataSize, file);

            if (count == 0) TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to write file", fileName);
            else if (count != dataSize) TRACELOG(LOG_WARNING, "FILEIO: [%s] File partially written", fileName);
            else TRACELOG(LOG_INFO, "FILEIO: [%s] File saved successfully", fileName);

            int closed = fclose(file);
            if (closed == 0) result = true;
        }
        else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to open file", fileName);
    }
    else TRACELOG(LOG_WARNING, "FILEIO: File name provided is not valid");

    return result;
}// Save data to file from byte array (write), returns true on success
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

    if (result != 0) TRACELOG(LOG_INFO, "FILEIO: [%s] Data as code exported successfully", fileName);
    else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to export data as code", fileName);

    return result;
}
// Export data to code (.h), returns true on success
char *LoadFileText(const char *fileName)
{
    char *text = NULL;

    if (fileName != NULL)
    {
        if (loadFileText) return loadFileText(fileName);

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

                    TRACELOG(LOG_INFO, "FILEIO: [%s] Text file loaded successfully", fileName);
                }
                else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to allocated memory for file reading", fileName);
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
void UnloadFileText(char *text)
{
    free(text);
}// Unload file text data allocated by LoadFileText()
bool SaveFileText(const char *fileName, const char *text)
{
    bool result = false;

    if (fileName != NULL)
    {
        //if (saveFileText) return saveFileText(fileName, text);

        FILE *file = fopen(fileName, "wt");

        if (file != NULL)
        {
            int count = fprintf(file, "%s", text);

            if (count < 0) TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to write text file", fileName);
            else TRACELOG(LOG_INFO, "FILEIO: [%s] Text file saved successfully", fileName);

            int closed = fclose(file);
            if (closed == 0) result = true;
        }
        else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to open text file", fileName);
    }
    else TRACELOG(LOG_WARNING, "FILEIO: File name provided is not valid");

    return result;
}// Save text data to file (write), string must be '\0' terminated

// File access custom callbacks
// WARNING: Callbacks setup is intended for advanced users

// Set custom file binary data loader
void SetLoadFileDataCallback(LoadFileDataCallback callback)
{
    loadFileData = callback;
}
void SetSaveFileDataCallback(SaveFileDataCallback callback)
{
    saveFileData = callback;
}// Set custom file binary data saver
void SetLoadFileTextCallback(LoadFileTextCallback callback)
{
    loadFileText = callback;
}// Set custom file text data loader
void SetSaveFileTextCallback(SaveFileTextCallback callback)
{
    saveFileText = callback;
}// Set custom file text data saver

int FileRename(const char *fileName, const char *fileRename)
{
    int result = -1;

    if (FileExists(fileName))
    {
        result = rename(fileName, fileRename);
    }

    return result;
}// Rename file (if exists)
int FileRemove(const char *fileName)
{
    int result = -1;

    if (FileExists(fileName))
    {
        result = remove(fileName);
    }

    return result;
}// Remove file (if exists)
int FileCopy(const char *srcPath, const char *dstPath)
{
    int result = -1;
    int srcDataSize = 0;
    unsigned char *srcFileData = LoadFileData(srcPath, &srcDataSize);

    // Create required paths if they do not exist
    if (DirectoryExists(GetDirectoryPath(dstPath))) result = 0; // Already exists
    else result = MakeDirectory(GetDirectoryPath(dstPath));

    if (result == 0) // Directory created successfully or already exists
    {
        if ((srcFileData != NULL) && (srcDataSize > 0))
        {
            bool saved = SaveFileData(dstPath, srcFileData, srcDataSize);
            if (saved) result = 0;
        }
    }

    UnloadFileData(srcFileData);

    return result;
} // Copy file from one path to another, dstPath created if it doesn't exist
int FileMove(const char *srcPath, const char *dstPath)
{
    int result = -1;

    if (FileExists(srcPath))
    {
        result = FileCopy(srcPath, dstPath);

        if (result == 0)
        {
            // Make sure file has been correctly copied before removing
            if (FileExists(dstPath) && (GetFileLength(srcPath) == GetFileLength(dstPath)))
            {
                result = FileRemove(srcPath);
                if (result != 0) TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to remove source file after copy", srcPath);
            }
            else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to copy file to [%s]", srcPath, dstPath);
        }
    }
    else TRACELOG(LOG_WARNING, "FILEIO: [%s] Source file does not exist", srcPath);

    return result;
}// Move file from one directory to another, dstPath created if it doesn't exist
int FileTextReplace(const char *fileName, const char *search, const char *replacement)
{
    int result = -1;


    char *fileText = NULL;
    char *fileTextUpdated = { 0 };

    if (FileExists(fileName))
    {
        fileText = LoadFileText(fileName);
       // fileTextUpdated = TextReplaceAlloc(fileText, search, replacement);//TODO UNCOMMENT WHEN RTEXT IS IMPLEMENTD!!!!
        bool saved = SaveFileText(fileName, fileTextUpdated);
        if (saved) result = 0;
        MemFree(fileTextUpdated);
        UnloadFileText(fileText);
    }
    TRACELOG(LOG_WARNING, "FILE: File text replace requires [rtext] module");//todo comment when rtext is implemented


    return result;
}// Replace text in an existing file
int FileTextFindIndex(const char *fileName, const char *search)
{
    int result = -1;

    if (FileExists(fileName))
    {
        char *fileText = LoadFileText(fileName);
        char *ptr = strstr(fileText, search);
        if (ptr != NULL) result = (int)(ptr - fileText);
        UnloadFileText(fileText);
    }

    return result;
}
#include <sys/stat.h> //todo put at top of file
#include <dirent.h>
#define MAX_FILENAME_LENGTH     256
bool FileExists(const char *fileName)
{
    bool result = false;
    struct stat statbuf;
    if (stat(fileName, &statbuf) == 0) result = true;

    return result;
}                       // Check if file exists
bool IsFileExtension(const char *fileName, const char *ext)
{
    #define MAX_FILE_EXTENSIONS  32

    bool result = false;
    const char *fileExt = GetFileExtension(fileName);

    // WARNING: fileExt points to last '.' on fileName string but it could happen
    // that fileName is not correct: "myfile.png more text following\n"

    if (fileExt != NULL)
    {
        int fileExtLength = (int)strlen(fileExt);
        char fileExtLower[16] = { 0 };
        for (int i = 0; (i < fileExtLength) && (i < 15); i++)
        {
            // Copy and convert to lower-case
            if ((fileExt[i] >= 'A') && (fileExt[i] <= 'Z')) fileExtLower[i] =  fileExt[i] + 32;
            else fileExtLower[i] =  fileExt[i];
        }

        int extCount = 1;
        int extLength = (int)strlen(ext);
        char *extList = (char *)calloc(extLength + 1, 1);
        char *extListPtrs[MAX_FILE_EXTENSIONS] = { 0 };
        memcpy(extList, ext, extLength);
        extListPtrs[0] = extList;

        for (int i = 0; i < extLength; i++)
        {
            // Convert to lower-case if extension is upper-case
            if ((extList[i] >= 'A') && (extList[i] <= 'Z')) extList[i] += 32;

            // Get pointer to next extension and add null-terminator
            if (extList[i] == ';')
            {
                extList[i] = '\0';

                if (extCount < MAX_FILE_EXTENSIONS)
                {
                    extListPtrs[extCount] = extList + i + 1;
                    extCount++;
                }
            }
        }

        for (int i = 0; i < extCount; i++)
        {
            // Consider the case where extension provided
            // does not start with the '.'
            char *fileExtLowerPtr = fileExtLower;
            if (extListPtrs[i][0] != '.') fileExtLowerPtr++;

            if (strcmp(fileExtLowerPtr, extListPtrs[i]) == 0)
            {
                result = true;
                break;
            }
        }

        free(extList);
    }

    return result;
}// Check file extension (recommended include point: .png, .wav)
bool DirectoryExists(const char *dirPath)
{
    bool result = false;
    DIR *dir = opendir(dirPath);

    if (dir != NULL)
    {
        result = true;
        closedir(dir);
    }

    return result;
}
int GetFileLength(const char *fileName)
{
    int size = 0;

    // NOTE: On Unix-like systems, it can by used the POSIX system call: stat(),
    // but depending on the platform that call could not be available
    //struct stat result = { 0 };
    //stat(fileName, &result);
    //return result.st_size;

    FILE *file = fopen(fileName, "rb");

    if (file != NULL)
    {
        fseek(file, 0L, SEEK_END);
        long int fileSize = ftell(file);

        // Check for size overflow (INT_MAX)
        if (fileSize > 2147483647) TRACELOG(LOG_WARNING, "[%s] File size overflows expected limit, do not use GetFileLength()", fileName);
        else size = (int)fileSize;

        fclose(file);
    }

    return size;
}// Get file length in bytes (NOTE: GetFileSize() conflicts with windows.h)
long GetFileModTime(const char *fileName)
{
    struct stat result = { 0 };
    long modTime = 0;

    if (stat(fileName, &result) == 0)
    {
        time_t mod = result.st_mtime;
        modTime = (long)mod;
    }

    return modTime;
}                   // Get file modification time (last write time)
const char *GetFileExtension(const char *fileName)
{
    const char *dot = strrchr(fileName, '.');

    if (!dot || (dot == fileName)) return NULL;

    return dot;
}          // Get pointer to extension for a filename string (includes dot: '.png')
static const char *strprbrk(const char *text, const char *charset)
{
    const char *latestMatch = NULL;

    for (; (text != NULL) && (text = strpbrk(text, charset)); latestMatch = text++) { }

    return latestMatch;
}//W
const char *GetFileName(const char *filePath)
{
    const char *fileName = NULL;

    if (filePath != NULL) fileName = strprbrk(filePath, "\\/");

    if (fileName == NULL) return filePath;

    return fileName + 1;
}// Get pointer to filename for a path string
const char *GetFileNameWithoutExt(const char *filePath)
{


    static char fileName[MAX_FILENAME_LENGTH] = { 0 };
    memset(fileName, 0, MAX_FILENAME_LENGTH);

    if (filePath != NULL)
    {
        snprintf(fileName, MAX_FILENAME_LENGTH, "%s", GetFileName(filePath)); // Get filename.ext without path
        int fileNameLength = (int)strlen(fileName); // Get size in bytes

        for (int i = fileNameLength; i > 0; i--) // Reverse search '.'
        {
            if (fileName[i] == '.')
            {
                // NOTE: Break on first '.' found
                fileName[i] = '\0';
                break;
            }
        }
    }

    return fileName;
}     // Get filename string without extension (uses static string)
const char *GetDirectoryPath(const char *filePath)
{
    /*
    // NOTE: Directory separator is different in Windows and other platforms,
    // fortunately, Windows also support the '/' separator, that's the one should be used
    #if defined(_WIN32)
        char separator = '\\';
    #else
        char separator = '/';
    #endif
    */
    const char *lastSlash = NULL;
    static char dirPath[MAX_FILEPATH_LENGTH] = { 0 };
    memset(dirPath, 0, MAX_FILEPATH_LENGTH);

    // In case provided path does not contain a root drive letter (C:\, D:\)
    // nor leading path separator (\, /), add the current directory path to dirPath
    if ((filePath[1] != ':') && (filePath[0] != '\\') && (filePath[0] != '/'))
    {
        // For security, set starting path to current directory,
        // obtained path will be concatenated to this
        dirPath[0] = '.';
        dirPath[1] = '/';
    }

    lastSlash = strprbrk(filePath, "\\/");
    if (lastSlash)
    {
        if (lastSlash == filePath)
        {
            // The last and only slash is the leading one: path is in a root directory
            dirPath[0] = filePath[0];
            dirPath[1] = '\0';
        }
        else
        {
            char *dirPathPtr = dirPath;
            if ((filePath[1] != ':') && (filePath[0] != '\\') && (filePath[0] != '/')) dirPathPtr += 2;     // Skip drive letter, "C:"
            memcpy(dirPathPtr, filePath, strlen(filePath) - (strlen(lastSlash) - 1));
            dirPath[strlen(filePath) - strlen(lastSlash) + (((filePath[1] != ':') && (filePath[0] != '\\') && (filePath[0] != '/'))? 2 : 0)] = '\0';  // Add '\0' manually
        }
    }

    return dirPath;
}          // Get full path for a given fileName with path (uses static string)
const char *GetPrevDirectoryPath(const char *dirPath)
{
    static char prevDirPath[MAX_FILEPATH_LENGTH] = { 0 };
    memset(prevDirPath, 0, MAX_FILEPATH_LENGTH);
    int dirPathLength = (int)strlen(dirPath);

    if (dirPathLength <= 3) snprintf(prevDirPath, MAX_FILEPATH_LENGTH, "%s", dirPath);

    for (int i = (dirPathLength - 1); (i >= 0) && (dirPathLength > 3); i--)
    {
        if ((dirPath[i] == '\\') || (dirPath[i] == '/'))
        {
            // Check for root: "C:\" or "/"
            if (((i == 2) && (dirPath[1] ==':')) || (i == 0)) i++;

            memcpy(prevDirPath, dirPath, i);
            break;
        }
    }

    return prevDirPath;
}       // Get previous directory path for a given path (uses static string)
const char *GetWorkingDirectory(void)
{
    {
        static char currentDir[MAX_FILEPATH_LENGTH] = { 0 };
        memset(currentDir, 0, MAX_FILEPATH_LENGTH);

        char *path = getcwd(currentDir, MAX_FILEPATH_LENGTH);

        return path;
    }
}                     // Get current working directory (uses static string)
const char *GetApplicationDirectory(void)
{
   //no clue how this will work todo add later
    return "fat/:";
};                 // Get the directory of the running application (uses static string)
int MakeDirectory(const char *dirPath)
{
    if ((dirPath == NULL) || (dirPath[0] == '\0')) return -1; // Path is not valid
    if (DirectoryExists(dirPath)) return 0; // Path already exists (is valid)

    // Copy path string to avoid modifying original
    int dirPathLength = (int)strlen(dirPath) + 1;
    char *pathcpy = (char *)calloc(dirPathLength, 1);
    memcpy(pathcpy, dirPath, dirPathLength);

    // Iterate over pathcpy, create each subdirectory as needed
    for (int i = 0; (i < dirPathLength) && (pathcpy[i] != '\0'); i++)
    {
        if (pathcpy[i] == ':') i++;
        else
        {
            if ((pathcpy[i] == '\\') || (pathcpy[i] == '/'))
            {
                pathcpy[i] = '\0';
                if (!DirectoryExists(pathcpy)) mkdir(pathcpy,0755);
                pathcpy[i] = '/';
            }
        }
    }

    // Create final directory
    if (!DirectoryExists(pathcpy)) mkdir(pathcpy,0755);
    free(pathcpy);

    // In case something failed and requested directory
    // was not successfully created, return -1
    if (!DirectoryExists(dirPath)) return -1;

    return 0;
}                   // Create directories (including full path requested), returns 0 on success
int ChangeDirectory(const char *dirPath)
{
    // NOTE: On success, CHDIR() return 0; on error, returns -1 and errno is set to indicate the error,
    // depending on the filesystem, other errors can be returned
    int result = chdir(dirPath);

    if (result != 0) TRACELOG(LOG_WARNING, "SYSTEM: Failed to change to directory: %s", dirPath);
    else TRACELOG(LOG_INFO, "SYSTEM: Working Directory: %s", dirPath);

    return result;
} // Change working directory, return true on success
bool IsPathFile(const char *path)
{
    bool result = false;

    struct stat info = { 0 };
    stat(path, &info);

    if (S_ISREG(info.st_mode)) result = true;

    return result;
}   // Check if provided path point to a file
bool IsPathDirectory(const char *path)
{
    bool result = false;

    if (!IsPathFile(path)) result = true;

    return result;
}
bool IsFileNameValid(const char *fileName)
{
    bool valid = true;

    if ((fileName != NULL) && (fileName[0] != '\0'))
    {
        int fileNameLength = (int)strlen(fileName);
        bool allPeriods = true;

        for (int i = 0; i < fileNameLength; i++)
        {
            // Check invalid characters
            if ((fileName[i] == '<') ||
                (fileName[i] == '>') ||
                (fileName[i] == ':') ||
                (fileName[i] == '\"') ||
                (fileName[i] == '/') ||
                (fileName[i] == '\\') ||
                (fileName[i] == '|') ||
                (fileName[i] == '?') ||
                (fileName[i] == '*')) { valid = false; break; }

            // Check non-glyph characters
            if ((unsigned char)fileName[i] < 32) { valid = false; break; }

            // Check if filename is not all periods
            if (fileName[i] != '.') allPeriods = false;
        }

        if (allPeriods) valid = false;

    }

    return valid;
}             // Check if fileName is valid for the platform/OS
FilePathList LoadDirectoryFiles(const char *dirPath)
{
    return LoadDirectoryFilesEx(dirPath, FILE_FILTER_TAG_ALL, false);
}        // Load directory filepaths, files and directories, no subdirs scan
FilePathList LoadDirectoryFilesEx(const char *basePath, const char *filter, bool scanSubdirs)
{
    FilePathList files = { 0 };

    if (DirectoryExists(basePath)) // It's a directory
    {
        if ((filter != NULL) && (filter[0] == '\0')) filter = NULL;

        // SCAN 1: Count files
        unsigned int fileCounter = GetDirectoryFileCountEx(basePath, filter, scanSubdirs);

        // Memory allocation for dirFileCount
        files.paths = (char **)calloc(fileCounter, sizeof(char *));
        for (unsigned int i = 0; i < fileCounter; i++) files.paths[i] = (char *)calloc(MAX_FILEPATH_LENGTH, sizeof(char));

        // SCAN 2: Read filepaths
        // WARNING: basePath is always prepended to scanned paths
        ScanDirectoryFiles(basePath, &files, filter, fileCounter, scanSubdirs);

        // Security check: read files.count should match fileCounter
        if (files.count != fileCounter)
        {
            TRACELOG(LOG_WARNING, "FILEIO: Read files count (%u) does not match capacity allocated (%u)", files.count, fileCounter);
            files.count = fileCounter; // Avoid memory leak when unloading this FilePathList
        }
    }
    else TRACELOG(LOG_WARNING, "FILEIO: Directory cannot be opened (%s)", basePath);  // Maybe it's a file...

    return files;
}// Load directory filepaths with extension filtering and subdir scan; some filters available: "*.*", "FILES*", "DIRS*"
void UnloadDirectoryFiles(FilePathList files)
{
    if (files.paths != NULL)
    {
        for (unsigned int i = 0; i < files.count; i++) free(files.paths[i]);

        free(files.paths);
    }
}               // Unload filepaths
bool IsFileDropped(void){ return true; }//todo implement later};                                     // Check if a file has been dropped into window
FilePathList LoadDroppedFiles(void){ return (FilePathList){0,0}; }//todo implement later};                          // Load dropped filepaths
void UnloadDroppedFiles(FilePathList files){return;};  //todo implement later                 // Unload dropped filepaths
unsigned int GetDirectoryFileCount(const char *dirPath)
{
    return GetDirectoryFileCountEx(dirPath, FILE_FILTER_TAG_ALL, false);
}      // Get the file count in a directory
unsigned int GetDirectoryFileCountEx(const char *basePath, const char *filter, bool scanSubdirs)
{
    unsigned int fileCounter = 0;
    // WARNING: Path can not be static or it will be reused between recursive function calls
    char path[MAX_FILEPATH_LENGTH] = { 0 };
    memset(path, 0, MAX_FILEPATH_LENGTH);
    struct dirent *entity;
    DIR *dir = opendir(basePath);
    if (dir != NULL)
    {
        while ((entity = readdir(dir)) != NULL)
        {
            if ((strcmp(entity->d_name, ".") != 0) && (strcmp(entity->d_name, "..") != 0))
            {
                int pathLength = snprintf(path, MAX_FILEPATH_LENGTH, "%s/%s", basePath, entity->d_name);

                if ((pathLength < 0) || (pathLength >= MAX_FILEPATH_LENGTH))
                {
                    TRACELOG(LOG_WARNING, "FILEIO: Path longer than %d characters (%s...)", MAX_FILEPATH_LENGTH, basePath);
                }
                else if (IsPathFile(path))
                {
                    if ((filter == NULL) || (strstr(filter, FILE_FILTER_TAG_ALL) != NULL) ||
                        (strstr(filter, FILE_FILTER_TAG_FILE_ONLY) != NULL) || IsFileExtension(path, filter)) fileCounter++;
                }
                else
                {
                    if ((filter != NULL) && ((strstr(filter, FILE_FILTER_TAG_ALL) != NULL) || (strstr(filter, FILE_FILTER_TAG_DIR_ONLY) != NULL))) fileCounter++;
                    if (scanSubdirs) fileCounter += GetDirectoryFileCountEx(path, filter, scanSubdirs);
                }
            }
        }
        closedir(dir);
    }
    else TRACELOG(LOG_WARNING, "FILEIO: Directory cannot be opened (%s)", basePath);
    return fileCounter;
}



// Compression/Encoding functionality
unsigned char *CompressData(const unsigned char *data, int dataSize, int *compDataSize)
{
    #define COMPRESSION_QUALITY_DEFLATE  8

    unsigned char *compData = NULL;

#if SUPPORT_COMPRESSION_API
    // Compress data and generate a valid DEFLATE stream
    struct sdefl *sdefl = (struct sdefl *)RL_CALLOC(1, sizeof(struct sdefl));   // WARNING: Possible stack overflow, struct sdefl is almost 1MB
    int bounds = sdefl_bound(dataSize);
    compData = (unsigned char *)RL_CALLOC(bounds, 1);

    *compDataSize = sdeflate(sdefl, compData, data, dataSize, COMPRESSION_QUALITY_DEFLATE);   // Compression level 8, same as stbiw
    RL_FREE(sdefl);

    TRACELOG(LOG_INFO, "SYSTEM: Compress data: Original size: %i -> Comp. size: %i", dataSize, *compDataSize);
#endif

    return compData;
}
unsigned char *DecompressData(const unsigned char *compData, int compDataSize, int *dataSize)
{
    unsigned char *data = NULL;

#if SUPPORT_COMPRESSION_API
    // Decompress data from a valid DEFLATE stream
    unsigned char *data0 = (unsigned char *)RL_CALLOC(MAX_DECOMPRESSION_SIZE*1024*1024, 1);
    int size = sinflate(data0, MAX_DECOMPRESSION_SIZE*1024*1024, compData, compDataSize);

    // WARNING: RL_REALLOC can make (and leave) data copies in memory,
    // that can be a security concern in case of compression of sensitive data
    // So, using a second buffer to copy data manually, wiping original buffer memory
    data = (unsigned char *)RL_CALLOC(size, 1);
    memcpy(data, data0, size);
    memset(data0, 0, MAX_DECOMPRESSION_SIZE*1024*1024); // Wipe memory, is memset() safe?
    RL_FREE(data0);

    TRACELOG(LOG_INFO, "SYSTEM: Decompress data: Comp. size: %i -> Original size: %i", compDataSize, size);

    *dataSize = size;
#endif

    return data;
}
char *EncodeDataBase64(const unsigned char *data, int dataSize, int *outputSize)
{
    // Base64 conversion table from RFC 4648 [0..63]
    // NOTE: They represent 64 values (6 bits), to encode 3 bytes of data into 4 "sextets" (6bit characters)
    static const char base64EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    // Compute expected size and padding
    int paddedSize = dataSize;
    while (paddedSize%3 != 0) paddedSize++; // Padding bytes to round 4*(dataSize/3) to 4 bytes
    int estimatedOutputSize = 4*(paddedSize/3);
    int padding = paddedSize - dataSize;

    // Adding null terminator to string
    estimatedOutputSize += 1;

    // Load some memory to store encoded string
    char *encodedData = (char *)calloc(estimatedOutputSize, 1);
    if (encodedData == NULL) return NULL;

    int outputCount = 0;
    for (int i = 0; i < dataSize;)
    {
        unsigned int octetA = 0;
        unsigned int octetB = 0;
        unsigned int octetC = 0;
        unsigned int octetPack = 0;

        octetA = data[i]; // Generates 2 sextets
        octetB = ((i + 1) < dataSize)? data[i + 1] : 0; // Generates 3 sextets
        octetC = ((i + 2) < dataSize)? data[i + 2] : 0; // Generates 4 sextets

        octetPack = (octetA << 16) | (octetB << 8) | octetC;

        encodedData[outputCount + 0] = (unsigned char)(base64EncodeTable[(octetPack >> 18) & 0x3f]);
        encodedData[outputCount + 1] = (unsigned char)(base64EncodeTable[(octetPack >> 12) & 0x3f]);
        encodedData[outputCount + 2] = (unsigned char)(base64EncodeTable[(octetPack >> 6) & 0x3f]);
        encodedData[outputCount + 3] = (unsigned char)(base64EncodeTable[octetPack & 0x3f]);
        outputCount += 4;
        i += 3;
    }

    // Add required padding bytes
    for (int p = 0; p < padding; p++) encodedData[outputCount - p - 1] = '=';

    // Add null terminator to string
    encodedData[outputCount] = '\0';
    outputCount++;

    if (outputCount != estimatedOutputSize) TRACELOG(LOG_WARNING, "BASE64: Output size differs from estimation");

    *outputSize = estimatedOutputSize;
    return encodedData;
}
unsigned char *DecodeDataBase64(const char *text, int *outputSize)
{
    // Base64 decode table
    // NOTE: Following ASCII order [0..255] assigning the expected sixtet value to
    // every character in the corresponding ASCII position
    static const unsigned char base64DecodeTable[256] = {
        ['A'] =  0, ['B'] =  1, ['C'] =  2, ['D'] =  3, ['E'] =  4, ['F'] =  5, ['G'] =  6, ['H'] =  7,
        ['I'] =  8, ['J'] =  9, ['K'] = 10, ['L'] = 11, ['M'] = 12, ['N'] = 13, ['O'] = 14, ['P'] = 15,
        ['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19, ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23, ['Y'] = 24, ['Z'] = 25,
        ['a'] = 26, ['b'] = 27, ['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31, ['g'] = 32, ['h'] = 33,
        ['i'] = 34, ['j'] = 35, ['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39, ['o'] = 40, ['p'] = 41,
        ['q'] = 42, ['r'] = 43, ['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47, ['w'] = 48, ['x'] = 49, ['y'] = 50, ['z'] = 51,
        ['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55, ['4'] = 56, ['5'] = 57, ['6'] = 58, ['7'] = 59,
        ['8'] = 60, ['9'] = 61, ['+'] = 62, ['/'] = 63
    };

    *outputSize = 0;
    if (text == NULL) return NULL;

    // Compute expected size and padding
    int dataSize = (int)strlen(text); // WARNING: Expecting NULL terminated strings!
    int ending = dataSize - 1;
    int padding = 0;
    while (text[ending] == '=') { padding++; ending--; }
    int estimatedOutputSize = 3*(dataSize/4) - padding;
    int maxOutputSize = 3*(dataSize/4);

    // Load some memory to store decoded data
    // NOTE: Allocated enough size to include padding
    unsigned char *decodedData = (unsigned char *)calloc(maxOutputSize, 1);
    if (decodedData == NULL) return NULL;

    int outputCount = 0;
    for (int i = 0; i < dataSize;)
    {
        // Every 4 sextets must generate 3 octets
        if ((i + 2) >= dataSize)
        {
            TRACELOG(LOG_WARNING, "BASE64: Decoding error: Input data size is not valid");
            break;
        }

        unsigned int sixtetA = base64DecodeTable[(unsigned char)text[i]];
        unsigned int sixtetB = base64DecodeTable[(unsigned char)text[i + 1]];
        unsigned int sixtetC = (((i + 2) < dataSize) && (unsigned char)text[i + 2] != '=')? base64DecodeTable[(unsigned char)text[i + 2]] : 0;
        unsigned int sixtetD = (((i + 3) < dataSize) && (unsigned char)text[i + 3] != '=')? base64DecodeTable[(unsigned char)text[i + 3]] : 0;

        unsigned int octetPack = (sixtetA << 18) | (sixtetB << 12)  | (sixtetC << 6) | sixtetD;

        if ((outputCount + 3) > maxOutputSize)
        {
            TRACELOG(LOG_WARNING, "BASE64: Decoding error: Output data size is too small");
            break;
        }

        decodedData[outputCount + 0] = (octetPack >> 16) & 0xff;
        decodedData[outputCount + 1] = (octetPack >> 8) & 0xff;
        decodedData[outputCount + 2] = octetPack & 0xff;
        outputCount += 3;
        i += 4;
    }

    if (estimatedOutputSize != (outputCount - padding)) TRACELOG(LOG_WARNING, "BASE64: Decoded size differs from estimation");

    *outputSize = estimatedOutputSize;
    return decodedData;
}
unsigned int ComputeCRC32(const unsigned char *data, int dataSize)
{
    static unsigned int crcTable[256] = {
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
        0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
        0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
        0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
        0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
        0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
        0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
        0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
        0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
        0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
        0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
        0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
        0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
        0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
        0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
        0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
        0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
        0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
        0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
        0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
        0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
        0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
        0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
        0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
        0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
        0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
        0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
        0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
        0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
        0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
        0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
    };

    unsigned int crc = ~0u;

    for (int i = 0; i < dataSize; i++) crc = (crc >> 8) ^ crcTable[data[i] ^ (crc & 0xff)];

    return ~crc;
}
unsigned int *ComputeMD5(const unsigned char *data, int dataSize)
{
    #define ROTATE_LEFT(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

    static unsigned int hash[4] = { 0 };  // Hash to be returned

    // WARNING: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating

    // NOTE: r specifies the per-round shift amounts
    unsigned int r[] = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
    };

    // Using binary integer part of the sines of integers (in radians) as constants
    unsigned int k[] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
    };

    hash[0] = 0x67452301;
    hash[1] = 0xefcdab89;
    hash[2] = 0x98badcfe;
    hash[3] = 0x10325476;

    // Pre-processing: adding a single 1 bit
    // Append '1' bit to message
    // NOTE: The input bytes are considered as bits strings,
    // where the first bit is the most significant bit of the byte

    // Pre-processing: padding with zeros
    // Append '0' bit until message length in bit 448 (mod 512)
    // Append length mod (2 pow 64) to message

    int newDataSize = ((((dataSize + 8)/64) + 1)*64) - 8;

    unsigned char *msg = (unsigned char *)calloc(newDataSize + 64, 1); // Initialize with '0' bits, allocating 64 extra bytes
    memcpy(msg, data, dataSize);
    msg[dataSize] = 128; // Write the '1' bit

    unsigned int bitsLen = 8*dataSize;
    memcpy(msg + newDataSize, &bitsLen, 4); // Append the len in bits at the end of the buffer

    // Process the message in successive 512-bit chunks for each 512-bit chunk of message
    for (int offset = 0; offset < newDataSize; offset += 64)  // 512/8
    {
        // Break chunk into sixteen 32-bit words w[j], 0 <= j <= 15
        unsigned int *w = (unsigned int *)(msg + offset);

        // Initialize hash value for this chunk
        unsigned int a = hash[0];
        unsigned int b = hash[1];
        unsigned int c = hash[2];
        unsigned int d = hash[3];

        for (int i = 0; i < 64; i++)
        {
            unsigned int f = 0;
            unsigned int g = 0;

            if (i < 16)
            {
                f = (b & c) | ((~b) & d);
                g = i;
            }
            else if (i < 32)
            {
                f = (d & b) | ((~d) & c);
                g = (5*i + 1)%16;
            }
            else if (i < 48)
            {
                f = b ^ c ^ d;
                g = (3*i + 5)%16;
            }
            else
            {
                f = c ^ (b | (~d));
                g = (7*i)%16;
            }

            unsigned int temp = d;
            d = c;
            c = b;
            b = b + ROTATE_LEFT((a + f + k[i] + w[g]), r[i]);
            a = temp;
        }

        // Add chunk's hash to result so far
        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
    }

    free(msg);

    return hash;
}
unsigned int *ComputeSHA1(const unsigned char *data, int dataSize)
{
    #define SHA1_ROTATE_LEFT(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

    static unsigned int hash[5] = { 0 };  // Hash to be returned

    // Initialize hash values
    hash[0] = 0x67452301;
    hash[1] = 0xEFCDAB89;
    hash[2] = 0x98BADCFE;
    hash[3] = 0x10325476;
    hash[4] = 0xC3D2E1F0;

    // Pre-processing: adding a single 1 bit
    // Append '1' bit to message
    // NOTE: The input bytes are considered as bits strings,
    // where the first bit is the most significant bit of the byte

    // Pre-processing: padding with zeros
    // Append '0' bit until message length in bit 448 (mod 512)
    // Append length mod (2 pow 64) to message

    int newDataSize = ((((dataSize + 8)/64) + 1)*64);

    unsigned char *msg = (unsigned char *)calloc(newDataSize, 1); // Initialize with '0' bits
    memcpy(msg, data, dataSize);
    msg[dataSize] = 128; // Write the '1' bit

    unsigned long long bitsLen = 8ULL*dataSize;
    msg[newDataSize - 1] = (unsigned char)(bitsLen);
    msg[newDataSize - 2] = (unsigned char)(bitsLen >> 8);
    msg[newDataSize - 3] = (unsigned char)(bitsLen >> 16);
    msg[newDataSize - 4] = (unsigned char)(bitsLen >> 24);
    msg[newDataSize - 5] = (unsigned char)(bitsLen >> 32);
    msg[newDataSize - 6] = (unsigned char)(bitsLen >> 40);
    msg[newDataSize - 7] = (unsigned char)(bitsLen >> 48);
    msg[newDataSize - 8] = (unsigned char)(bitsLen >> 56);

    // Process the message in successive 512-bit chunks
    for (int offset = 0; offset < newDataSize; offset += 64)  // 512/8
    {
        // Break chunk into sixteen 32-bit words w[j], 0 <= j <= 15
        unsigned int w[80] = { 0 };
        for (int i = 0; i < 16; i++)
        {
            w[i] = ((unsigned int)msg[offset + (i*4) + 0] << 24) |
                   ((unsigned int)msg[offset + (i*4) + 1] << 16) |
                   ((unsigned int)msg[offset + (i*4) + 2] << 8) |
                   ((unsigned int)msg[offset + (i*4) + 3]);
        }

        // Message schedule: extend the sixteen 32-bit words into eighty 32-bit words:
        for (int i = 16; i < 80; i++) w[i] = SHA1_ROTATE_LEFT(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);

        // Initialize hash value for this chunk
        unsigned int a = hash[0];
        unsigned int b = hash[1];
        unsigned int c = hash[2];
        unsigned int d = hash[3];
        unsigned int e = hash[4];

        for (int i = 0; i < 80; i++)
        {
            unsigned int f = 0;
            unsigned int k = 0;

            if (i < 20)
            {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            }
            else if (i < 40)
            {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            }
            else if (i < 60)
            {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            }
            else
            {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            unsigned int temp = SHA1_ROTATE_LEFT(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = SHA1_ROTATE_LEFT(b, 30);
            b = a;
            a = temp;
        }

        // Add this chunk's hash to result so far
        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
        hash[4] += e;
    }

    free(msg);

    return hash;
}
unsigned int *ComputeSHA256(const unsigned char *data, int dataSize)
{
    #define SHA256_ROTATE_RIGHT(x, c) ((x >> c) | (x << ((sizeof(unsigned int)*8) - c)))
    #define SHA256_A0(x) (SHA256_ROTATE_RIGHT(x, 7) ^ SHA256_ROTATE_RIGHT(x, 18) ^ (x >> 3))
    #define SHA256_A1(x) (SHA256_ROTATE_RIGHT(x, 17) ^ SHA256_ROTATE_RIGHT(x, 19) ^ (x >> 10))

    static const unsigned int k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    static unsigned int hash[8] = { 0 };
    hash[0] = 0x6A09e667;
    hash[1] = 0xbb67ae85;
    hash[2] = 0x3c6ef372;
    hash[3] = 0xa54ff53a;
    hash[4] = 0x510e527f;
    hash[5] = 0x9b05688c;
    hash[6] = 0x1f83d9ab;
    hash[7] = 0x5be0cd19;

    const unsigned long long bitLen = 8ULL*dataSize;
    unsigned long long paddedSize = dataSize + sizeof(dataSize);
    paddedSize += (64 - (paddedSize%64));
    unsigned char *buffer = (unsigned char *)calloc(paddedSize, sizeof(unsigned char));

    memcpy(buffer, data, dataSize);
    buffer[dataSize] = 0x80;
    for (int i = 1; i <= sizeof(bitLen); i++)
    {
        buffer[(paddedSize - sizeof(bitLen)) + (i - 1)] = (bitLen >> (8*(sizeof(bitLen) - i))) & 0xFF;
    }

    for (unsigned long long blockN = 0; blockN < paddedSize/64; blockN++)
    {
        unsigned int a = hash[0];
        unsigned int b = hash[1];
        unsigned int c = hash[2];
        unsigned int d = hash[3];
        unsigned int e = hash[4];
        unsigned int f = hash[5];
        unsigned int g = hash[6];
        unsigned int h = hash[7];

        unsigned char *block = buffer + (blockN*64);
        unsigned int w[64] = { 0 };
        for (int i = 0; i < 16; i++)
        {
            w[i] = ((unsigned int)block[i*4 + 0] << 24) |
                   ((unsigned int)block[i*4 + 1] << 16) |
                   ((unsigned int)block[i*4 + 2] << 8)  |
                   ((unsigned int)block[i*4 + 3]);
        }
        for (int t = 16; t < 64; t++) w[t] = SHA256_A1(w[t - 2]) + w[t - 7] + SHA256_A0(w[t - 15]) + w[t - 16];

        for (int t = 0; t < 64; t++)
        {
            unsigned int e1 = (SHA256_ROTATE_RIGHT(e, 6) ^ SHA256_ROTATE_RIGHT(e, 11) ^ SHA256_ROTATE_RIGHT(e, 25));
            unsigned int ch = ((e & f) ^ (~e & g));
            unsigned int t1 = (h + e1 + ch + k[t] + w[t]);
            unsigned int e0 = (SHA256_ROTATE_RIGHT(a, 2) ^ SHA256_ROTATE_RIGHT(a, 13) ^ SHA256_ROTATE_RIGHT(a, 22));
            unsigned int maj = ((a & b) ^ (a & c) ^ (b & c));
            unsigned int t2 = e0 + maj;

            h = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
        hash[4] += e;
        hash[5] += f;
        hash[6] += g;
        hash[7] += h;
    }

    free(buffer);

    return hash;
}

bool IsKeyPressed(int key)
{
    return (keysDown() & key) != 0;
}
bool IsKeyPressedRepeat(int key)
{
    return (keysDownRepeat() & key) != 0;
}
bool IsKeyDown(int key)
{
    return (keysHeld() & key) != 0;
}
bool IsKeyReleased(int key)
{
    return (keysUp() & key) != 0;
}
bool IsKeyUp(int key)
{
    return (keysHeld() & key) == 0;
}
int GetKeyPressed(void)
{
    u16 kd = keysDown();
    return kd;
};                                // Get key pressed (keycode), call it multiple times for keys queued, returns 0 when the queue is empty
int GetCharPressed(void);                               // Get char pressed (unicode), call it multiple times for chars queued, returns 0 when the queue is empty
const char *GetKeyName(int key){ return " ";};                        // Get name of a QWERTY key on the current keyboard layout (eg returns string 'q' for KEY_A on an AZERTY keyboard)
void SetExitKey(int key)
{
    DS.exitKey = key;
};

bool IsGamepadAvailable(int gamepad){return true;};                   // Check if a gamepad is available
const char *GetGamepadName(int gamepad){return "Nintendo DS gamepad";};                // Get gamepad internal name id
bool IsGamepadButtonPressed(int gamepad, int button){return IsKeyPressed(button);};   // Check if a gamepad button has been pressed once
bool IsGamepadButtonDown(int gamepad, int button){return IsKeyDown(button);};      // Check if a gamepad button is being pressed
bool IsGamepadButtonReleased(int gamepad, int button){return IsKeyReleased(button);};  // Check if a gamepad button has been released once
bool IsGamepadButtonUp(int gamepad, int button){return IsKeyUp(button);}        // Check if a gamepad button is NOT being pressed
int GetGamepadButtonPressed(void){return GetKeyPressed();};                      // Get the last gamepad button pressed
int GetGamepadAxisCount(int gamepad){return 0;};                   // Get axis count for a gamepad
float GetGamepadAxisMovement(int gamepad, int axis){return 0.0f;};    // Get movement value for a gamepad axis
int SetGamepadMappings(const char *mappings){return 0;};           // Set internal gamepad mappings (SDL_GameControllerDB) doable-ish? //todo
void SetGamepadVibration(int gamepad, float leftMotor, float rightMotor, float duration){return;}; // Set gamepad vibration for both motors (duration in seconds)

//--------------- TODO DO ALL THIS MUCH LATER ---------------------------
bool IsMouseButtonPressed(int button);                  // Check if a mouse button has been pressed once
bool IsMouseButtonDown(int button);                     // Check if a mouse button is being pressed
bool IsMouseButtonReleased(int button);                 // Check if a mouse button has been released once
bool IsMouseButtonUp(int button);                       // Check if a mouse button is NOT being pressed
int GetMouseX(void);                                    // Get mouse position X
int GetMouseY(void);                                    // Get mouse position Y
Vector2 GetMousePosition(void);                         // Get mouse position XY
Vector2 GetMouseDelta(void);                            // Get mouse delta between frames
void SetMousePosition(int x, int y);                    // Set mouse position XY
void SetMouseOffset(int offsetX, int offsetY);          // Set mouse offset
void SetMouseScale(float scaleX, float scaleY);         // Set mouse scaling
float GetMouseWheelMove(void);                          // Get mouse wheel movement for X or Y, whichever is larger
Vector2 GetMouseWheelMoveV(void);                       // Get mouse wheel movement for both X and Y
void SetMouseCursor(int cursor);                        // Set mouse cursor


int GetTouchX(void)
{
    return DS.touchpos.px;
};                                    // Get touch position X for touch point 0 (relative to screen size)
int GetTouchY(void)
{
    return DS.touchpos.py;
};                                    // Get touch position Y for touch point 0 (relative to screen size)
Vector2 GetTouchPosition(int index)
{
    return (Vector2){(float)DS.touchpos.px,(float)DS.touchpos.py};
};                    // Get touch position XY for a touch point index (relative to screen size)
int GetTouchPointId(int index)
{
    //what does this mean?
};                         // Get touch point identifier for given index
int GetTouchPointCount(void)
{
    u16 kd = keysDown();
    if (kd & KEY_TOUCH){ return 1 ;}
    return 0;
};

//https://github.com/blocksds/sdk/blob/master/examples/input/gesture_recognition/source/main.cpp ??ftodo compare
void SetGesturesEnabled(unsigned int flags);            // Enable a set of gestures using flags
bool IsGestureDetected(unsigned int gesture);           // Check if a gesture have been detected
int GetGestureDetected(void);                           // Get latest detected gesture
float GetGestureHoldDuration(void);                     // Get gesture hold time in seconds
Vector2 GetGestureDragVector(void);                     // Get gesture drag vector
float GetGestureDragAngle(void);                        // Get gesture drag angle
Vector2 GetGesturePinchVector(void);                    // Get gesture pinch delta
float GetGesturePinchAngle(void);