# TODO list
* Fix bug in cd implementation
  * We need to hold on to a string containing the current working directory at all points.
  * We can do this by keeping a copy of it in the $PWD environment variable.
  * Upon program launch, look inside the PWD variable to see if it's set. If not, set it from
    getcwd(). Set a pointer pointing to the PWD variable afterwards for quick access.
  * Whenever `cd` is run, we'll update PWD environment variable directly each time, and then reset
    the pointer going to it.
* Create ways to unit test builtin commands
* Implement the pwd and printf commands
* Add support for reading commands from a file
* Fix the performance problems of the hash map implementation
* Add parsing for single and double quotes, backslashes, and semicolons
* add the `type` command (will require refactor to do it right)
* Make the cached file names in PATH more dynamic
  * What should be done when the PATH or the state of PATH changes?
    * new program added to dir in PATH -> program will fail to find program in table, so go through
      everything in PATH to see if you can find said program.
    * program removed from dir in PATH -> hashing succeeds, but exec fails. React by removing the
      failed key.
    * Directory in PATH removed -> hashing succeeds, but exec fails. I will need to have some way
      of knowing if that  exec failure happens due to a directory being wrong or the end file being
      wrong. Regardless, react by removing all entries from the map with the bad directory, and
      remove the string for the directory in memory. Do not alter PATH to have the directory
      removed.
    * PATH variable changes -> Have special logic in variable assignment and the `export` builtin
      for changing the store of strings for each directory in PATH. Also will need to go through the
      hash map and cache new directories or remove entries resolving to old ones. This will be a lot
      of work.
* Add pipes
* Add history at some point (may require re-thinking how everything is architected...)
* Add control flow statements (how the fuck...)
