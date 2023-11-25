CC = gcc
CFLAGS = -Wall -Wextra -O2  

SRC = cbm2bmp.c
EXE = cbm2bmp
all: $(EXE)
$(EXE): $(SRC)
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -f $(EXE)
.PHONY: all clean
