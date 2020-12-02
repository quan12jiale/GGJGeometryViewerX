@echo off
cd %~dp0
set tools=%~dp0
if not exist "%tools%\..\..\build" md "%tools%\..\..\build"

::VS平台
set vsVersion="Visual Studio 14 2015"
::set vsVersion="Visual Studio 15 2017"
::set vsVersion="Visual Studio 16 2019"

::配置
set config=RelWithDebInfo
::set config=Release

::解决方案名字
set product=GGJGeometryViewer

::平台
set platform=Win32
::set platform=x64

::cmakelists.txt所在目录
set builderDir=%~dp0\..\..\source\cmake\

::输出解决方案目录
set cmakeOutputDir=%~dp0\..\..\build\Sln\%platform%\%config%

cmake -S %builderDir% -B %cmakeOutputDir% -G%vsVersion% -A %platform% -D CMAKE_CONFIGURATION_TYPES=%config% -D version=%product%

pause