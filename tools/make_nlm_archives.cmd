@echo off
set SevenZip="%ProgramFiles%\7-Zip\7z.exe"

mkdir temp
pushd temp
for %%X in ("..\..\bin\Release_x64\*.dll") do (
	copy /Y %%X %%~nX-0.7.dll
	%SevenZip% a ..\nlm\64bit\%%~nX-0.7.zip %%~nX-0.7.dll
)
del /Q *.dll
for %%X in ("..\..\bin\Release_Win32\*.dll") do (
	copy /Y %%X %%~nX-0.7.dll
	%SevenZip% a ..\nlm\32bit\%%~nX-0.7.zip %%~nX-0.7.dll
)
popd
rmdir /S /Q temp