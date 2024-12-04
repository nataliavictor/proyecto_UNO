# General variables
CC = gcc
CFLAGS = -Wall `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`
EXE = exe

# Targets
p1:
	$(CC) -o $(EXE) $(CFLAGS) juego_UNO.c $(LDFLAGS)

clean:
	rm -f $(EXE)

