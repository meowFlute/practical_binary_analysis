CC 	= gcc -Wall -Wextra -g
INCLUDE = -I./
LIB 	= -lbfd
SRC 	= $(wildcard ./*.c)
OBJ 	= $(SRC:.c=.o)

loader : $(OBJ)
	$(CC) -o $@ $^ $(INCLUDE) $(LIB)

.PHONY : clean
.PHONY : clean_all

clean :
	rm $(OBJ)

clean_all : 
	rm $(OBJ) ./loader
