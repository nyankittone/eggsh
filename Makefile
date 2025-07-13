CC := cc
CFLAGS := -std=c99 -lc -pedantic-errors -Wall -Iinclude
test_flags := -lcheck -DRUN_TESTS

program_name := eggsh
source_dir := src
object_dir := obj
test_object_dir := test_obj

sources := $(wildcard $(source_dir)/*.c)
objects := $(patsubst $(source_dir)/%.c,$(object_dir)/%.o,$(sources))
test_objects := $(patsubst $(source_dir)/%.c,$(test_object_dir)/%.o,$(sources))

.PHONY: all clean

all: $(program_name)

$(program_name): $(objects) $(program_name).o 
	$(CC) $^ $(CFLAGS) -o $@

$(program_name).o: main.c
	$(CC) -c $< $(CFLAGS) -o $@

tests: $(test_objects) tests.o
	$(CC) $^ $(CFLAGS) $(test_flags) -o $@

tests.o: tests.c
	$(CC) -c $< $(CFLAGS) $(test_flags) -o $@

$(object_dir)/%.o: $(source_dir)/%.c | $(object_dir)
	$(CC) $(CFLAGS) -c $< -o $@

$(test_object_dir)/%.o: $(source_dir)/%.c | $(test_object_dir)
	$(CC) $(CFLAGS) $(test_flags) -c $< -o $@

$(object_dir):
	mkdir $@

$(test_object_dir):
	mkdir $@

clean:
	rm -rf *.o $(object_dir) $(test_object_dir) $(program_name) tests result

