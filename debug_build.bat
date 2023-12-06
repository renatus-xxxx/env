zcc +msx -create-app -O0 -m -debug -DAMALLOC -subtype=msxdos -lm -v -bn ENV.COM main.c libiot.c 
rename ENV.img ENV.DSK
move /y ENV.DSK bin
cd bin
START /WAIT DISKMGR.exe -A -F -C ENV.DSK COMMAND.COM
START /WAIT DISKMGR.exe -A -F -C ENV.DSK DOS.SYS
cd ../