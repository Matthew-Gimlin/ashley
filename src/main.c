#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ashley.h"

#define USAGE "usage: ashley [-h] [-v] -e string | file\n"
#define OPTIONS "options:\n" \
    "  -h, --help           show the help message and exit\n" \
    "  -v, --version        show the version and exit\n" \
    "  -e string, --exec string\n" \
    "                       execute the string\n"

typedef struct {
    const char* filename;
    const char* string;
} args_t;

static inline bool is_opt(const char* arg, const char* opt, const char* long_opt) {
    return strcmp(arg, opt) == 0 || strcmp(arg, long_opt) == 0;
}

static const char* get_opt_arg(int argc, char** argv, int i) {
    if (i >= argc) return NULL;
    return argv[i];
}

static args_t parse_args(int argc, char** argv) {
    args_t args = {0};
    for (int i = 1; i < argc; i++) {
        if (is_opt(argv[i], "-h", "--help")) {
            printf(USAGE "\n" OPTIONS);
            exit(0);
        } else if (is_opt(argv[i], "-v", "--version")) {
            printf("%d.%d.%d\n", ASH_VERSION_MAJOR, ASH_VERSION_MINOR,
                    ASH_VERSION_PATCH);
            exit(0);
        } else if (is_opt(argv[i], "-e", "--exec")) {
            i++;
            args.string = get_opt_arg(argc, argv, i);
            if (!args.string) {
                fprintf(stderr, "error: missing string for option '%s'\n", argv[i - 1]);
                fprintf(stderr, USAGE);
                exit(0);
            }
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "error: unknown option '%s'\n", argv[i]);
            exit(1);
        } else {
            args.filename = argv[i];
        }
    }
    if (!args.filename && !args.string) {
        fprintf(stderr, "error: missing input\n");
        fprintf(stderr, USAGE);
        exit(1);
    }
    if (args.filename && args.string) {
        fprintf(stderr, "error: cannot input a file and a string\n");
        fprintf(stderr, USAGE);
        exit(1);
    }
    return args;
}

int main(int argc, char** argv) {
    args_t args = parse_args(argc, argv);
    ash_state_t* a = ash_state_new();

    if (args.filename) {
        if (ash_do_file(a, args.filename) != 0) {
            printf("error\n");
        }
    }
    else {
        if (ash_do_string(a, args.string) != 0) {
            printf("error\n");
        }
    }

    ash_state_free(a);
    return 0;
}
