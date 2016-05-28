@echo off

ren 发布用 贴吧管理器
copy /y Release\TiebaManager.exe 贴吧管理器
copy /y Release\Helper.dll 贴吧管理器
copy /y Release\TiebaAPI.dll 贴吧管理器
copy /y Release\TiebaManagerCore.dll 贴吧管理器
copy /y Release\Update.dll 贴吧管理器
copy /y Release\plugin\*.dll 贴吧管理器\plugin
del 贴吧管理器.zip >nul 2>nul
"F:\Program Files\7-Zip\7z.exe" a -tzip 贴吧管理器 贴吧管理器
ping 127.0.0.1 -n 3 >nul

set VER=%date:~2,8%
set VER=%VER:/=-%
set BACKUPDIR=历史备份\%VER%
md %BACKUPDIR% 2>nul
md %BACKUPDIR%\plugin 2>nul

move /y 贴吧管理器\TiebaManager.exe %BACKUPDIR%
move /y 贴吧管理器\Helper.dll %BACKUPDIR%
move /y 贴吧管理器\TiebaAPI.dll %BACKUPDIR%
move /y 贴吧管理器\TiebaManagerCore.dll %BACKUPDIR%
move /y 贴吧管理器\Update.dll %BACKUPDIR%
move /y 贴吧管理器\plugin\*.dll %BACKUPDIR%\plugin
copy /y Release\*.pdb %BACKUPDIR%
copy /y Release\plugin\*.pdb %BACKUPDIR%\plugin

ren 贴吧管理器 发布用

pause
