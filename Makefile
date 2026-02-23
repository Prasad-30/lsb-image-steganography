OBJ:= $(patsubst %.c, %.o, $(wildcard *.c))
output.exe : $(OBJ)
	gcc -o $@ $^
clean:
	rm *.o *.exe