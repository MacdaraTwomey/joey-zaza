

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include <Shobjidl.h>
#include <Shlobj.h>
#include <winbase.h>

//#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#define STBIW_WINDOWS_UTF8
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "logo.h"
#include "joey-logo.h"

int main(int ArgCount, char **Args) 
{
    char TempDirPath[4096] = {};
    {
        char Home[4096] = {};
        if (!SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, Home))) {
            //printf("SHGetFolderPathW failed\n");
            return 1;
        }
        
        size_t TempDirPathLength = strlen(Home);
        
        memcpy(TempDirPath, Home, TempDirPathLength);
        
        char *NewDir = "\\.temp2";
        size_t AppendLength = strlen(NewDir);
        
        memcpy(TempDirPath + TempDirPathLength, NewDir, AppendLength);
        TempDirPathLength += AppendLength;
        TempDirPath[TempDirPathLength] = 0;
        
        BOOL CreatedDir = CreateDirectory(TempDirPath, 0);
        if (CreatedDir) {
            //printf("Created dir\n");
        }
        else {
            //printf("Failed to create dir\n");
        }
    }
    
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        printf("CoInitialize returned %d\n", hr);
        return 1;
    }
    
    IDesktopWallpaper *DesktopWallpaperObj = nullptr;
    hr = CoCreateInstance(__uuidof(DesktopWallpaper), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&DesktopWallpaperObj));
    if (FAILED(hr)) {
        printf("CoCreateInstance failed\n");
    }
    else {
        wchar_t *ExistingWallpaperPath16 = 0;
        HRESULT Result = DesktopWallpaperObj->GetWallpaper(NULL, &ExistingWallpaperPath16);
        if (Result == S_OK) {
            
            size_t ExistingWallpaperPathLength = wcslen(ExistingWallpaperPath16);
            
            char ExistingWallpaperPath8[4096] = {};
            
            // Returns 0 on failure
            int ByteCount = WideCharToMultiByte(CP_UTF8, 0, 
                                                ExistingWallpaperPath16, (int)ExistingWallpaperPathLength, 
                                                (char *)ExistingWallpaperPath8, (int)sizeof(ExistingWallpaperPath8), 
                                                NULL, NULL);
            
            ExistingWallpaperPath8[ByteCount] = 0;
            //printf("ExistingWallpaperPath8 %s\n", ExistingWallpaperPath8);
            
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
            
            
            int Width = 0;
            int Height = 0;
            int ChannelCount = 0;
            int DesiredComponents = 4;
            unsigned char *Data = stbi_load(ExistingWallpaperPath8, &Width, &Height, &ChannelCount, DesiredComponents);
            printf("Got image data, x = %d, y = %d, n = %d\n", Width, Height, ChannelCount);
            
            float logo_fraction = (float)LOGO_WIDTH / Width;
            
            srand((int)time(NULL));
            float t = (float)rand() / RAND_MAX;
            //printf("t %f\n", t);
            
            int MinX = LOGO_WIDTH;
            int MaxX = Width - LOGO_WIDTH;
            
            t = 0.5f;
            int PosX = (int)((1.0f - t) * MinX + (t * MaxX));
            int PosY = Height / 2;
            
            int Stride = Width * DesiredComponents;
            
#if 1
            unsigned int *LogoPtr = (unsigned int *)JoeyLogo;
            for (int y = 0; y < JOEY_LOGO_WIDTH; ++y) {
                unsigned int *Ptr = (unsigned int *)(Data + (PosX * DesiredComponents) + ((PosY + y) * Stride));
                for (int x = 0; x < JOEY_LOGO_WIDTH; ++x) {
                    //                         AARRGGBB
                    // unsigned int Colour = 0xFF00FF00; 
                    unsigned int Colour = *LogoPtr++;
                    //unsigned ColourABGR = Colour 
                    *Ptr++ = Colour;
                }
            }
#endif
            // stb will write expecting RGBA
            
            stbi_write_png_compression_level = 8;

            if (0)
            {
                int Width = 0;
                int Height = 0;
                int ChannelCount = 0;
                int DesiredComponents = 4;
                unsigned int *Data = (unsigned int *)stbi_load("c:\\dev\\projects\\joey-zaza\\joey2.png", &Width, &Height, &ChannelCount, DesiredComponents);

                unsigned char *Ptr = (unsigned char *)Data;
                
                if (0) {
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
                
                int WriteRet = stbi_write_png("c:\\Users\\mac\\.temp2\\joey-logo.png", JOEY_LOGO_WIDTH, JOEY_LOGO_HEIGHT, 4, JoeyLogo, JOEY_LOGO_WIDTH * 4);
                //int WriteRet = stbi_write_png("c:\\Users\\mac\\.temp2\\joey-logo.png", Width, Height, 4, Data, Width * 4);
                printf("write ret %d\n", WriteRet);
            }

            int WriteRet = stbi_write_png("c:\\Users\\mac\\.temp2\\joey-logo.png", Width, Height, DesiredComponents, Data, Stride);
            printf("write ret %d\n", WriteRet);


            //assert(sizeof(joey2) == sizeof(LogoBytes));
            //int different = memcmp(joey2, LogoBytes, sizeof(joey2));
            //printf("different = %d\n", different);
            //int WriteRet = stbi_write_png("c:\\Users\\mac\\.temp2\\joey2.png", JOEY2_WIDTH, JOEY2_HEIGHT, 4, joey2, JOEY2_WIDTH * 4);
            //printf("write ret %d\n", WriteRet);
            //WriteRet = stbi_write_png("c:\\Users\\mac\\.temp2\\LogoBytes.png", LOGO_WIDTH, LOGO_HEIGHT, LOGO_COMPONENTS, LogoBytes, LOGO_WIDTH * LOGO_COMPONENTS);
            
            //int WriteRet = stbi_write_png(NewWallpaperPath8, Width, Height, DesiredComponents, Data, Stride);
            //printf("write ret %d\n", WriteRet);
            
            
            if (0) {
                wchar_t NewWallpaperPath16[4096] = {};
                int CharacterCount = MultiByteToWideChar(CP_UTF8, 0, 
                                                        (char *)NewWallpaperPath8, (int)NewWallpaperPathLength, 
                                                        NewWallpaperPath16, (int)sizeof(NewWallpaperPath16));
                NewWallpaperPath16[CharacterCount] = 0;
                wprintf(L"NewWallpaperPath16 %ws\n", NewWallpaperPath16);
                
                Result = DesktopWallpaperObj->SetWallpaper(NULL, NewWallpaperPath16);
                if (Result == S_OK) {
                    printf("SetWallpaper success\n");
                }
                else {
                    printf("SetWallpaper failed\n");
                }
            }
        }
    }
    
    DesktopWallpaperObj->Release();
    CoUninitialize();
}