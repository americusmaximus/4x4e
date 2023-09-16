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

#include "IO.Ini.hxx"
#include "Strings.hxx"

using namespace Strings;

namespace IO::Ini
{
    // 0x00465730
    void SelectAbstractIniFile(AbstractIni* self, const char* file)
    {
        if (IsNullOrEmpty(file)) { self->Name[0] = NULL; return; }

        strcpy(self->Name, file);
    }

    // 0x00465770
    void SelectAbstractIniSection(AbstractIni* self, const char* section)
    {
        if (IsNullOrEmpty(section)) { self->Section[0] = NULL; return; }

        strcpy(self->Section, section);
    }
}