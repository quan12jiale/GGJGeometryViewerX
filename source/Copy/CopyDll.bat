@echo off

set QtPath=%1
set QtPath=%QtPath:\=/%
set GTJPath=%2
set GTJPath=%GTJPath:\=/%
set DestPath=%3
set DestPath=%DestPath:\=/%

::set QtPath=F:\GTJ\external\qt563\msvc2015
::set GTJPath=F:\GTJ\build\bin\Win32\RelWithDebInfo
::set DestPath=F:\GTJ\tools\GGJGeometryViewerX\build\bin\Win32\RelWithDebInfo

if not exist "%DestPath%\platforms" md "%DestPath%\platforms"

Copy /Y "%GTJPath%\libGLESv2.dll" "%DestPath%\"
Copy /Y "%GTJPath%\ViewManager.dll" "%DestPath%\"
Copy /Y "%GTJPath%\GMath.dll" "%DestPath%\"
Copy /Y "%GTJPath%\GGDB.dll" "%DestPath%\"
Copy /Y "%GTJPath%\api-ms-win-crt-string-l1-1-0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\GP.dll" "%DestPath%\"
Copy /Y "%GTJPath%\api-ms-win-crt-convert-l1-1-0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\dbghelp.dll" "%DestPath%\"
Copy /Y "%GTJPath%\MDScript.dll" "%DestPath%\"
Copy /Y "%GTJPath%\Geometry.dll" "%DestPath%\"
Copy /Y "%GTJPath%\api-ms-win-crt-utility-l1-1-0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\lib3ds-2_0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\libfbxsdk.dll" "%DestPath%\"
Copy /Y "%GTJPath%\ViewCore.dll" "%DestPath%\"
Copy /Y "%GTJPath%\api-ms-win-crt-multibyte-l1-1-0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\api-ms-win-crt-environment-l1-1-0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\VCRUNTIME140.dll" "%DestPath%\"
Copy /Y "%GTJPath%\MDCmdLog.dll" "%DestPath%\"
Copy /Y "%GTJPath%\api-ms-win-crt-stdio-l1-1-0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\api-ms-win-crt-math-l1-1-0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\assimp-vc140-mt.dll" "%DestPath%\"
Copy /Y "%GTJPath%\RenderSystemGL.dll" "%DestPath%\"
Copy /Y "%GTJPath%\sdt.dll" "%DestPath%\"
Copy /Y "%GTJPath%\sdt64.dll" "%DestPath%\"
Copy /Y "%GTJPath%\api-ms-win-crt-heap-l1-1-0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\MDCache.dll" "%DestPath%\"
Copy /Y "%GTJPath%\RenderSystemAngle.dll" "%DestPath%\"
Copy /Y "%GTJPath%\api-ms-win-crt-filesystem-l1-1-0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\api-ms-win-crt-time-l1-1-0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\Qt5Gui.dll" "%DestPath%\"
Copy /Y "%GTJPath%\Qt5Core.dll" "%DestPath%\"
Copy /Y "%GTJPath%\MSVCR100.dll" "%DestPath%\"
Copy /Y "%GTJPath%\libEGL.dll" "%DestPath%\"
Copy /Y "%GTJPath%\glew32.dll" "%DestPath%\"
Copy /Y "%GTJPath%\CmdCore.dll" "%DestPath%\"
Copy /Y "%GTJPath%\glewctx.dll" "%DestPath%\"
Copy /Y "%GTJPath%\Qt5Widgets.dll" "%DestPath%\"
Copy /Y "%GTJPath%\Common.dll" "%DestPath%\"
Copy /Y "%GTJPath%\MSVCP100.dll" "%DestPath%\"
Copy /Y "%GTJPath%\api-ms-win-crt-locale-l1-1-0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\MSVCP140.dll" "%DestPath%\"
Copy /Y "%GTJPath%\api-ms-win-crt-runtime-l1-1-0.dll" "%DestPath%\"
Copy /Y "%GTJPath%\FreeImage.dll" "%DestPath%\"
Copy /Y "%GTJPath%\MDCommon.dll" "%DestPath%\"
Copy /Y "%GTJPath%\CGBase.dll" "%DestPath%\"
Copy /Y "%GTJPath%\MemoryChunkPager.dll" "%DestPath%\"

Copy /Y "%QtPath%\plugins\platforms\qminimal.dll" "%DestPath%\platforms\"
Copy /Y "%QtPath%\plugins\platforms\qoffscreen.dll" "%DestPath%\platforms\"
Copy /Y "%QtPath%\plugins\platforms\qwindows.dll" "%DestPath%\platforms\"

pause
