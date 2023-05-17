CC = g++
SRC_DIR = src/
INCLUDEPATH = -I src

COMPRESS_DIR =build/
MAINSRC = $(notdir $(shell ls $(SRC_DIR)*.cpp))
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
en:fblcd.out
	./fblcd.out -en $(SRC).bmp $(COMPRESS_DIR)$(SAMPLE)$(NUMBER) 
de: fblcd.out	
	./fblcd.out -de $(COMPRESS_DIR)$(FILENAME) $(COMPRESS_DIR)$(SAMPLE)$(NUMBER)de.bmp 
version:fblcd.out
	./fblcd.out --version 
ende:fblcd.out
	make en 
	make de 

.PHONY: clean
clean :
	rm -f *.out
	rm -f *.o
	rm -rf ./build 
