# ksh

A simple Unix shell written in C. Supporting command execution, builtin commands, redirection and pipe support .

## Features

- **Command execution** — runs external programs by resolving them via `PATH` and executing them as child processes
- **Built-in commands**:
  - `cd` — change the current working directory
  - `exit` — exit the shell
- **Redirection** - can redirect input and output of a program. (`ls -l > list.txt`)
- **Pipes** - supports pipes for connecting each command's stdout to the next command's stdin. (`cmd1 | cmd2 | cmd3`)

## Building

Clone the repository and run 'make' from the project root:

```bash
git clone https://github.com/kartikpanesar/ksh.git
cd ksh
make
```

## Usage

```bash
./ksh
```

Once running, ksh presents a prompt where you can type commands just like in `bash` or `sh`:

```
ksh > ls -la
ksh > cd Documents
ksh > exit
ksh > ls -l > list.txt
ksh > wc < essay.txt
ksh > ls -l | wc -l
```

## How It Works

At a high level, ksh:

1. Reads a line of input from the user
2. Parses it into a command and its arguments
3. Checks if the command is a built-in (`cd`, `exit`) and handles it directly
4. Otherwise, forks a child process and uses `exec` to run the command
5. Waits for the child process to finish before showing the next prompt

## Future Improvements

- [x] Pipe support (`cmd1 | cmd2`)
- [x] Output redirection (`>`, `>>`)
- [x] Input redirection (`<`)
- [ ] Additional built-ins (`pwd`, `history`, etc.)

