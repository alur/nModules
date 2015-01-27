@echo off
mkdir temp
set SevenZip="%ProgramFiles%\7-Zip\7z.exe"
REM I know this has a regional dependendency :/
for /f "tokens=1-3 delims=/ " %%a in ('date /t') do (set dts=%%c-%%b-%%a)
for %%X in ("..\bin\Release_x64\*.dll") do (
	copy /Y %%X temp\%%~nX-0.7.0.1.dll
)
pushd temp
%SevenZip% a -mx=9 ..\builds\nModules-0.7.0.1-%dts%-x64.7z *.dll 
popd
rmdir /S /Q temp