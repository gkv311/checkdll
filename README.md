CheckDLL - command line tool for checking DLL and EXE dependencies
=================================

CheckDLL is a compact utility for checking missing DLLs within installed application.
This tool is NOT a replacements for more advanced tools like Dependency Walker or others.
It is designed for *FAST* check of missing dependencies - by trying to actually load modules in usual way.

![checkDLL screenshot](/checkdll_screenshot.png)

The usage of utility is very simple - just put checkDLL64.exe (or checkDLL32.exe for 32-bit applications) into application folder and start it directly from Explorer.
The tool will try to load all libraries (.dll) and executables (.exe) within the folder and will print information about successfully loaded / failed modules.
This is also a simple way to check if application folder erroneously contains binaries with wrong CPU architecture (e.g. 32-bit libraries within 64-bit application).

Here is the main repository of the project:<br/>
https://github.com/gkv311/checkdll

## Licensing

See the [LICENSE](LICENSE.txt) file.
