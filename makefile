all:
	arm-linux-gcc src/*.c -o bin/main -I inc/ -lpthread -L ~/jpg/lib/ -ljpeg -L /home/mangata/project/lib -lfont
