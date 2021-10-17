gnu:
	gcc -o test -fvolatile test.c gc.c

borland:
	bcc32 -r- test.c gc.c

testdos:
	test.exe

testunix:
	./test

cleandos:
	del test.exe
	del *.o
	del *.obj
	del *.tds
	del test

cleanunix:
	rm test
	rm *.o