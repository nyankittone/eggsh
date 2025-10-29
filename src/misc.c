#include <misc.h>

// TODO: UIpdate function to use colorscheme specified by config
inline void printVersionInfo(FILE *const output_file) {
    fputs("\33[1;96m"PROGRAM_NAME"\33[m, version \33[1;94m"VERSION_STRING"\33[m\n"
        "Created by nyankittone \33[1;95m<3\33[m (https://github.com/nyankittone)\n\n"
    , output_file);
}

