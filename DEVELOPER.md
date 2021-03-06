# Developer Information
This file contains what is essentially notes for anyone wanting to develop for this (including myself,) or any other thcrap plugin. It contains mostly information on how I found most files needed for building, because that took way longer than it should have the first time. There is also information on how some subsystems work. If there's something here you don't understand, just add an issue and I'll try to document it here. 

None of this is under warranty of any kind, it's just little notes that may or may not be helpful.

# TODOS:
1) Tie into thcrap better; ~~cleanup~~ (Done(?)) and configuration hotpatches would be good
2) Come up with a list of functions and values to provide (and document them here!)
3) Find the needed memory addresses for those values and functions.
4) Make cmake able to build on any Linux and Windows (providing several toolchain files would do this)
5) (Maybe) Add to thcrap launcher, if plugins with code are allowed
6) Provide graphical editor for values used by program to demonstrate functionality.

# Known bugs:
- The threads are spaghetti, and this means that any change can and will break them. They need a rewrite by someone who knows C properly.
- Developer spends more time wishing he finished this already and fantasizing about being done than actually working.

# Included source file's sources

## Header files
Header files come from a variety of repositories. Basically, just copy whatever is missing into `/include` until everything's defined.
The most notable sources are:
- thcrap:
   - thcrap/thcrap/src/*.h
   - thcrap/thcrap_tsa/src/*.h
- jansson:
  - Wherever jansson.h comes from (I got it from a system dir)
- Everything else I forgot (I'll replace this when it's generated (eg. once this is useless documentation because builds will generate this section and the code for that will document it better))

## .lib files
The .lib files in `/lib` are all generated from .def files. The command used to do this is 
```bash
dlltool -d jansson.def -l lib/jansson.lib
```
The def files for thcrap come directly from the project itself, while the def file for jansson comes from 
```bash
gcc -aux-info /dev/stdout include/jansson.h | grep jansson.h | cpp | awk 'BEGIN {print "LIBRARY jansson.dll\nEXPORTS"};/^[^#;]/{gsub(/\(.*|\*/,"");print $(NF)}' > jansson.def
```

# How it works
thcrap will load any dll in its `bin` directory upon launching a game. It then attempts to call a function `thcrap_plugin_init`, which should return 0 to continue being loaded, or anything else to be unloaded. The dll is also unloaded if the function is not found in it. In the `thcrap_plugin_init` function, `stack_remove_if_unneeded` can be called to check if the game being loaded is supported via means of its configuration files. This is what allows not loading for an unsupported game. After this, a variety of other functions are called.
These include
- \*\_mod\_\*: A variety of methods that provide basic functionality, including a later init routine and cleanup (See thcrap/thcrap/src/plugin.h)
- BP_\*: Called whenever the breakpoint `*` is triggered. Also passes the json for configuring said breakpoint, allowing breakpoints to have settings.

Note that these will **ONLY** work when the functions are correctly exported, which is pretty easy to forget to do. Make sure not to do that.

We simply spawn a new thread to run a server, and use it to handle connections. This is done with winsock, a handy and not POSIX compliant in the least library from Microsoft. See [/src/server.c](src/server.c) and [/src/server_threads.c](src/server_threads.c) for some information about that.