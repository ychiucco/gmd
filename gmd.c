#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void print_element(int pflag, int nflag, const char *buffer) {

    char *start_ptr = strdup(buffer);
    // start_ptr -> "PATH:LINE:{INDENTATION}[def/async def/class]{spaces}NAME(..."
    // extract PATH
    char PATH[256];
    char *ptr = strchr(start_ptr, ':'); 
    *ptr = '\0';
    strcpy(PATH, start_ptr);
    start_ptr = ptr + 1;

    // start_ptr -> "LINE:{INDENTATION}[def/async def/class]{spaces}NAME(..."
    // extract LINE
    char LINE_str[50];
    ptr = strchr(start_ptr, ':'); 
    *ptr = '\0';
    strcpy(LINE_str, start_ptr);
    int LINE = atoi(LINE_str);
    start_ptr = ptr + 1;

    // start_ptr -> "{INDENTATION}[def/async def/class]{spaces}NAME(..."
    // extract INDENTATION
    int INDENTATION = 0;
    while (isspace(*start_ptr)) {
        INDENTATION++;
        start_ptr++;
    }

    // start_ptr -> "[def/async def/class]{spaces}NAME(..."
    // extract [def/async def/class], i.e. CATEGORY
    char CATEGORY[10];
    const char* def_str = "def";
    const char* async_def_str = "async def";
    const char* class_str = "class";
    if (strncmp(start_ptr, async_def_str, strlen(async_def_str)) == 0) {
        strcpy(CATEGORY, async_def_str);
        start_ptr += strlen(async_def_str);
    } else if (strncmp(start_ptr, def_str, strlen(def_str)) == 0) {
        strcpy(CATEGORY, def_str);
        start_ptr += strlen(def_str);
    } else if (strncmp(start_ptr, class_str, strlen(class_str)) == 0) {
        strcpy(CATEGORY, class_str);
        start_ptr += strlen(class_str);
    } else {
        // ignore grepped edge cases like 'print("def class ()")'
        return;
    }

    // start_ptr -> "{spaces}NAME(..."
    // ignore spaces
    while (isspace(*start_ptr)) {
        start_ptr++;
    }

    // start_ptr -> "NAME(..."
    // extract NAME
    char NAME[256];
    ptr = strchr(start_ptr, '(');
    *ptr = '\0';
    strcpy(NAME, start_ptr);

    if (pflag && nflag) {
        printf("%s:%d %s %s\n", PATH, LINE, CATEGORY, NAME);
    } else if (pflag) {
        printf("%s:%d\n", PATH, LINE);
    } else if (nflag) {
        printf("%s%s\n", CATEGORY, NAME);
    } else {
        printf("%d %s %s\n", INDENTATION, CATEGORY, NAME);
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
