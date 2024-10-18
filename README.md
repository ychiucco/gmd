# gmd

`gmd` (**Give Me Definition**) is a shell utility that allows you to quickly find and retrieve details about class and function definitions in a Python project tracked by `git`.

### Installation

```sh
git clone git@github.com:ychiucco/gmd.git
cd gmd
gcc -o gmd.o gmd.c
echo "\nalias gmd=$(pwd)/gmd.o\n" >> ~/.${0#-}rc
source ~/.${0#-}rc
```

### Usage

```sh
gmd [-c|-f] [-s] [-p] [-n] [-o] 'arg'
```

Options:
- Query:
    - `-c`, `--class`: Search for class definitions only (cannot be used with `-f`)
    - `-f`, `--function`: Search for function definitions only (cannot be used with `-c`)
    - `-s`, `--sensitive`: Make the search case-sensitive

- Printing:
    - `-p`, `--path`: Display the file path and line number of the match
    - `-n`, `--name`: Display the name of the matched object
    - `-o`, `--open`: Open matching file at the correct line with VSCode (ignore `-p` and/or `-n`)

Positional Arguments:
- `arg`: The class or function name to search for. Use `.*` in any position as a wildcard to match patterns. Wrap the argument in single quotes (`' '`) when using wildcards.