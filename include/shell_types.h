#pragma once
#include <argument_parser.h>

// We need types for at least:
// integer (do I want 32-bit and 64-bit ones or nah?)
// unsigned integer
// floating point (probably double precision)
// string (will I need to copy the string parameter or not? probably not, thankully)

// I want to have the highlighting codes be re-definable at runtime. How do I tackle that exactly?
// I want highlighting for the following:
// * data types
// * parts of the help text
// * warnings and errors in the typed command while in interactive mode (eventually)
// * information provided by eggsh and internal commands via --version
// Should I have the color variables be actual shell variables? I think that would be a good idea.
// Should variables also support the concept of "pointers" to other variables? I have no fucking
// clue.
// Also, mayyybe we could add a full type system into the interpreter??? But that sounds like I may
// be jumping the shark.

extern ShellType shell_type_int;
extern ShellType shell_type_uint;
extern ShellType shell_type_float;
extern ShellType shell_type_string;

