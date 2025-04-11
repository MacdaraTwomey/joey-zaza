

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include <Shobjidl.h>
#include <Shlobj.h>
#include <winbase.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
 #define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "logo.h"

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
            size_t FilenameLength = 0;
            char *LastSlash = strrchr(ExistingWallpaperPath8, '\\');
            if (LastSlash) {
                Filename = LastSlash + 1;
                FilenameLength = strlen(Filename);
            }

            int Number = 0;
            char *UnderscoreDash = strstr(Filename, "_-");
            if (UnderscoreDash) {
                FilenameLength = (size_t)(UnderscoreDash - Filename);
                char *NumberPart = UnderscoreDash + 2;
                if (*NumberPart) {
                    int OldNumber = atoi(NumberPart);
                    Number = OldNumber + 1;
                }
            }

            char NewWallpaperPath8[4096] = {};
            snprintf(NewWallpaperPath8, sizeof(NewWallpaperPath8), "%s\\%.*s_-%d", TempDirPath, (int)FilenameLength, Filename, Number);
            size_t NewWallpaperPathLength = strlen(NewWallpaperPath8);


            int x = 0;
            int y = 0;
            int n = 0;
            int DesiredComponents = 4;
            unsigned char *Data = stbi_load(ExistingWallpaperPath8, &x, &y, &n, x * DesiredComponents);
            printf("Got image data, x = %d, y = %d, n = %d\n", y, x, n);

            float logo_fraction = (float)x / LOGO_WIDTH;


            float t = (float)rand() / RAND_MAX;
            printf("t %f\n", t);


            int WriteRet = stbi_write_png(NewWallpaperPath8, x, y, DesiredComponents, Data, x);
            printf("write ret %d\n", WriteRet);


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

    DesktopWallpaperObj->Release();
    CoUninitialize();
}