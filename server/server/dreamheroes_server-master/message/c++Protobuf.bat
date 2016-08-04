@echo off
setlocal enabledelayedexpansion
set tool=.\
set common_path=..\common\message\
set pbfile_path=.\
for /f %%i in ('dir /a-d /b *.proto') do ( 
echo %%i
%tool%protoc.exe --proto_path=%pbfile_path%  --cpp_out=%common_path%  %%i 
)
pause
