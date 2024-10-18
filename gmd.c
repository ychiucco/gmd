#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char __GMD_VERSION__[] = "1.0.0a2";

void
print_element(
    const int pflag,
    const int nflag,
    const int oflag,
    const char *buffer,
    const char *sep
)
{
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
    if (oflag) {
        char code_goto[1024];
        snprintf(
            code_goto, sizeof(code_goto), "code --goto %s:%d", PATH, LINE
        );
        system(code_goto);
        return;
    }
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
        printf("%s %s\n", CATEGORY, NAME);
    } else {
        printf("%s%s:%d\n%s", sep, PATH, LINE, sep);
        // open file at PATH
        FILE* file = fopen(PATH, "r");
        if (file == NULL) {
            perror("Error opening file");
            exit(1);
        }
        // we want INDENTATION+1 white spaces (+1 for '\0')
        int INDENT = INDENTATION + 1;
        char required_spaces[INDENT + 1];
        memset(required_spaces, ' ', INDENT);
        required_spaces[INDENT] = '\0';
        //  read file line by line
        int current_line = 1;
        char line[1024];
        while (fgets(line, sizeof(line), file)) {
            if (current_line < LINE) {
                ;
            }
            else if (current_line == LINE) {
                printf("%s", line);
            }
            else {
                if (strncmp(line, required_spaces, INDENT) == 0) {
                    printf("%s", line);
                } else {
                    ptr = line;
                    while (isspace(*ptr)) {
                        ptr++;
                    }
                    if (*ptr == '\0') {
                        // do not print empty lines
                        ;
                    }
                    else if (
                        strncmp(ptr, "):", 2) == 0 || strncmp(ptr, ") ->", 4) == 0
                    ) {
                        printf("%s", line);
                    }
                    else {
                        break;
                    }
                }
            }
            current_line++;
        }
        // Close the file
        fclose(file);
        printf("\n\n");
    }
}

int
main(int argc, char **argv)
{
    // check git exists
    int git_available = system("git --version > /dev/null 2>&1");
    if (git_available != 0) {
        printf("ERROR: git is not available.\n");
        exit(1);
    }
    // check this is a git repo
    int git_repo = system(
        "git rev-parse --is-inside-work-tree > /dev/null 2>&1"
    );
    if (git_repo != 0) {
        printf("ERROR: this is not a git repository.\n");
        exit(1);
    }
    // parse flags
    int cflag = 0;
    int hflag = 0;
    int fflag = 0;
    int sflag = 0;
    int pflag = 0;
    int nflag = 0;
    int oflag = 0;
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
        } else if (
            strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--open") == 0
        ) {
            // check VSCode exists
            int vscode_available = system("code --version > /dev/null 2>&1");
            if (vscode_available != 0) {
                printf("ERROR: VSCode is not available.\n");
                exit(1);
            }
            oflag = 1;
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
    // Prepare query
    char sensitive[4] = "";
    if (!sflag) {
        strcpy(sensitive, "-i "); 
    }
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
    // git grep -l
    char grep_l[1024];
    snprintf(
        grep_l,
        sizeof(grep_l),
        "git grep -l %s%s -- '*.py'",
        sensitive,
        query
    );
    FILE *fp = popen(grep_l, "r");
    if (fp == NULL) {
        perror("popen failed");
        return 1;
    }
    char buffer[1024];
    int MAX_LENGTH = 0;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strlen(buffer) > MAX_LENGTH) {
            MAX_LENGTH = strlen(buffer);
        }
    }
    if (pclose(fp) == -1) {
        perror("pclose failed");
        return 1;
    }
    int LINE_LENGTH = MAX_LENGTH + 4;
    if (LINE_LENGTH < 79) {
        LINE_LENGTH = 79;
    }
    char *separator = (char *)malloc((LINE_LENGTH + 2) * sizeof(char));
    memset(separator, '-', LINE_LENGTH);
    separator[LINE_LENGTH] = '\n';
    separator[LINE_LENGTH + 1] = '\0';
    // git grep -n
    char grep_n[1024];
    snprintf(
        grep_n,
        sizeof(grep_n),
        "git grep -n %s%s -- '*.py'",
        sensitive,
        query
    );
    fp = popen(grep_n, "r");
    if (fp == NULL) {
        perror("popen failed");
        return 1;
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        print_element(pflag, nflag, oflag, buffer, separator);
    }
    if (pclose(fp) == -1) {
        perror("pclose failed");
        return 1;
    }
    // clean up memory
    free(separator);
    return 0;
}