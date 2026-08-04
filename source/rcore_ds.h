#ifndef RCORE_DS
#define RCORE_DS

typedef void (*TraceLogCallback)(int logLevel, const char *text, va_list args); // Logging: Redirect trace log messages
typedef unsigned char *(*LoadFileDataCallback)(const char *fileName, int *dataSize); // FileIO: Load binary data
typedef bool (*SaveFileDataCallback)(const char *fileName, const void *data, int dataSize); // FileIO: Save binary data
typedef char *(*LoadFileTextCallback)(const char *fileName);                  // FileIO: Load text data
typedef bool (*SaveFileTextCallback)(const char *fileName, const char *text); // FileIO: Save text data

typedef struct FilePathList {
    unsigned int count;             // Filepaths entries count
    char **paths;                   // Filepaths entries
} FilePathList;

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;

typedef struct Image {
    void *data;             // Image raw data
    int width;              // Image base width
    int height;             // Image base height
    int mipmaps;            // Mipmap levels, 1 by default
    int format;             // Data format (PixelFormat type)
} Image;
#include <stdbool.h>

//----------------------------------------------------------------------------------
// Logging
//----------------------------------------------------------------------------------
void TRACELOG(int logType, const char *text, ...);
void SetTraceLogCallback(TraceLogCallback callback);

//----------------------------------------------------------------------------------
// Window / core
//----------------------------------------------------------------------------------
void InitWindow(int width, int height, const char *title);
void CloseWindow(void);
bool WindowShouldClose(void);

bool IsWindowReady(void);
bool IsWindowFullscreen(void);
bool IsWindowHidden(void);
bool IsWindowMinimized(void);
bool IsWindowMaximized(void);
bool IsWindowFocused(void);
bool IsWindowResized(void);
bool IsWindowState(unsigned int flag);
void SetWindowState(unsigned int flags);
void ClearWindowState(unsigned int flags);
void ToggleFullscreen(void);
void ToggleBorderlessWindowed(void);
void MaximizeWindow(void);
void MinimizeWindow(void);
void RestoreWindow(void);
void SetWindowIcon(Image image);
void SetWindowIcons(Image *images, int count);
void SetWindowTitle(const char *title);
void SetWindowPosition(int x, int y);
void SetWindowMonitor(int monitor);
void SetWindowMinSize(int width, int height);
void SetWindowMaxSize(int width, int height);
void SetWindowSize(int width, int height);
void SetWindowOpacity(float opacity);
void SetWindowFocused(void);
void *GetWindowHandle(void);

int getScreenWidth(void);
int GetScreenHeight(void);
int GetMonitorCount(void);

void EnableEventWaiting(void);
void DisableEventWaiting(void);

void ShowCursor(void);
void HideCursor(void);
bool IsCursorHidden(void);
void EnableCursor(void);
void DisableCursor(void);
bool IsCursorOnScreen(void);

void RlSwapScreens(void);
void ClearBackground(Color color);
void BeginDrawing(void);
void SetTargetFPS(int fps);
void EndDrawing(void);
float GetFrameTime(void);
float GetTime(void);

void SetRandomSeed(unsigned int seed);
int GetRandomValue(int min, int max);

void TakeScreenshot(const char *filename);

//----------------------------------------------------------------------------------
// Memory management
//----------------------------------------------------------------------------------
void *MemAlloc(unsigned int size);
void *MemRealloc(void *ptr, unsigned int size);
void MemFree(void *ptr);

//----------------------------------------------------------------------------------
// File I/O
//----------------------------------------------------------------------------------
unsigned char *LoadFileData(const char *fileName, int *dataSize);
void UnloadFileData(unsigned char *data);
bool SaveFileData(const char *fileName, const void *data, int dataSize);
bool ExportDataAsCode(const unsigned char *data, int dataSize, const char *fileName);

char *LoadFileText(const char *fileName);
void UnloadFileText(char *text);
bool SaveFileText(const char *fileName, const char *text);

void SetLoadFileDataCallback(LoadFileDataCallback callback);
void SetSaveFileDataCallback(SaveFileDataCallback callback);
void SetLoadFileTextCallback(LoadFileTextCallback callback);
void SetSaveFileTextCallback(SaveFileTextCallback callback);

int FileRename(const char *fileName, const char *fileRename);
int FileRemove(const char *fileName);
int FileCopy(const char *srcPath, const char *dstPath);
int FileMove(const char *srcPath, const char *dstPath);
int FileTextReplace(const char *fileName, const char *search, const char *replacement);
int FileTextFindIndex(const char *fileName, const char *search);

bool FileExists(const char *fileName);
bool IsFileExtension(const char *fileName, const char *ext);
bool DirectoryExists(const char *dirPath);
int GetFileLength(const char *fileName);
long GetFileModTime(const char *fileName);

const char *GetFileExtension(const char *fileName);
const char *GetFileName(const char *filePath);
const char *GetFileNameWithoutExt(const char *filePath);
const char *GetDirectoryPath(const char *filePath);
const char *GetPrevDirectoryPath(const char *dirPath);
const char *GetWorkingDirectory(void);
const char *GetApplicationDirectory(void);

int MakeDirectory(const char *dirPath);
int ChangeDirectory(const char *dirPath);

bool IsPathFile(const char *path);
bool IsPathDirectory(const char *path);
bool IsFileNameValid(const char *fileName);

FilePathList LoadDirectoryFiles(const char *dirPath);
FilePathList LoadDirectoryFilesEx(const char *basePath, const char *filter, bool scanSubdirs);
void UnloadDirectoryFiles(FilePathList files);
void ScanDirectoryFiles(const char *basePath, FilePathList *files, const char *filter, unsigned int maxFiles, bool scanSubdirs);

bool IsFileDropped(void);
FilePathList LoadDroppedFiles(void);
void UnloadDroppedFiles(FilePathList files);

unsigned int GetDirectoryFileCount(const char *dirPath);
unsigned int GetDirectoryFileCountEx(const char *basePath, const char *filter, bool scanSubdirs);

//----------------------------------------------------------------------------------
// Compression / encoding / hashing
//----------------------------------------------------------------------------------
unsigned char *CompressData(const unsigned char *data, int dataSize, int *compDataSize);
unsigned char *DecompressData(const unsigned char *compData, int compDataSize, int *dataSize);

char *EncodeDataBase64(const unsigned char *data, int dataSize, int *outputSize);
unsigned char *DecodeDataBase64(const char *text, int *outputSize);

unsigned int ComputeCRC32(const unsigned char *data, int dataSize);
unsigned int *ComputeMD5(const unsigned char *data, int dataSize);
unsigned int *ComputeSHA1(const unsigned char *data, int dataSize);
unsigned int *ComputeSHA256(const unsigned char *data, int dataSize);

#endif // RCORE_DS