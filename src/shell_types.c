#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <argument_parser.h>

static ConverterResult toInteger(char *const arg, void *const output) {
    assert(arg != NULL);
    assert(output != NULL);

    ConverterResult returned;
    returned.is_ok = true;

    char *end;
    errno = 0;
    const long long result = strtoll(arg, &end, 10);
    if(errno || *(end + strspn(end, " \t\n"))) {
        returned.is_ok = false;
        returned.on_fail = 69; // A very nice placeholder value
        // TODO: Add REAL error handling, eventually, some day.
    }

    *(long long*) output = result;
    return returned;
}

static ConverterResult toUnsignedInteger(char *const arg, void *const output) {
    assert(arg != NULL);
    assert(output != NULL);

    ConverterResult returned;
    returned.is_ok = true;

    char *end;
    errno = 0;
    const unsigned long long result = strtoull(arg, &end, 10);
    if(errno || *(end + strspn(end, " \t\n"))) {
        returned.is_ok = false;
        returned.on_fail = 420; // A very nice placeholder value
        // TODO: Add REAL error handling, eventually, some day.
    }

    *(unsigned long long*) output = result;
    return returned;
}

static ConverterResult toFloat(char *const arg, void *const output) {
    assert(arg != NULL);
    assert(output != NULL);

    ConverterResult returned;
    returned.is_ok = true;

    char *end;
    errno = 0;
    const double result = strtod(arg, &end);
    if(errno || *(end + strspn(end, " \t\n"))) {
        returned.is_ok = false;
        returned.on_fail = 1337; // A very nice placeholder value
        // TODO: Add REAL error handling, eventually, some day.
    }

    *(double*) output = result;
    return returned;
}

static ConverterResult toString(char *const arg, void *const output) {
    assert(arg != NULL);
    assert(output != NULL);

    ConverterResult returned;
    returned.is_ok = true;

    *(char**)output = arg;

    return returned;
}

ShellType shell_type_int = {
    .name = "Integer",
    .short_name = "int",
    .highlighting = NULL,
    .converter = &toInteger,
};

ShellType shell_type_uint = {
    .name = "Unsigned Integer",
    .short_name = "uint",
    .highlighting = NULL,
    .converter = &toUnsignedInteger,
};

ShellType shell_type_float = {
    .name = "Float",
    .short_name = "float",
    .highlighting = NULL,
    .converter = &toFloat,
};

ShellType shell_type_string = {
    .name = "String",
    .short_name = NULL,
    .highlighting = NULL,
    .converter = &toString,
};

