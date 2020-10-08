::------------------------------ 自动提升运行权限 ------------------------------::
%1 %2
ver|find "5.">nul&&goto :Admin
mshta vbscript:createobject("shell.application").shellexecute("%~s0","goto :Admin","","runas",1)(window.close)&goto :eof
:Admin

mkdir bin
cd ./bin
mkdir Debug
mkdir Release
cd ..

mklink /D %~dp0bin\Debug\Res %~dp0Res
mklink /D %~dp0bin\Release\Res %~dp0Res

pause