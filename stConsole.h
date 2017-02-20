/**
 * Copyright © Kirill Gavrilov, 2009
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __stConsole_h__
#define __stConsole_h__

#if defined(_WIN32)
  #include <stdint.h>
#endif

#include <stddef.h>
#include <iostream>

#if defined(_WIN32)
  #define ST_TEXT(quote) L##quote
  typedef wchar_t stUtfChar_t;
#elif(defined(__linux__) || defined(__linux))
  typedef char stUtfChar_t;
  #define ST_TEXT(quote) quote
#endif
typedef stUtfChar_t stUtf_t; // just short alias

namespace st
{
#if defined(_WIN32)
  typedef std::wofstream ofstream;
  typedef std::wostream ostream;
  static std::wostream& cout = std::wcout;
  static std::wostream& cerr = std::wcerr;
#else
  typedef std::ofstream ofstream;
  typedef std::ostream ostream;
  static std::ostream& cout = std::cout;
  static std::ostream& cerr = std::cerr;
#endif

  int getch();

  inline st::ostream& SYS_PAUSE_EMPTY(st::ostream& theStream)
  {
    st::getch();
    return theStream;
  }

  inline st::ostream& SYS_PAUSE (st::ostream& theStream)
  {
    st::cout << ST_TEXT("Press any key to continue...");
    st::getch();
    st::cout << ST_TEXT("\n");
    return theStream;
  }

  // console output text-color functions
  st::ostream& COLOR_FOR_RED(st::ostream& s);
  st::ostream& COLOR_FOR_GREEN(st::ostream& s);
  st::ostream& COLOR_FOR_YELLOW_L(st::ostream& s);
  st::ostream& COLOR_FOR_YELLOW(st::ostream& s);
  st::ostream& COLOR_FOR_BLUE(st::ostream& s);
  st::ostream& COLOR_FOR_WHITE(st::ostream& s);

}

#endif //__stConsole_h__
