---
title: dte
section: 1
date: April 2020
description: A small, configurable text editor
author: [Craig Barnes, Timo Hirvonen]
seealso: ["`dterc`", "`dte-syntax`"]
---

# Synopsis

**dte**
\[**-HR**]
\[**-c** _command_]
\[**-t** _ctag_]
\[**-r** _rcfile_]
\[\[**+**line] _file_]...  
**dte** \[**-h**|**-B**|**-K**|**-V**|**-b** _rcname_|**-s** _file_]

# Options

`-c` _command_
:   Run _command_, after reading the rc file and opening any _file_
    arguments. See [`dterc`] for available commands.

`-t` _ctag_
:   Jump to source location of _ctag_. Requires `tags` file generated
    by [`ctags`].

`-r` _rcfile_
:   Read configuration from _rcfile_ instead of `~/.dte/rc`.

`-s` _file_
:   Load _file_ as a [`dte-syntax`] file and exit. Any errors
    encountered are printed to `stderr` and the exit status is
    set appropriately.

`-b` _rcname_
:   Dump the contents of the built-in rc or syntax file named _rcname_
    and exit.

`-B`
:   Print a list of all built-in config names that can be used with the
    `-b` option and exit.

`-H`
:   Don't load history files at startup or save history files on exit (see
    `FILES` section below). History features will work as usual but will
    be in-memory only and not persisted to the filesystem.

`-R`
:   Don't read the rc file.

`-K`
:   Start in a special mode that continuously reads input and prints the
    symbolic name of each pressed key.

`-h`
:   Display the help summary and exit.

`-V`
:   Display the version number and exit.

# Key Bindings

There are 3 editor modes, each having a different set of key bindings.
Normal mode bindings can be customized by using the [`bind`] command
(see [`dterc`]) or displayed using the [`show bind`] command.

The key bindings listed below are in the same format as accepted by
the [`bind`] command. In particular, key combinations are represented
as follows:

* `M-x` is Alt+x
* `C-V` (or `^V`) is Ctrl+v
* `S-left` is Shift+left
* `C-M-S-left` is Ctrl+Alt+Shift+left

## Normal Mode

Normal mode is the mode the editor starts in. Pressing basic keys
(i.e. without modifiers) simply inserts text into the buffer. There
are also various key combinations bound by default:

`S-up`, `S-down`, `S-left`, `S-right`
:   Move cursor and select characters

`C-S-left`, `C-S-right`
:   Move cursor and select whole words

`C-S-up`, `C-S-down`
:   Move cursor and select whole lines

`^C`
:   Copy current line or selection

`^X`
:   Cut current line or selection

`^V`
:   Paste

`^Z`
:   Undo

`^Y`
:   Redo

`M-x`
:   Enter command mode

`^F`
:   Enter search mode

`F3`
:   Search next

`F4`
:   Search previous

`^T`
:   Open new buffer

`M-1`, `M-2` ... `M-9`
:   Switch to buffer 1 (or 2, 3, 4, etc.)

`^W`
:   Close current buffer

`^S`
:   Save

`^Q`
:   Quit

## Command Mode

Command mode allows running various editor commands using a language
similar to Unix shell. The [`next`] and [`prev`] commands switch
to the next/previous file. The [`open`], [`save`] and [`quit`]
commands should be self-explanatory. For a full list of available
commands, see [`dterc`].

The key bindings for command mode are:

`up`, `down`
:   Browse previous command history.

`tab`
:   Auto-complete current command or argument

`^A`
:   Go to beginning of command line

`^B`
:   Move left

`^C`
:   Exit command mode

`^D`
:   Delete

`^E`
:   Go to end of command line

`^F`
:   Move right

`^K`
:   Delete to end of command line

`^U`
:   Delete to beginning of command line

`^W`
:   Erase word

`Esc`
:   Exit command mode

## Search Mode

Search mode allows entering a regular expression to search in the
current buffer.

The key bindings for search mode are mostly the same as in command mode,
plus these additional keys:

`M-c`
:   Toggle case sensitive search option.

`M-r`
:   Reverse search direction.

`Enter`
:   Perform regex search.

`M-Enter`
:   Perform plain-text search (escapes the regex).

# Environment

The following environment variables are inspected at startup:

`DTE_HOME`
:   User configuration directory. Defaults to `$HOME/.dte` if not set.

`DTE_FORCE_TERMINFO`
:   Force dte to use `terminfo` database, even for terminals
    with built-in support. Enabled if set.

`HOME`
:   User home directory. Used when expanding `~/` in filenames and also
    to determine the default value for `DTE_HOME`.

`TERM`
:   Terminal identifier. Used to determine which `terminfo` entry or
    built-in terminal support to use.

`COLORTERM`
:   Enables 24-bit color output, if set to `truecolor`.

# Files

`$DTE_HOME/rc`
:   User configuration file. See [`dterc`] for a full list of
    available commands and options or run "dte -b rc" to see the built-in,
    default config.

`$DTE_HOME/syntax/*`
:   User syntax files. These override the syntax files that come with
    the program. See [`dte-syntax`] for more information or run
    "dte -b syntax/dte" for a basic example.

`$DTE_HOME/file-locks`
:   Records open files, to prevent accidentally editing files opened in
    another dte process. Used only if the [`lock-files`] option is enabled.

`$DTE_HOME/file-history`
:   History of edited files and cursor positions. Used only if the
    [`file-history`] option is enabled.

`$DTE_HOME/command-history`
:   History of `dterc` commands used while in command mode.

`$DTE_HOME/search-history`
:   History of search patterns used while in search mode.


[`dterc`]: dterc.html
[`dte-syntax`]: dte-syntax.html
[`bind`]: dterc.html#bind
[`show bind`]: dterc.html#show
[`next`]: dterc.html#next
[`prev`]: dterc.html#prev
[`open`]: dterc.html#open
[`save`]: dterc.html#save
[`quit`]: dterc.html#quit
[`lock-files`]: dterc.html#lock-files
[`file-history`]: dterc.html#file-history
[`ctags`]: https://en.wikipedia.org/wiki/Ctags
