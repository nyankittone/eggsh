include version_config.mk

release_flags := -O3 -DNDEBUG
debug_flags := -Og -g -ggdb
test_flags := -lcheck -DRUN_TESTS $(debug_flags)

program_name := eggsh
debug_program_name := eggsh-dbg
source_dir := src
builtins_source_dir := builtins_src
tests_source_dir := tests_src

object_dir := obj
normal_object_dir := $(object_dir)/normal
test_object_dir := $(object_dir)/test
debug_object_dir := $(object_dir)/debug

gperf_filename := $(builtins_source_dir)/builtins_map

sources := $(wildcard $(source_dir)/*.c)
builtins_sources := $(wildcard $(builtins_source_dir)/*.c) $(gperf_filename).c
tests_sources := $(wildcard $(tests_source_dir)/*.c)

objects := $(patsubst $(source_dir)/%.c,$(normal_object_dir)/%.o,$(sources)) $(patsubst $(builtins_source_dir)/%.c,$(normal_object_dir)/builtin_%.o,$(builtins_sources))
test_objects := $(patsubst $(source_dir)/%.c,$(test_object_dir)/%.o,$(sources)) $(patsubst $(builtins_source_dir)/%.c,$(test_object_dir)/builtin_%.o,$(builtins_sources)) $(patsubst $(tests_source_dir)/%.c,$(test_object_dir)/tests_%.o,$(tests_sources))
debug_objects := $(patsubst $(source_dir)/%.c,$(debug_object_dir)/%.o,$(sources)) $(patsubst $(builtins_source_dir)/%.c,$(debug_object_dir)/builtin_%.o,$(builtins_sources))

ifeq "$(prerelease)" "true"
	# TODO: shell command here has bashisms in it. Make this m,ore portable.
	program_version := $(program_version)-pre-$(shell find -type f -name '*.[ch]' ! -name 'builtins_map.c' | xargs cat | sha1sum | cut -c -12)
endif

CC := cc
CFLAGS := -std=c99 -lc -pedantic-errors -Wall -Iinclude -Ibuiltins_src -DPROGRAM_NAME=\"$(self_reporting_name)\" -DVERSION_STRING=\"$(program_version)\"

.PHONY: all clean release debug

all: release

release: $(program_name)

debug: $(debug_program_name)

$(program_name): $(objects) $(program_name).o
	$(CC) $^ $(CFLAGS) $(release_flags) -o $@

$(program_name).o: main.c
	$(CC) -c $< $(CFLAGS) $(release_flags) -o $@

$(debug_program_name): $(debug_objects) $(debug_program_name).o
	$(CC) $^ $(CFLAGS) $(debug_flags) -o $@

$(debug_program_name).o: main.c
	$(CC) -c $< $(CFLAGS) $(debug_flags) -o $@

tests: $(test_objects) tests.o
	$(CC) $^ $(CFLAGS) $(test_flags) -o $@

tests.o: tests.c
	$(CC) -c $< $(CFLAGS) $(test_flags) -o $@

$(normal_object_dir)/%.o: $(source_dir)/%.c | $(normal_object_dir)
	$(CC) $(CFLAGS) $(release_flags) -c $< -o $@

$(debug_object_dir)/%.o: $(source_dir)/%.c | $(debug_object_dir)
	$(CC) $(CFLAGS) $(debug_flags) -c $< -o $@

$(test_object_dir)/%.o: $(source_dir)/%.c | $(test_object_dir)
	$(CC) $(CFLAGS) $(test_flags) -c $< -o $@

$(test_object_dir)/tests_%.o: $(tests_source_dir)/%.c | $(test_object_dir)
	$(CC) $(CFLAGS) $(test_flags) -c $< -o $@

$(normal_object_dir)/builtin_%.o: $(builtins_source_dir)/%.c | $(normal_object_dir)
	$(CC) $(CFLAGS) $(release_flags) -c $< -o $@
	
$(debug_object_dir)/builtin_%.o: $(builtins_source_dir)/%.c | $(debug_object_dir)
	$(CC) $(CFLAGS) $(debug_flags) -c $< -o $@

$(test_object_dir)/builtin_%.o: $(builtins_source_dir)/%.c | $(test_object_dir)
	$(CC) $(CFLAGS) $(test_flags) -c $< -o $@

$(normal_object_dir):
	mkdir -p $@
	
$(debug_object_dir):
	mkdir -p $@

$(test_object_dir):
	mkdir -p $@

$(gperf_filename).c: $(gperf_filename).gperf
	gperf $< > $@

clean:
	rm -rf *.o $(object_dir) $(gperf_filename).c $(program_name) tests result $(debug_program_name)

