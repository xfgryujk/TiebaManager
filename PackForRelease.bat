@echo off

if not exist Release\TiebaManager.exe (
move Release\TiebaManager¾É°æ.exe Release\TiebaManager.exe >nul
move Release\TiebaManager¾É°æ.pdb Release\TiebaManager.pdb >nul
)
if not exist Release\Helper.dll (
move Release\Helper¾É°æ.dll Release\Helper.dll >nul
move Release\Helper¾É°æ.pdb Release\Helper.pdb >nul
)
if not exist Release\TiebaAPI.dll (
move Release\TiebaAPI¾É°æ.dll Release\TiebaAPI.dll >nul
move Release\TiebaAPI¾É°æ.pdb Release\TiebaAPI.pdb >nul
)
if not exist Release\TiebaManagerCore.dll (
move Release\TiebaManagerCore¾É°æ.dll Release\TiebaManagerCore.dll >nul
move Release\TiebaManagerCore¾É°æ.pdb Release\TiebaManagerCore.pdb >nul
)

ren ·¢²¼ÓÃ Ìù°É¹ÜÀíÆ÷
move Release\TiebaManager.exe Ìù°É¹ÜÀíÆ÷ >nul
move Release\Helper.dll Ìù°É¹ÜÀíÆ÷ >nul
move Release\TiebaAPI.dll Ìù°É¹ÜÀíÆ÷ >nul
move Release\TiebaManagerCore.dll Ìù°É¹ÜÀíÆ÷ >nul
del Ìù°É¹ÜÀíÆ÷.zip >nul 2>nul
"F:\Program Files\7-Zip\7z.exe" a -tzip Ìù°É¹ÜÀíÆ÷ Ìù°É¹ÜÀíÆ÷

:Restore
ping 127.0.0.1 -n 3 >nul
move Ìù°É¹ÜÀíÆ÷\TiebaManager.exe Release\TiebaManager¾É°æ.exe >nul 2>nul || goto Restore
move Release\TiebaManager.pdb Release\TiebaManager¾É°æ.pdb >nul 2>nul || goto Restore
move Ìù°É¹ÜÀíÆ÷\Helper.dll Release\Helper¾É°æ.dll >nul 2>nul || goto Restore
move Release\Helper.pdb Release\Helper¾É°æ.pdb >nul 2>nul || goto Restore
move Ìù°É¹ÜÀíÆ÷\TiebaAPI.dll Release\TiebaAPI¾É°æ.dll >nul 2>nul || goto Restore
move Release\TiebaAPI.pdb Release\TiebaAPI¾É°æ.pdb >nul 2>nul || goto Restore
move Ìù°É¹ÜÀíÆ÷\TiebaManagerCore.dll Release\TiebaManagerCore¾É°æ.dll >nul 2>nul || goto Restore
move Release\TiebaManagerCore.pdb Release\TiebaManagerCore¾É°æ.pdb >nul 2>nul || goto Restore
ren Ìù°É¹ÜÀíÆ÷ ·¢²¼ÓÃ 2>nul || goto Restore
