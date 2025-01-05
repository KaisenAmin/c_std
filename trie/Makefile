CC=clang
CFLAGS=-I.

# Define the names of your object files
OBJ = main.o trie.o

# Define your executable file name
EXEC = trie.bin

# The first rule is the one executed when no parameters are fed into the Makefile
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Clean up
.PHONY: clean
	rm *.o

clean:
	rm -f $(OBJ) $(EXEC)

