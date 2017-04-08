@echo off
del /q *.exe
for %%f in (*.h) do (
	call cc /w /FI %%~nf.h /DWHO=%%~nf test01.cpp /Fe%%~nf.exe
)
