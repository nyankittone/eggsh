# TODO list and Eggsh roadmap

## 0.0.3
The major goal of this release is to get eggsh into the minimal viable state needed for making it my
default shell, so I can dog-food it.
- [X] Add multi-line command support
- [O] Add parsing for single and double quotes, backslashes, and semicolons
  - [X] single quotes
  - [X] double quotes
  - [ ] backslashes
  - [ ] semicolons
- [ ] Add proper handling of signals such as SIGINT so that, for example, the shell doesn't close when
  pressing Ctrl-C
- [ ] Fix a couple random bugs that I feel like fixing
- [ ] Fix possible segfault bug in the hash map on small bucket arrays
- [ ] Consider implementing OSC escape codes into my shelll (I recently discovered what those do lol)

## 0.0.4
This release should focus on making the argument parser used in the main function and internal
commands more robust and complete, and implement a few internal commands using it.
- [ ] Add the `CompoundError` data structure, used mainly for creating and displaying errors in eggsh
  regarding argument parsing
  - [ ] We will also need to make the argument parser actually use this data structure.
- [ ] Clean up the code needed in a command's "main" function to make it more pleasing to write
- [ ] Implement the `printf` command
- [ ] Implement a command for generating random numbers

## Later versions
- [ ] Create more ways to unit test builtin commands
  - [ ] For programs like `pwd`, I'll need to create a mock filesystem that contains directories I can 
    cd into and test with
    - [ ] This would include symbolic links so I can ensure `pwd -P` vs `pwd -L` work as intended
- [ ] Make the `cd` implementation properly POSIX-conforming
- [ ] Fix the performance problems of the hash map implementation (and other performance problems)
- [ ] add the `type` command (will require refactor to do it right)
- [ ] Make the cached file names in PATH more dynamic
  - [ ] What should be done when the PATH or the state of PATH changes?
    - [ ] new program added to dir in PATH -> program will fail to find program in table, so go through
      everything in PATH to see if you can find said program.
    - [ ] program removed from dir in PATH -> hashing succeeds, but exec fails. React by removing the
      failed key.
    - [ ] Directory in PATH removed -> hashing succeeds, but exec fails. I will need to have some way
      of knowing if that  exec failure happens due to a directory being wrong or the end file being
      wrong. Regardless, react by removing all entries from the map with the bad directory, and
      remove the string for the directory in memory. Do not alter PATH to have the directory
      removed.
    - [ ] PATH variable changes -> Have special logic in variable assignment and the `export` builtin
      for changing the store of strings for each directory in PATH. Also will need to go through the
      hash map and cache new directories or remove entries resolving to old ones. This will be a lot
      of work.
- [ ] Add pipes
- [ ] Add history at some point (may require re-thinking how everything is architected...)
- [ ] Add control flow statements (how the fuck...)
