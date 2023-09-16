/*
Copyright (c) 2023 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "ASCII.hxx"
#include "IO.FileFinder.hxx"
#include "Logger.hxx"
#include "Strings.hxx"

#include <stdlib.h>

#define MAX_FILE_FINDER_DELIMITER_LENGTH 4

using namespace Logger;
using namespace Objects;
using namespace Strings;

namespace IO
{
    FileFinderContainer FileFinderState;

    static time_t filetime_to_timet(FILETIME const& ft)
    {
        ULARGE_INTEGER ull;

        ull.LowPart = ft.dwLowDateTime;
        ull.HighPart = ft.dwHighDateTime;

        return (time_t)(ull.QuadPart / 10000000ULL - 11644473600ULL);
    }

    // 0x004332e0
    FileFinder* ConstructFileFinder(FileFinder* self)
    {
        self->Search.IsActive = FALSE;
        self->File.Name[0] = NULL;

        return self;
    }

    // 0x00433300
    void* ReleaseFileFinder(FileFinder* self, const ReleaseMode mode)
    {
        CloseFileFinder(self);

        return self;
    }

    // 0x00433420
    void CloseFileFinder(FileFinder* self)
    {
        if (!self->Search.IsActive) { self->File.Name[0] = NULL;  return; }

        FindClose(self->Search.Handle);

        self->Search.IsActive = FALSE;
        self->File.Name[0] = NULL;
    }

    // 0x004333a0
    // a.k.a. open
    BOOL ExecuteFileFinder(FileFinder* self)
    {
        if (!self->Search.IsActive) { LogError("File finder search is inactive."); }

        WIN32_FIND_DATAA data;

        if (!FindNextFileA(self->Search.Handle, &data))
        {
            SelectFileFinderState(self, &data);

            return TRUE;
        }

        self->File.Name[0] = NULL;

        return FALSE;
    }

    // 0x00433320
    BOOL OpenFileFinder(FileFinder* self, const char* path)
    {
        WIN32_FIND_DATAA data;

        CloseFileFinder(self);

        auto handle = FindFirstFileA(path, &data);

        if (handle == INVALID_HANDLE_VALUE)
        {
            if (!self->Search.IsActive)
            {
                self->File.Name[0] = NULL;
                return self->Search.IsActive;
            }
        }
        else
        {
            self->Search.Handle = handle;

            SelectFileFinderState(self, &data);

            self->Search.IsActive = TRUE;
        }

        return self->Search.IsActive;
    }

    // 0x0043a600
    // a.k.a. convertStruct
    void SelectFileFinderState(FileFinder* self, WIN32_FIND_DATAA* data)
    {
        ZeroMemory(&self->File.Name, STANDARD_IO_FILE_NAME_LENGTH);

        strncpy(self->File.Name, data->cFileName, (STANDARD_IO_FILE_NAME_LENGTH - 1));

        self->File.Options = FileFinderFileOptions::None;

        if (data->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
        {
            self->File.Options = self->File.Options | FileFinderFileOptions::Archive;
        }

        if (data->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
        {
            self->File.Options = self->File.Options | FileFinderFileOptions::Hidden;
        }

        if (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            self->File.Options = self->File.Options | FileFinderFileOptions::Directory;
        }

        if (data->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        {
            self->File.Options = self->File.Options | FileFinderFileOptions::ReadOnly;
        }

        if (data->nFileSizeHigh != 0 || 0x7fffffff < data->nFileSizeLow)
        {
            LogError("File %s is to big for the game to handle.", data->cFileName);
        }

        self->File.Size = data->nFileSizeLow;
        self->File.DateTime = filetime_to_timet(data->ftLastWriteTime);
    }
}