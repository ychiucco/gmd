#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    int cflag = 0;
    int hflag = 0;
    int fflag = 0;
    int sflag = 0;
    int pflag = 0;
    int nflag = 0;
    char* search_pattern = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--class") == 0) {
            cflag = 1;
        } else if (
            strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0
        ) {
            hflag = 1;
        } else if (
            strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--function") == 0
        ) {
            fflag = 1;
        } else if (
            strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--sensitive") == 0
        ) {
            sflag = 1;
        } else if (
            strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--path") == 0
        ) {
            pflag = 1;
        } else if (
            strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--name") == 0
        ) {
            nflag = 1;
        } else if (argv[i][0] != '-') {
            search_pattern = argv[i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 1;
        }
    }
    
    printf("-c: %d\n-f: %d\n-s: %d\n-p: %d\n-n: %d\narg: %s\n",
       cflag, fflag, sflag, pflag, nflag, search_pattern ? search_pattern : "NULL");

    return 0;
}
