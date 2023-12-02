CC = g++
SRC_DIR = ./
INCLUDEPATH = -I src

COMPRESS_DIR =build/
MAINSRC = $(notdir $(shell ls $(SRC_DIR)jlstest.cpp))
OBJ = $(patsubst %.cpp, %.o, $(MAINSRC))

CFLAGS = -O0 -g -fPIC -Werror

fblcd.out: $(OBJ)
	$(CC) $^ $(CFLAGS) -o $@
	mkdir $(COMPRESS_DIR) 
$(OBJ): %.o: $(SRC_DIR)%.cpp
	$(CC) -c $< $(INCLUDEPATH) $(CFLAGS) -o $@
NUMBER=01
SAMPLE=sample
FILENAME=$(SAMPLE)$(NUMBER)
FILE_DIR =res/
SRC=$(FILE_DIR)$(FILENAME)
run:fblcd.out
	./fblcd.out
.PHONY: clean
clean :
	rm -f *.out
	rm -f *.o
	rm -rf ./build 
