# gmd

`gmd` (**Give Me Definition**) is a shell utility that allows you to quickly find and retrieve details about class and function definitions in a Python project tracked by `git`.

Right now the code is tested on `zsh` only.

### Installation

```zsh
echo "\n" >> ~/.zshrc && curl -s https://raw.githubusercontent.com/ychiucco/gmd/refs/heads/main/gmd.zsh >> ~/.zshrc
```

### Usage

```zsh
gmd [OPTIONS] [--] 'arg'
```

Options:
- `-h`, `--help`: Show help message
- `-p`, `--path`: Display the file and line number of the match
- `-n`, `--name`: Display only the name of the matched object (class/function)
- `-c`, `--class`: Search for class definitions only (cannot be used with `-m`)
- `-f`, `--function`: Search for function definitions only (cannot be used with `-c`)
- `-s`, `--sensitive`: Make the search case-sensitive

Positional Arguments:
- `arg`: The class or function name to search for

<br>
Default search is case-insensitive.

Use `.*` in any position as a wildcard to match patterns. Wrap the argument in single quotes (`' '`) when using wildcards.

---

### Examples

```zsh
$ gmd -cn '.*user.*'

UserBench
LinkUserGroup
LinkUserProject
LinkUserProjectV2
UserOAuth
UserGroup
UserSettings
ProjectUser
UserRead
UserUpdate
UserUpdateStrict
UserUpdateWithNewGroupIds
UserCreate
UserGroupRead
UserGroupCreate
UserGroupUpdate
UserSettingsRead
UserSettingsReadStrict
UserSettingsUpdate
UserSettingsUpdateStrict
SQLModelUserDatabaseAsync
UserManager
SlurmSshUserSettings
SlurmSudoUserSettings
```

```zsh
$ gmd -s userbench

$ gmd userbench

benchmarks/api_bench.py:20

class UserBench(BaseModel):
    name: str
    password: str
    token: Optional[str]
```

---
Next features:

- `-o` flag to open files with VSCode.

- Highlight `class`, `def` and `async def` when printing.

- Allow multiple args.

- Write prerequisites.

- Prevent edge cases: e.g. `print("def class foo")`