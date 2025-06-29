CC := cc
CFLAGS := -std=c99 -lc -pedantic-errors -Wall -Iinclude

program_name := eggsh
source_dir := src
object_dir := obj

sources := $(wildcard $(source_dir)/*.c)
objects := $(patsubst $(source_dir)/%.c,$(object_dir)/%.o,$(sources))

.PHONY: all clean

all: $(program_name)

$(program_name): $(objects) $(program_name).o 
	$(CC) $^ $(CFLAGS) -o $@

$(program_name).o: main.c
	$(CC) -c $< $(CFLAGS) -o $@

tests: $(objects) tests.o
	$(CC) $^ $(CFLAGS) -lcunit -o $@

tests.o: tests.c
	$(CC) -c $< $(CFLAGS) -o $@

$(object_dir)/%.o: $(source_dir)/%.c | $(object_dir)
	$(CC) $(CFLAGS) -c $< -o $@

$(object_dir):
	mkdir $@

clean:
	rm -rf *.o $(object_dir) $(program_name) tests result

