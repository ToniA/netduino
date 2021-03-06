@echo off

IF NOT "" == "%1" SET GCC_VER=%1
IF "" == "%GCC_VER%" GOTO :ARG_ERROR

%~dp0\setenv_base.cmd GCC %GCC_VER% %2 %3 %4 %5

GOTO :EOF


:ARG_ERROR
@echo.
@echo ERROR: Invalid version argument.
@echo USAGE: setenv_gcc.cmd GCC_VERSION [GCC_TOOL_PATH]
@echo        where GCC_VERSION is (4.2.1, 4.5.2, 4.6.1, ...)
@echo.
