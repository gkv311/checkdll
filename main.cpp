/**
 * Copyright © Kirill Gavrilov, 2010-2017
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#include <windows.h>

#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <ImageHlp.h>

#include "stConsole.h"

template<class T> // T is PIMAGE_NT_HEADERS 
IMAGE_SECTION_HEADER* getEnclosingSectionHeader (DWORD theRva, T* theNTHeader)
{
  IMAGE_SECTION_HEADER* aSectionHeader = IMAGE_FIRST_SECTION(theNTHeader);
  for (unsigned aSectionIter = 0; aSectionIter < theNTHeader->FileHeader.NumberOfSections; ++aSectionIter, ++aSectionHeader)
  {
    DWORD aSize = aSectionHeader->Misc.VirtualSize != 0 ? aSectionHeader->Misc.VirtualSize : aSectionHeader->SizeOfRawData;
    if(theRva >= aSectionHeader->VirtualAddress
    && theRva < (aSectionHeader->VirtualAddress + aSize))
    {
      return aSectionHeader;
    }
  }
  return NULL;
}

template<class T> // T is PIMAGE_NT_HEADERS 
void* getPtrFromRVA (DWORD theRva, T* theNTHeader, BYTE* theImageBase) 
{
  IMAGE_SECTION_HEADER* aSectionHeader = getEnclosingSectionHeader (theRva, theNTHeader);
  if (aSectionHeader == NULL)
  {
    return 0;
  }

  int delta = int(aSectionHeader->VirtualAddress - aSectionHeader->PointerToRawData);
  return (void* )(theImageBase + theRva - delta);
}

bool readDependences (const std::string&        theName,
                      std::vector<std::string>& theTable)
{
  LOADED_IMAGE* anImage = ImageLoad (theName.c_str(), 0);
  if (anImage == NULL)
  {
    GetLastError();
    return false;
  }
  else if (anImage->FileHeader->OptionalHeader.NumberOfRvaAndSizes < 2)
  {
    ImageUnload (anImage);
    return true;
  }

  IMAGE_IMPORT_DESCRIPTOR* anImportDesc =
      (IMAGE_IMPORT_DESCRIPTOR* )getPtrFromRVA(
          anImage->FileHeader->OptionalHeader.DataDirectory[1].VirtualAddress,
          anImage->FileHeader,
          anImage->MappedAddress);
  for (;;)
  {
    // See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
    if (anImportDesc->TimeDateStamp == 0
     && anImportDesc->Name == 0)
    {
      break;
    }

    const char* aName = (const char* )getPtrFromRVA (anImportDesc->Name,
                                                     anImage->FileHeader,
                                                     anImage->MappedAddress);
    theTable.push_back (aName);
    ++anImportDesc;
  }
  ImageUnload (anImage);
  return true;
}

bool probeModule (const std::wstring& theName,
                  const std::string&  theShortName,
                  const std::wstring& thePrefix,
                  int   theDepDepth,
                  std::set<std::string>& theShownLibs)
{
  if (theDepDepth > 100)
  {
    return false;
  }

  std::wstring aName = theName;
  if (aName.empty())
  {
    aName = std::wstring (theShortName.begin(), theShortName.end());
  }
  if (theDepDepth == 0)
  {
    std::wcout << thePrefix << L'\"' << aName << L"\" Loading...";

    // Ensure error messages will be shown!
    // This is the only way to get description of which symbols are missing.
    // Does not work on Win10.
    SetErrorMode (0);
  }
  else
  {
    SetErrorMode (SEM_FAILCRITICALERRORS);
  }
  HMODULE aModule = !theName.empty()
                  ? LoadLibraryW (theName.c_str())
                  : LoadLibraryA (theShortName.c_str());
  if (aModule != NULL)
  {
    if (theDepDepth == 0)
    {
      std::wcout << st::COLOR_FOR_GREEN << L" Success!\n" << st::COLOR_FOR_WHITE;
    }
    ///FreeLibrary (aModule);
    return true;
  }

  DWORD anErrId = GetLastError();
  if (theDepDepth == 0)
  {
    wchar_t anErrDesc[4096];
    FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, 0, anErrId,
                    ///MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), // might be unavailable on localized WinXP-
                    anErrDesc, 4096, NULL);
    size_t anErrDescLen = wcslen (anErrDesc);
    if (anErrDescLen > 3)
    {
      anErrDesc[anErrDescLen - 3] = L'\0';
    }
  
    ///MessageBoxW (NULL, anErrDesc, L"Failed", MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST);
    std::wcout << st::COLOR_FOR_RED << L" FAILED!\n" << st::COLOR_FOR_WHITE
               << L"  Error #" << anErrId << L": " << anErrDesc << L"!\n";
  }

  if (theShownLibs.find(theShortName) != theShownLibs.end())
  {
    return false;
  }

  std::vector<std::string> aDeps;
  std::wstring aPrefix = thePrefix + L"  ";
  if (!readDependences (theShortName, aDeps))
  {
    theShownLibs.insert (theShortName);
    std::wcout << st::COLOR_FOR_RED << aPrefix << aName << L" not found!" << st::COLOR_FOR_WHITE << L"\n";
  }
  for (std::vector<std::string>::const_iterator aDepIter = aDeps.begin(); aDepIter != aDeps.end(); ++aDepIter)
  {
    probeModule (std::wstring(), *aDepIter, aPrefix, theDepDepth + 1, theShownLibs);
  }
  return false;
}

int main (int , char** )
//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  setlocale (LC_ALL, ".OCP");

  std::wcout << L"This is simple tool to check dynamic libraries are loadable\n"
                L"in general way (thus - all dependences are found)\n"
                L"Copyright (C) Kirill Gavrilov, 2010-2016\n\n";

  int aNbArgs = 0;
  wchar_t** anArgVec = CommandLineToArgvW (GetCommandLineW(), &aNbArgs);
  std::wcout << L"Usage: " << anArgVec[0] << L" [path]\n";

  wchar_t aCurrentFolder[4096];
  GetCurrentDirectoryW (4096, aCurrentFolder); // take current directory
  std::wstring aSearchMask (aCurrentFolder);
  if (aNbArgs >= 2)
  {
    aSearchMask = anArgVec[1];
  }
  LocalFree (anArgVec);

  WIN32_FIND_DATAW aFindFile;
  aSearchMask += L"\\*.dll";

  HANDLE aFindIter = FindFirstFileW (aSearchMask.c_str(), &aFindFile);
  for (BOOL hasFile = aFindIter != INVALID_HANDLE_VALUE; hasFile == TRUE;
       hasFile = FindNextFileW (aFindIter, &aFindFile))
  {
    char aShortName[4096];
    wcstombs (aShortName, aFindFile.cAlternateFileName, 4096);
    if (*aShortName == '\0')
    {
      wcstombs (aShortName, aFindFile.cFileName, 4096);
    }
    std::set<std::string> aShownLibs;
    probeModule (aFindFile.cFileName, aShortName, L"", 0, aShownLibs);
  }
  FindClose (aFindIter);

  system ("pause");
  return 0;
}
