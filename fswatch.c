#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <CoreServices/CoreServices.h> 

/* fswatch.c
 * 
 * usage: ./fswatch /some/directory[:/some/otherdirectory:...] "some command" 
 * "some command" is eval'd by bash when /some/directory generates any file events
 *
 * compile me with something like: gcc fswatch.c -framework CoreServices -o fswatch
 *
 * adapted from the FSEvents api PDF
*/

#define MAX_ARGS 16

extern char **environ;
//the command to run
char *to_run;

//fork a process when there's any change in watch file
void callback( 
    ConstFSEventStreamRef streamRef, 
    void *clientCallBackInfo, 
    size_t numEvents, 
    void *eventPaths, 
    const FSEventStreamEventFlags eventFlags[], 
    const FSEventStreamEventId eventIds[]) 
{ 
  pid_t pid;
  int   status;

  /*printf("Callback called\n"); */
  if(2*numEvents+2 > MAX_ARGS) {
    fprintf(stderr, "error: too many events\n");
    exit(1);
  }

  if((pid = fork()) < 0) {
    fprintf(stderr, "error: couldn't fork \n");
    exit(1);
  } else if (pid == 0) {
    char *args[MAX_ARGS] = {
      to_run,
    };
    char flags[MAX_ARGS][11];
    int i = 0;
    for(i=0;i<numEvents; i++) {
      sprintf(flags[i], "0x%08x", (unsigned int)eventFlags[i]);
      args[i*2+1] = ((char**)eventPaths)[i];
      args[i*2+2] = flags[i];
    }
    args[i*2+1] = 0;
    if(execve(args[0], args, environ) < 0) {
      fprintf(stderr, "error: error executing\n");
      exit(1);
    }
  } else {
    while(wait(&status) != pid)
      ;
  }
} 
 
//set up fsevents and callback
int main(int argc, char **argv) {

  if(argc != 3) {
    fprintf(stderr, "You must specify a directory to watch and a command to execute on change\n");
    exit(1);
  }

  to_run = argv[2];

  CFStringRef mypath = CFStringCreateWithCString(NULL, argv[1], kCFStringEncodingUTF8); 
  CFArrayRef pathsToWatch = CFStringCreateArrayBySeparatingStrings (NULL, mypath, CFSTR(":"));

  void *callbackInfo = NULL; 
  FSEventStreamRef stream; 
  CFAbsoluteTime latency = 1.0;

  stream = FSEventStreamCreate(NULL,
    &callback,
    callbackInfo,
    pathsToWatch,
    kFSEventStreamEventIdSinceNow,
    latency,
    kFSEventStreamCreateFlagNone
  ); 

  FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode); 
  FSEventStreamStart(stream);
  CFRunLoopRun();

}
