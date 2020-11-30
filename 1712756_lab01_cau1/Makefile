CC='g++'
PROJECT=Lab1
SRC=Lab1_1_2b.cpp

LIB= `pkg-config opencv4 --cflags --libs`
$(PROJECT) : $(SRC)
	$(CC) $(SRC) -o $(PROJECT) $(LIB)
