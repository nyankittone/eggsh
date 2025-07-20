CC := cc
CFLAGS := -std=c99 -lc -pedantic-errors -Wall -Iinclude -Ibuiltins_src
test_flags := -lcheck -DRUN_TESTS

program_name := eggsh
source_dir := src
builtins_source_dir := builtins_src

object_dir := obj
normal_object_dir := $(object_dir)/normal
test_object_dir := $(object_dir)/test

sources := $(wildcard $(source_dir)/*.c)
builtins_sources := $(wildcard $(builtins_source_dir)/*.c)
# builtins_sources := $(filter-out $(builtins_source_dir)/builtin_commands.c, $(builtins_sources))
# builtins_sources := $(filter-out $(builtins_source_dir)/builtins_map.c, $(builtins_sources))

objects := $(patsubst $(source_dir)/%.c,$(normal_object_dir)/%.o,$(sources)) $(patsubst $(builtins_source_dir)/%.c,$(normal_object_dir)/builtin_%.o,$(builtins_sources))
test_objects := $(patsubst $(source_dir)/%.c,$(test_object_dir)/%.o,$(sources))

.PHONY: all clean

all: $(program_name)
	@echo $(builtins_sources)

$(program_name): $(objects) $(program_name).o
	$(CC) $^ $(CFLAGS) -o $@

$(program_name).o: main.c
	$(CC) -c $< $(CFLAGS) -o $@

tests: $(test_objects) tests.o
	$(CC) $^ $(CFLAGS) $(test_flags) -o $@

tests.o: tests.c
	$(CC) -c $< $(CFLAGS) $(test_flags) -o $@

$(normal_object_dir)/%.o: $(source_dir)/%.c | $(normal_object_dir)
	$(CC) $(CFLAGS) -c $< -o $@

$(test_object_dir)/%.o: $(source_dir)/%.c | $(test_object_dir)
	$(CC) $(CFLAGS) $(test_flags) -c $< -o $@

$(normal_object_dir)/builtin_%.o: $(builtins_source_dir)/%.c | $(normal_object_dir)
	$(CC) $(CFLAGS) -c $< -o $@

$(normal_object_dir):
	mkdir -p $@

$(test_object_dir):
	mkdir -p $@

clean:
	rm -rf *.o $(object_dir) $(program_name) tests result

