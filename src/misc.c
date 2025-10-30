#include <misc.h>
#include <build_globals.h>

// TODO: Update function to use colorscheme specified by config
inline void printVersionInfo(FILE *const output_file) {
    fprintf(output_file, "\33[1;96m%s\33[m, version \33[1;94m%s\33[m\n"
            "Created by nyankittone \33[1;95m<3\33[m (https://github.com/nyankittone)\n\n",
            build.program_name, build.version_string
    );
}

