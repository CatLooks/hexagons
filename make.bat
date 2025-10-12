@echo off
if "%1" equ "" (
	echo Usage:
	echo.
	echo make delete   - Deletes the build environment.
	echo make generate - Generates a build environment.
	echo make build    - Builds the project.
	echo make run      - Runs the executable.
	echo make syntax   - Exports Sublime Text syntax files.
	exit /b
)
if "%1" equ "delete" (
	if not exist build (
		echo Not build environment detected.
		exit /b
	)
	echo Deleting the build environment...
	rmdir /s /q build
	echo Done.
	exit /b
)
if "%1" equ "generate" (
	if not exist build mkdir build
	echo Generating a build environment...
	cmake -S . build
	echo Done.
	exit /b
)
if "%1" equ "build" (
	if not exist build (
		echo Build environment is not generated.
		exit /b
	)
	echo Building the project...
	cmake --build build
	echo Done.
	exit /b
)
if "%1" equ "run" (
	if not exist build (
		echo Build environment is not generated.
		exit /b
	)
	if not exist build\bin\Debug (
		echo Build "Debug" does not exist.
		exit /b
	)
	build\bin\Debug\main
	exit /b
)
if "%1" equ "syntax" (
	copy /y TLML.sublime-syntax "%appdata%\Sublime Text 3\Packages\User\TLML.sublime-syntax"
	exit /b
)
