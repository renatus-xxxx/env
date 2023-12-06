zcc +msx -create-app -O3 -DAMALLOC -subtype=msxdos main.c libiot.c -bn ENV.COM -lm
del ENV.img
move /y ENV.COM bin
cd bin
copy dosformsx.dsk ENV.DSK
START /WAIT DISKMGR.exe -A -F -C ENV.DSK ENV.COM
cd ../
