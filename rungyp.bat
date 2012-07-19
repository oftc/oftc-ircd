@echo off

SET GYP=c:\dev\gyp\gyp

%GYP% ircd.gyp --depth=. -Dlibrary=static_library -Icommon.gypi
