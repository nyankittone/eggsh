# Eggsh design goals
* Eggsh should be as POSIX-compliant as possible. Ideally, I should strive for 100% perfect
  compliance.
* Eggsh should have colorful info, warning, and error messages, to help make the messages more
  readable.
* Eggsh's errors should be verbose, yet helpful and approachable. Eggsh should do a reasonably good
  job at telling the user every way that a commnand was incorrectly called, and should maybe tell
  the user *where* the error in their command exactly is.
* Eggsh should be well-documented. There should be good built-in documentation for all of the
  built-in commands. Every builtin should support `--version` and `--help`, as long as POSIX
  doesn't strictly forbid doing so. There should be a `help` builtin that shows a summary of help
  for builtins and also external commands. the `help` builtin should also have the option to provide
  more in-depth information about a builtin ro some functionality, explaining what the thing does,
  where it comes from, its rationale, how to use it, the parameters and options it takes, and
  examples of all of this. I may want to have both traditional manpages for this and also something
  more in-depth and interactive.
* Eggsh should be user-friendly wherever it's possible. This is kind of an extension of wanting to
  have good error messages, but also includes things such as built-in syntax highlighting and
  autocomplete, built-in and configurable command suggestions when the user types something in
  wrong, and whatever else I can think of.
* Eggsh should *feel* fast to use interactively. There should be no perceivable delay when starting
  the shell or running commands. Performance of running shell scripts is also important, but not as
  high-priority as making things feel smooth interactively.
* Eggsh should have *some* kind of module and/or configuration system that is fast to load from
  (i.e. not executing shell commands).

