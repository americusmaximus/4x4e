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

#include "Memory.hxx"
#include "Native.Basic.hxx"

#include <malloc.h>

namespace Memory
{
    // 0x005a0f70
    void* AllocateMemory(const u32 size)
    {
        auto ptr = malloc(size);

        if (ptr != NULL) { ZeroMemory(ptr, size); }

        return ptr;
    }

    // 0x005a1090
    void ReleaseMemory1(void* ptr) { free(ptr); }

    // 0x005a0c93
    void ReleaseMemory2(void* ptr)
    {
        if (ptr == NULL) { return; }

        ReleaseMemory1(ptr);
    }

    // 0x005a0ccd
    void ReleaseMemory3(void* ptr) { ReleaseMemory2(ptr); }

    // 0x005a1ae0
    void* ReallocateMemory(void* ptr, const u32 size) { return realloc(ptr, size); }
}