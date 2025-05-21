#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <random>

#include <Shobjidl.h>
#include <Shlobj.h>
#include <winbase.h>
#include <windows.h>

#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBIW_WINDOWS_UTF8
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "joey-logo.h"

#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Max(a, b) ((a) > (b) ? (a) : (b))

void GenerateBlob() {
    int Width = 0;
    int Height = 0;
    int ChannelCount = 0;
    int DesiredComponents = 4;
    unsigned int *Data = (unsigned int *)stbi_load("c:\\dev\\projects\\joey-zaza\\joey2.png", &Width, &Height, &ChannelCount, DesiredComponents);
    
    unsigned char *Ptr = (unsigned char *)Data;
    
    printf("#define JOEY_LOGO_WIDTH 221\n");
    printf("#define JOEY_LOGO_HEIGHT 221\n");
    printf("#define JOEY_LOGO_COMPONENTS 4\n");
    printf("unsigned char JoeyLogo[] = {\n");
    for (int y = 0; y < Height; ++y) {
        for (int x = 0; x < Width; ++x) {
            printf("0x%02hhx, ", *Ptr++);
            printf("0x%02hhx, ", *Ptr++);
            printf("0x%02hhx, ", *Ptr++);
            printf("0x%02hhx, ", *Ptr++);
        }
        printf("\n");
    }
    printf("};\n");
}

