#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_element(int pflag, int nflag, const char *buffer) {
    char path[256];
    char line_number_str[50];
    int line_number = 0;
    char object_name[256];
    
    char *start_ptr = strdup(buffer);
    
    char *ptr = strchr(start_ptr, ':'); 
    *ptr = '\0';
    strcpy(path, start_ptr);
    
    start_ptr = ptr + 1;
    ptr = strchr(start_ptr, ':'); 
    *ptr = '\0';
    strcpy(line_number_str, start_ptr);
    line_number = atoi(line_number_str);

    start_ptr = ptr + 1;
    ptr = strchr(start_ptr, ' ');
    start_ptr = ptr + 1;
    ptr = strchr(start_ptr, '(');
    *ptr = '\0';
    strcpy(object_name, start_ptr);
    // problems with 'async def'.
    // e.g. `cgmd.o '.*' -n`.

    if (pflag && nflag) {
        printf("%s:%d %s\n", path, line_number, object_name);
    } else if (pflag) {
        printf("%s:%d\n", path, line_number);
    } else if (nflag) {
        printf("%s\n", object_name);
    } else {

    }
}

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
    
    if (cflag && fflag) {
        fprintf(
            stderr,
            "ERROR: Cannot use both -c and -f options at the same time.\n"
        );
        return 1;
    }

    // -s flag
    char sensitive[4] = "";
    if (!sflag) {
        strcpy(sensitive, "-i "); 
    }
    // -c and -f flags
    char query[256];
    if (cflag) {
        snprintf(query, sizeof(query), "\"class %s(\"", search_pattern);
    } else if (fflag) {
        snprintf(query, sizeof(query), "\"def %s(\"", search_pattern);
    } else {
        snprintf(
            query,
            sizeof(query),
            " -e \"def %s(\" --or -e \"class %s(\"",
            search_pattern,
            search_pattern
        );
    }

    char command[1024];
    snprintf(
        command,
        sizeof(command),
        "git grep -n %s%s -- '*.py'",
        sensitive,
        query
    );

    // ----------------------------------

    
    FILE *fp = popen(command, "r");
    
    if (fp == NULL) {
        perror("popen failed");
        return 1;
    }
    
    // Buffer to hold each line of output
    char buffer[1024];
    // Read output a line at a time
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Process the output line (for now, just print it)
        print_element(pflag, nflag, buffer);
    }
    
    // Close the process
    if (pclose(fp) == -1) {
        perror("pclose failed");
        return 1;
    }


    return 0;
}
