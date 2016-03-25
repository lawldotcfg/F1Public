@echo off
REM msbuild batch file for building this project when working in atom

REM call the vars all batch file
REM this will allow us to call msbuild
REM if you build with another version, change this to reflect it
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\vc\vcvarsall.bat"

REM run the actual build by calling msbuild with our soloution
msbuild ".\F1Public.sln" /verbosity:minimal /p:Configuration=Release /p:Platform=x86

REM wait for user input then exit
exit
