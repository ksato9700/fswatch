## fswatch.c

This is a small program using the Mac OS X FSEvents API to monitor a directory.
When an event about any change to that directory is received, the specified
command is executed.

If you're on GNU/Linux,
[inotifywatch](http://linux.die.net/man/1/inotifywatch) (part of the
`inotify-tools` package on most distributions) provides similar
functionality.

### Compile

You need to be on Mac OS X 10.5 or higher with Developer Tools
installed.  Then, run `make`.  Copy the resulting `fswatch` binary to
a directory on your `$PATH`.

### Basic Usage

    ./fswatch /some/dir watch.py

This would monitor `/some/dir` for any change, and execute the python script
`watch.py` (or any other executable you specified) when a modification event
is received.

In the case you want to watch multiple directories, just separate them
with colons like:

    ./fswatch /some/dir:/some/otherdir watch.py

### Callback Program Argument

The callback program you specified will get list of pairs of directory path
and flags (FSEventStreamEventFlags). See `watch.py` how you can use the passed
arguments in the callback program.

### Usage with rsync

`fswatch` can be used with `rsync` to keep a remote directory in sync
with a local directory continuously as local files change.  The
following example was contributed by
[Michael A. Smith](http://twitter.com/michaelasmith):

```bash
#!/bin/sh

##
# Keep local path in sync with remote path on server.
# Ignore .git metadata.
#
local=$1
remote=$2

cd "$local" &&
fswatch . "date +%H:%M:%S && rsync -iru --exclude .git --exclude-from=.gitignore --delete . $remote"
```

### About

This code was adapted from the example program in the
[FSEvents API documentation](https://developer.apple.com/library/mac/documentation/Darwin/Conceptual/FSEvents_ProgGuide/FSEvents_ProgGuide.pdf).
