CC = clang
CFLAGS = -Wall -Wextra
SRC = main.c
EXEC = out

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

clean:
	rm -f $(EXEC)

