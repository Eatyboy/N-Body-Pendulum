@echo off
set program=%1

if "%program%"=="nbody" (
	call "build\N_Body_Pendulum.exe"
) else if "%program%"=="single" (
	call "build\Single_Pendulum.exe"
) else (
	echo "wrong usage"
)
