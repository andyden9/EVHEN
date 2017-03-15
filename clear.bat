set BATCHDIR=%~dp0
set CURDIR=%CD%
cd %BATCHDIR%
del /s /a:h *.suo
del /s *.sdf
del /s *.user
del /s *.ipch
del /s *.tlog
del /s *.idb
del /s *.pdb
del /s *.obj
del /s *.pch
del /s *.exp
del /s *.aps
del /s *.filters
del /s *.ilk
del /s /q debug
del /s /q release
del /s /q wb_creator\debug
del /s /q wb_creator\release
cd %CURDIR%