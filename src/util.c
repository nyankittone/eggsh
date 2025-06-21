#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <util.h>

const char OUT_OF_MEM_MESSAGE[] = "Out of memory!";

void panic(int exit_code, const char *const message_format, ...) {
    va_list message_list;
    va_start(message_list,message_format);

    fputs("\33[1;91mfatal:\33[m ",stderr);
    vfprintf(stderr,message_format,message_list);
    fputc('\n',stderr);

    va_end(message_list);
    exit(exit_code);
}

void OOMPanic(void) {
    panic(EXIT_OUT_OF_MEMORY, "%s: %s", FATAL_ERROR_TEXT, OUT_OF_MEM_MESSAGE);
}

void *reallocOrDie(void *old_block, size_t new_allocated_size) {
    void *new_block = realloc(old_block,new_allocated_size);
    if(!new_block) OOMPanic();

    return new_block;
}

void *mallocOrDie(size_t new_allocated_size) {
    void *new_block = malloc(new_allocated_size);
    if(!new_block) OOMPanic();

    return new_block;
}
