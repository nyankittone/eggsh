# TODO list
* Fix bug in cd implementation
* Create ways to unit test builtin commands
* Implement the pwd and printf commands
* Add support for reading commands from a file
* Add parsing for single and double quotes, backslashes, and semicolons
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
