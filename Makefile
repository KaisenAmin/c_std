CC = gcc
CFLAGS = -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -Wno-deprecated-declarations -std=c17 -s
LDFLAGS = -lssl -lcrypto
# Add any other libraries you need to link against

# Directories containing header files
INCLUDE_DIRS = -Ialgorithm -Iarray -Ibitset -Iconfig -Icrypto -Icsv -Idate -Ideque -Idir -Iencoding -Ifile_io -Ifmt -Iforward_list -Ijson -Ilist -Imap -Inumbers -Inumeric -Ipriority_queue -Iqueue -Ispan -Istack -Istring -Itime -Ituple -Ivector -Idependency/include

# Source files
SOURCES = main.c \
		  algorithm/algorithm.c \
		  array/array.c \
		  bitset/bitset.c \
		  config/config.c \
		  crypto/crypto.c \
		  csv/csv.c \
		  date/date.c \
		  deque/deque.c \
		  dir/dir.c \
		  encoding/encoding.c \
		  file_io/file_reader.c \
		  file_io/file_writer.c \
		  fmt/fmt.c \
		  forward_list/forward_list.c \
		  json/json.c \
		  list/list.c \
		  map/map.c \
		  numeric/numeric.c \
		  priority_queue/priority_queue.c \
		  queue/queue.c \
		  span/span.c \
		  stack/stack.c \
		  string/string.c \
		  time/time.c \
		  tuple/tuple.c \
		  vector/vector.c
# Remove or correct any other .c files that don't exist


# Object files
OBJECTS = $(SOURCES:.c=.o)

# Output executable
EXECUTABLE = CStdProject

# Windows-specific adjustments
ifeq ($(OS),Windows_NT)
	LDFLAGS += -lAdvapi32
	ifeq ($(wildcard dir/*.c),)
		LDFLAGS += -lshlwapi
	endif
	RM = del /Q /F
else
	RM = rm -f
endif

all: $(SOURCES) $(EXECUTABLE)
	@echo Building...

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	-$(RM) main.o
	-$(RM) algorithm\algorithm.o
	-$(RM) array\array.o
	-$(RM) bitset\bitset.o
	-$(RM) config\config.o
	-$(RM) crypto\crypto.o
	-$(RM) csv\csv.o
	-$(RM) date\date.o
	-$(RM) deque\deque.o
	-$(RM) dir\dir.o
	-$(RM) encoding\encoding.o
	-$(RM) file_io\file_reader.o
	-$(RM) file_io\file_writer.o
	-$(RM) fmt\fmt.o
	-$(RM) forward_list\forward_list.o
	-$(RM) json\json.o
	-$(RM) list\list.o
	-$(RM) map\map.o
	-$(RM) numeric\numeric.o
	-$(RM) priority_queue\priority_queue.o
	-$(RM) queue\queue.o
	-$(RM) span\span.o
	-$(RM) stack\stack.o
	-$(RM) string\string.o
	-$(RM) time\time.o
	-$(RM) tuple\tuple.o
	-$(RM) vector\vector.o
	-$(RM) $(EXECUTABLE).exe