//int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main() 
{
    char TempDirPath[4096] = {};
    {
        char Home[4096] = {};
        if (!SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, Home))) {
            //printf("SHGetFolderPathW failed\n");
            return 0;
        }
        
        size_t TempDirPathLength = strlen(Home);
        
        memcpy(TempDirPath, Home, TempDirPathLength);
        
        char *NewDir = (char *)"\\.temp";
        size_t AppendLength = strlen(NewDir);
        
        memcpy(TempDirPath + TempDirPathLength, NewDir, AppendLength);
        TempDirPathLength += AppendLength;
        TempDirPath[TempDirPathLength] = 0;
        
        CreateDirectory(TempDirPath, 0);
    }

    char LogFile[4096] = {};
    snprintf(LogFile, sizeof(LogFile), "%s\\log.txt", TempDirPath);

    FILE *LogHandle = fopen(LogFile, "a+");
    if (LogHandle == 0) {
        return 0;
    }
    
    fprintf(LogHandle, "\nNothing to see here Mikey...\n\n");
    fclose(LogHandle);
    LogHandle = 0;

    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        return 0;
    }

    while (true) {
        time_t t = time(NULL);
        struct tm *Now = localtime(&t);
        bool GoodTime = ((Now->tm_hour >= 10 && Now->tm_min >= 30) || (Now->tm_hour > 11)) && (Now->tm_hour < 16);
        if (!GoodTime) {
            if (Now->tm_hour >= 16) {
                break;
            }

            Sleep(1000 * 60); // 60 secs
            continue;
        }

        bool Ok = true;
         
        FILE *LogHandle = fopen(LogFile, "a+");
        if (LogHandle == 0) {
            Ok = false;
        }
               
        IDesktopWallpaper *DesktopWallpaperObj = nullptr;
        hr = CoCreateInstance(__uuidof(DesktopWallpaper), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&DesktopWallpaperObj));
        if (FAILED(hr)) {
            fprintf(LogHandle, "CoCreateInstance failed\n");
            Ok = false;
        }
        else if (Ok) {
            wchar_t *ExistingWallpaperPath16 = 0;
            HRESULT Result = DesktopWallpaperObj->GetWallpaper(NULL, &ExistingWallpaperPath16);
            if (Ok && Result == S_OK) {
                size_t ExistingWallpaperPathLength = wcslen(ExistingWallpaperPath16);
                
                char ExistingWallpaperPath8[4096] = {};
                
                // Returns 0 on failure
                int ByteCount = WideCharToMultiByte(CP_UTF8, 0, 
                                                    ExistingWallpaperPath16, (int)ExistingWallpaperPathLength, 
                                                    (char *)ExistingWallpaperPath8, (int)sizeof(ExistingWallpaperPath8), 
                                                    NULL, NULL);
                
                ExistingWallpaperPath8[ByteCount] = 0;
                fprintf(LogHandle, "ExistingWallpaperPath8 %s\n", ExistingWallpaperPath8);
                
                char *Filename = 0;
                int FilenameLength = 0;
                char *LastSlash = strrchr(ExistingWallpaperPath8, '\\');
                if (LastSlash) {
                    Filename = LastSlash + 1;
                    FilenameLength = (int)strlen(Filename);
                }
                
                
                int FileNumber = 0;
                char *UnderscoreDash = strstr(Filename, "_--");
                if (UnderscoreDash) {
                    FilenameLength = (int)(UnderscoreDash - Filename);
                    
                    char *NumberPart = UnderscoreDash + 3;
                    
                    int NumberLength = 0;
                    for (int i = FilenameLength - 1; i >= 0; i -= 1) {
                        if (NumberPart[i] == '.') {
                            NumberLength = i;
                            break;
                        }
                    }
                    
                    char Number[100] = {};
                    memcpy(Number, NumberPart, NumberLength);
                    if (NumberLength) {
                        int OldNumber = atoi(Number);
                        FileNumber = OldNumber + 1;
                    }
                }
                
                char NewWallpaperPath8[4096] = {};
                snprintf(NewWallpaperPath8, sizeof(NewWallpaperPath8), "%s\\%.*s_--%d.png", TempDirPath, FilenameLength, Filename, FileNumber);
                size_t NewWallpaperPathLength = strlen(NewWallpaperPath8);
                fprintf(LogHandle, "NewWallpaperPath8 %s\n", NewWallpaperPath8);
                
                wchar_t NewWallpaperPath16[4096] = {};
                int CharacterCount = MultiByteToWideChar(CP_UTF8, 0, 
                                                        (char *)NewWallpaperPath8, (int)NewWallpaperPathLength, 
                                                        NewWallpaperPath16, (int)sizeof(NewWallpaperPath16));
                NewWallpaperPath16[CharacterCount] = 0;
                
                int Width = 0;
                int Height = 0;
                int ChannelCount = 0;
                int DesiredComponents = 4;
                unsigned char *Data = stbi_load(ExistingWallpaperPath8, &Width, &Height, &ChannelCount, DesiredComponents);
                if (Ok && Data) {
                    fprintf(LogHandle, "Got image data, x = %d, y = %d, n = %d\n", Width, Height, ChannelCount);
                    
                    std::random_device rd;
                    std::default_random_engine e1(rd());
                    std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
                    
                    float RandX = uniform_dist(e1);
                    float RandY = uniform_dist(e1);
                    
                    int MinX = (int)Min(JOEY_LOGO_WIDTH * 1.2f, Width);
                    int MaxX = (int)Min((unsigned)(Width - (JOEY_LOGO_WIDTH * 1.2f)), (unsigned)Width);
                    
                    int MinY = (int)Min(JOEY_LOGO_HEIGHT * 1.2f, Width);
                    int MaxY = (int)Min((unsigned)(Height - (JOEY_LOGO_HEIGHT * 1.2f)), (unsigned)Height);
                    int RangeX = MaxX - MinX;
                    int RangeY = MaxY - MinY;
                    int PosX = (int)(MinX + (RandX * RangeX));
                    int PosY = (int)(MinY + (RandY * RangeY));
                    
                    int Stride = Width * DesiredComponents;
                    
                    fprintf(LogHandle, "RandX: %f\n", RandX);
                    fprintf(LogHandle, "RandY: %f\n", RandY);
                    fprintf(LogHandle, "MinX: %d\n", MinX);
                    fprintf(LogHandle, "MaxX: %d\n", MaxX);
                    fprintf(LogHandle, "MinY: %d\n", MinY);
                    fprintf(LogHandle, "MaxY: %d\n", MaxY);
                    fprintf(LogHandle, "PosX: %d\n", PosX);
                    fprintf(LogHandle, "PosY: %d\n", PosY);
                    
                    for (int y = 0; y < JOEY_LOGO_HEIGHT; ++y) {
                        if (PosY + y >= Height) {
                            break;
                        }
                        unsigned int *DestPtr = (unsigned int *)(Data + (PosX * DesiredComponents) + ((PosY + y) * Stride));
                        unsigned int *SourcePtr = (unsigned int *)(JoeyLogo + (y * JOEY_LOGO_WIDTH * JOEY_LOGO_COMPONENTS));
                        for (int x = 0; x < JOEY_LOGO_WIDTH; ++x) {
                            if (PosX + x >= Width) {
                                break;
                            }
                            //                         AARRGGBB
                            // unsigned int Colour = 0xFF00FF00; 
                            unsigned int SourcePixel = *SourcePtr++;
                            unsigned int DestPixel = *DestPtr;
                            
                            float Alpha =   ((SourcePixel & 0xFF000000) >> 24) / 255.0f;
                            
                            float SourceR = ((SourcePixel & 0x00FF0000) >> 16) / 255.0f;
                            float SourceG = ((SourcePixel & 0x0000FF00) >> 8) / 255.0f;
                            float SourceB = ((SourcePixel & 0x000000FF) >> 0) / 255.0f;
                            
                            float DestR = ((DestPixel & 0x00FF0000) >> 16) / 255.0f;
                            float DestG = ((DestPixel & 0x0000FF00) >> 8) / 255.0f;
                            float DestB = ((DestPixel & 0x000000FF) >> 0) / 255.0f;
                            
                            unsigned int Red = (unsigned int)roundf(((1.0f - Alpha) * DestR + (Alpha * SourceR)) * 255.0f);
                            unsigned int Green = (unsigned int)roundf(((1.0f - Alpha) * DestG + (Alpha * SourceG)) * 255.0f);
                            unsigned int Blue = (unsigned int)roundf(((1.0f - Alpha) * DestB + (Alpha * SourceB)) * 255.0f);
                            
                            unsigned int Colour = 0xFF000000 | (Red << 16) | (Green << 8) | (Blue << 0);
                            *DestPtr++ = Colour;
                        }
                    }
                    stbi_write_png_compression_level = 8;
                    
                    int WriteRet = stbi_write_png(NewWallpaperPath8, Width, Height, DesiredComponents, Data, Stride);
                    fprintf(LogHandle, "write ret %d\n", WriteRet);
                    if (Ok && WriteRet) {
                        Result = DesktopWallpaperObj->SetWallpaper(NULL, NewWallpaperPath16);
                        if (Ok && Result == S_OK) {
                            fprintf(LogHandle, "SetWallpaper success\n");
                        }
                        else {
                            fprintf(LogHandle, "SetWallpaper failed\n");
                            Ok = false;
                        }
                    }
                    else {
                        Ok = false;
                    }
                }
                else {
                    fprintf(LogHandle, "Failed to load wallpaper image\n");
                    Ok = false;
                }
            }
            else {
                fprintf(LogHandle, "Failed to get wallpaper\n");
                Ok = false;
            }
        }
         
        if (DesktopWallpaperObj) DesktopWallpaperObj->Release();
        if (LogHandle) fclose(LogHandle);

        if (!Ok) {
            break;
        }

        Sleep(1000 * 60 * 60); // 1 hour
    }

    CoUninitialize();
   
    return 0;
}