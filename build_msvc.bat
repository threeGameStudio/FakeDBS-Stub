@echo off
setlocal

if not exist .build mkdir .build

cl /nologo /TC /W4 /Od /D_CRT_SECURE_NO_WARNINGS /Fo:.build\dbsbuild64.obj /Fd:.build\dbsbuild64.pdb /Fe:dbsbuild64.exe dbsbuild64.c /link /PDB:.build\dbsbuild64-link.pdb
if errorlevel 1 exit /b 1

cl /nologo /TC /W4 /Od /D_CRT_SECURE_NO_WARNINGS /Fo:.build\dbsutil.obj /Fd:.build\dbsutil.pdb /Fe:dbsutil.exe dbsutil.c /link /PDB:.build\dbsutil-link.pdb
if errorlevel 1 exit /b 1

cl /nologo /TC /W4 /Od /D_CRT_SECURE_NO_WARNINGS /Fo:.build\dbsview.obj /Fd:.build\dbsview.pdb /Fe:dbsview.exe dbsview.c /link /PDB:.build\dbsview-link.pdb /SUBSYSTEM:WINDOWS user32.lib
if errorlevel 1 exit /b 1

echo FakeDBS reconstruction built successfully.
