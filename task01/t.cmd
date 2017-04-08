@echo off
call cc /FI %1.h /DWHO=%1 test01.cpp /Fe%1.exe %2 %3 %4
%1.exe
