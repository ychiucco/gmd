gmd() {
  (  
    usage() {
      printf "\nUsage: gmd [OPTIONS] [--] 'arg'\n\n"
      printf "Options:\n"
      printf "  -h, --help          Show this help message\n"
      printf "  -p, --path          Display the file path and line number of the match\n"
      printf "  -n, --name          Display only the name of the matched object (class/function)\n"
      printf "  -c, --class         Search for class definitions only (cannot be used with -f)\n"
      printf "  -f, --function      Search for function definitions only (cannot be used with -c)\n"
      printf "  -s, --sensitive     Make the search case-sensitive\n\n"
      printf "Positional Arguments:\n"
      printf "  arg                 The class or function name to search for\n"
      printf "                      Default search is case-insensitive\n"
      printf "                      Use '.*' as a wildcard to match patterns\n"
      printf "                      Wrap the argument in single quotes (' ') when using wildcards\n\n"
      printf "Examples:\n"
      printf "  gmd -p -n MyClass    Search for 'MyClass' and display the file path and the name of the object\n"
      printf "  gmd -c -s MyClass    Search for class 'MyClass' definitions only\n"
      printf "  gmd 'user.*'         Search for any class or function whose name begins with 'user'\n\n"
      printf "  gmd -f '.*group.*'   Search for any function that contains the word 'group' in its name\n\n"
    }

    error() {
      echo "ERROR: $1\n"
      usage
    }

    P=
    N=
    C=
    F=
    S=
    while :; do
      case $1 in
        -h|--help) usage; return;;
        -p|--path) P=1;;
        -n|--name) N=1;;
        -c|--class) C=1;;
        -f|--function) F=1;;
        -s|--sensitive) S=1;;
        --) shift; break;;
        -?*) error "Bad option '$1'."; return 1;;
        *) break;;
      esac
      shift
    done
    
    if [ ! -z "$C" ] && [ ! -z "$F" ]; then
      error "Cannot use both -c and -f."
      return 1
    fi

    if [ -z "$1" ]; then
      error "Missing argument."
      return 1
    fi

    # Query
    
    if [ ! -z $C ]; then
      QUERY=("class ${1}(")
    elif [ ! -z $F ]; then
      QUERY=("def ${1}(")
    else
      QUERY=(-e "def ${1}(" --or -e "class ${1}(")
    fi

    if [ ! -z $S ]; then
      CASE=
    else
      CASE=(-i)
    fi
    
    # Print

    git grep ${CASE} -n ${QUERY} -- '*.py' | while read -r FILE; do

      FILE_PATH=$(echo "$FILE" | cut -d':' -f1)
      INITIAL_LINE_NUMBER=$(echo "$FILE" | cut -d':' -f2)
      
      if [ ! -z $P ] && [ ! -z $N ]; then
        OBJECT=$(
          sed -n "${INITIAL_LINE_NUMBER}p" "$FILE_PATH" | sed -E 's/.* ([^()]+)\(.*/\1/'
        )
        MAX_FILE_PATH_LENGTH=$(
          git ls-files '*.py' | awk '
          {
              if (length($0) > max) {
                  max = length($0);
                  longest = $0;
              }
          }
          END {
              print max;
          }'
        )
        printf "%-${MAX_FILE_PATH_LENGTH}s %s\n" "${FILE_PATH}:${INITIAL_LINE_NUMBER}" "${OBJECT}"
        continue
      elif [ ! -z $P ]; then
        echo ${FILE_PATH}:${INITIAL_LINE_NUMBER}
        continue
      elif [ ! -z $N ]; then
        OBJECT=$(
          sed -n "${INITIAL_LINE_NUMBER}p" "$FILE_PATH" | sed -E 's/.* ([^()]+)\(.*/\1/'
        )
        echo $OBJECT
        continue
      fi

      TOTAL_LINES=$(wc -l < "$FILE_PATH")    
      INDENTATION_LEVEL=$(
        sed -n -e ${INITIAL_LINE_NUMBER}p $FILE_PATH | grep -o '^[ \t]*' | wc -c
      )
      INDENTATION=$(printf '%*s' "${INDENTATION_LEVEL}" '')

      exec 3<"$FILE_PATH"
      for ((i = 1; i <= INITIAL_LINE_NUMBER; i++)); do
          read -r _ <&3
      done

      LINE_NUMBER=$((INITIAL_LINE_NUMBER + 1))
      while [ "${LINE_NUMBER}" -le "${TOTAL_LINES}" ]; do
          
          IFS= read -r CURRENT_CONTENT <&3 || break

          if [[ -z "${CURRENT_CONTENT// }" ]]; then
              LINE_NUMBER=$((LINE_NUMBER + 1))
              continue
          fi

          if [[ "$CURRENT_CONTENT" =~ ^"${INDENTATION} " ]] || \
            [[ "$CURRENT_CONTENT" =~ ^"${INDENTATION}):" ]]; then
              LINE_NUMBER=$((LINE_NUMBER + 1))
          else
              break
          fi
      done

      exec 3<&- 

      echo "\n\e[30;43m${FILE_PATH}:${INITIAL_LINE_NUMBER}\e[0m\n"
      sed -n "${INITIAL_LINE_NUMBER},$((LINE_NUMBER - 1))p" "$FILE_PATH"

    done
  )
}