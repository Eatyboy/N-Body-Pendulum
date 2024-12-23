@echo off
set program=%1

mkdir build
pushd build

if "%program%"=="nbody" (
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" && cl ..\main.c /I \include /Zi /link /out:N_Body_Pendulum.exe /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /MACHINE:X64 ..\lib\raylib.lib msvcrt.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winmm.lib shell32.lib /NODEFAULTLIB:libcmt
) else if "%program%"=="single" (
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" && cl ..\single_pendulum.c /I \include /Zi /link /out:Single_Pendulum.exe /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /MACHINE:X64 ..\lib\raylib.lib msvcrt.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winmm.lib shell32.lib /NODEFAULTLIB:libcmt
) else if "%program%"=="double" (
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" && cl ..\double_pendulum.c /I \include /Zi /link /out:Double_Pendulum.exe /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /MACHINE:X64 ..\lib\raylib.lib msvcrt.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winmm.lib shell32.lib /NODEFAULTLIB:libcmt
) else (
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" && cl ..\main.c /I \include /Zi /link /out:N_Body_Pendulum.exe /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /MACHINE:X64 ..\lib\raylib.lib msvcrt.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winmm.lib shell32.lib /NODEFAULTLIB:libcmt
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" && cl ..\single_pendulum.c /I \include /Zi /link /out:Single_Pendulum.exe /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /MACHINE:X64 ..\lib\raylib.lib msvcrt.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winmm.lib shell32.lib /NODEFAULTLIB:libcmt
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" && cl ..\double_pendulum.c /I \include /Zi /link /out:Double_Pendulum.exe /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /MACHINE:X64 ..\lib\raylib.lib msvcrt.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winmm.lib shell32.lib /NODEFAULTLIB:libcmt
)
popd
