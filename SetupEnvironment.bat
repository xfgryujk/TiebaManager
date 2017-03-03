@echo off

copy 3rdParty\libcurl\bin\*.dll 贴吧管理器
copy 3rdParty\tinyxml2\bin\Release\*.dll 贴吧管理器
copy 3rdParty\opencv\bin\Release\*.dll 贴吧管理器

md Debug
copy 3rdParty\libcurl\bin\*.dll Debug
copy 3rdParty\tinyxml2\bin\Debug\*.dll Debug
copy 3rdParty\opencv\bin\Debug\*.dll Debug

md Release
copy 3rdParty\libcurl\bin\*.dll Release
copy 3rdParty\tinyxml2\bin\Release\*.dll Release
copy 3rdParty\opencv\bin\Release\*.dll Release

pause
