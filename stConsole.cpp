/**
 * Copyright © Kirill Gavrilov, 2009
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#include "stConsole.h"

#if defined(_WIN32)
  #include <windows.h>
  #include <conio.h>
#else
  #include <termios.h>
  #include <unistd.h>
  #include <stdio.h>
#endif

int st::getch()
{
#if defined(_WIN32)
  return _getch();
#else
  struct termios oldt;
  tcgetattr (STDIN_FILENO, &oldt);
  struct termios newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr (STDIN_FILENO, TCSANOW, &newt);
  int ch = getchar();
  tcsetattr (STDIN_FILENO, TCSANOW, &oldt);
  return ch;
#endif
}

// console output text-color functions
st::ostream& st::COLOR_FOR_RED(st::ostream& theStream)
{
#if defined(_WIN32)
  HANDLE hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute (hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
  theStream << ST_TEXT("\e[31m");
#endif
  return theStream;
}

st::ostream& st::COLOR_FOR_GREEN (st::ostream& theStream)
{
#if defined(_WIN32)
  HANDLE hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute (hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
  theStream << ST_TEXT("\e[32m");
#endif
  return theStream;
}

st::ostream& st::COLOR_FOR_YELLOW_L (st::ostream& theStream)
{
#if defined(_WIN32)
  HANDLE hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute (hStdout, FOREGROUND_GREEN | FOREGROUND_RED);
#else
  // no intence attribute for ISO...
  theStream << ST_TEXT("\e[33m");
#endif
  return theStream;
}

st::ostream& st::COLOR_FOR_YELLOW (st::ostream& theStream)
{
#if defined(_WIN32)
  HANDLE hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute (hStdout, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
  theStream << ST_TEXT("\e[33m");
#endif
  return theStream;
}

st::ostream& st::COLOR_FOR_BLUE (st::ostream& theStream)
{
#if defined(_WIN32)
  HANDLE hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute (hStdout, FOREGROUND_BLUE);
#else
  theStream << ST_TEXT("\e[34m");
#endif
  return theStream;
}

st::ostream& st::COLOR_FOR_WHITE(st::ostream& theStream)
{
#if defined(_WIN32)
  HANDLE hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
  // no default value on Win
  SetConsoleTextAttribute (hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else;
  theStream << ST_TEXT("\e[0m"); // set defaults
#endif
  return theStream;
}
