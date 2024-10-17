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

```zsh
gmd [OPTIONS] [--] 'arg'
```

Options:
- `-h`, `--help`: Show help message
- `-p`, `--path`: Display the file and line number of the match
- `-n`, `--name`: Display only the name of the matched object (class/function)
- `-c`, `--class`: Search for class definitions only (cannot be used with `-f`)
- `-f`, `--function`: Search for function definitions only (cannot be used with `-c`)
- `-s`, `--sensitive`: Make the search case-sensitive

Positional Arguments:
- `arg`: The class or function name to search for

<br>
Default search is case-insensitive.

Use `.*` in any position as a wildcard to match patterns. Wrap the argument in single quotes (`' '`) when using wildcards.