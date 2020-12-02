@echo off

set ResPath=%1
set ResPath=%ResPath:\=/%
set DestPath=%2
set DestPath=%DestPath:\=/%

::set ResPath=F:\GTJ\tools\GGJGeometryViewerX\resource
::set DestPath=F:\GTJ\tools\GGJGeometryViewerX\build\bin\Win32\RelWithDebInfo

if not exist "%DestPath%\image" md "%DestPath%\image"

XCopy /Y "%ResPath%\image" "%DestPath%\image\"
Copy /Y "%ResPath%\GGDBHelper.dll" "%DestPath%\"

pause
