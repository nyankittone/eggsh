# eggsh
This is a shell for Linux and other Unix and Unix-like systems, like macOS and the BSDs. I'm writing
it as a way to more deeply learn how a shell on Unix actually works.

Right now, the program has a basic REPL, and an extremely basic tokenizer (just splits tokens on
whitespece lol). It has basic support for a few internal commands like `cd`, `echo`, and `exit`,
and it can run programs inside directories listed in `$PATH`. That's about it.

## Planned additions
No garantuee that I'll continue to be motivated to add stuff to this in the future, but if I do,
I'll add the following down the road:
* Proper implementations for the `pwd` and `printf` internal commands
* Script file support
* More advanced parsing for the parsing code (singe and double quotes support, and backslashes)
* Pipes and control flow statements
* Command aliases
* Command history

## How to build/install
If you have Nix installed, you can simply run:

```sh
nix-env -if default.nix
```

from inside the project directory. That will install `eggsh` onto your system. This can be undone
with:

```sh
nix-env -e eggsh
```

If you don't have Nix, you'll want to:
* Install `gcc`, `make`, and `gperf`.
* If you want to run the test suite, install `check` as well.
* Run `make` from the project directory.
* To build the test suite, run `make tests`.

The shell binary will be named `eggsh`, and the binary for running the tests is `tests`.

