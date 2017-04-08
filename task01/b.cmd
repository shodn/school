@echo off
for %%f in (*.exe) do (
	echo %%~nf
	%%f > %%~nf.csv
)
