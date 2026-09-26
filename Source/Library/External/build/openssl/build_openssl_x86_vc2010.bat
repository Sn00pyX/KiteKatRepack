call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
cd ..\..\..\..\..\openssl\
@set PATH=..\tools\perl\perl\bin\;..\tools\nasm\;%PATH%
perl Configure VC-WIN32 no-shared no-idea no-mdc2 no-rc5 --prefix=./build/
call ms\do_nasm.bat
notepad ms\nt.mak
nmake -f ms\nt.mak clean
nmake -f ms\nt.mak
copy inc32\openssl\* ..\..\include\openssl\*
copy out32\libeay32.lib ..\..\..\lib\vc2010\libeay32MT.lib
copy out32\ssleay32.lib ..\..\..\lib\vc2010\ssleay32MT.lib
copy tmp32\libeay32MT.pdb ..\..\..\lib\vc2010\libeay32MT.pdb
copy tmp32\ssleay32MT.pdb ..\..\..\lib\vc2010\ssleay32MT.pdb
cd ..
