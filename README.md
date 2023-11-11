# CWebServer

goal: build a simple multi-threaded static http web server. look into servers and thier architecture and problems (just a little) before you start doing this, and plan ahead a little.

Since this is going to be in C/C++, first thing I need to figure out is how to build this thing (since building C/C++ is a pain in the ass), I can go raw, but I should consider CMake or makefile, I'll look into it.

I also need to figure out the specific details of the HTTP protocol, I need to figure out what NEEDS TO BE DONE. I also need to decide where I want to start, and how low-level I want this project to be, should I go all the way and implement at lowest level possible with only basic OS libraries and functionalities?

Then after having a solid idea of what I need to do, I need to start defining subsystems and architecture, and splitting the program in parts, and defining responsibilites and concerns...

then we start coding... god why did choose C this language is a pain in the ass

features brain dump key words stuff:

- multi-threaded
- caching?
- external config files containing:
- max-threads (thread pool queue)
- max-heap-size (memory allocator + synchronization)
- Mostly C with a hint of C++ or any other technologies when needed.
- static web resoureces
- http protocol
- blacklist of ips
- cache size! (are large files cached?)
- chunked transfer encoding? looks like a headache. (for Transfer-Encoding: Chunked header)

========
For now I'm gonna do some prototyping for some basic functionalities to get the ball rolling.

make a file browser-esque thing startign at index.html! ... index.html gives you a list of all available files and directories, and if the URL points to a directory, then the a wepbage with a list of files is returned to the user.... kinda like vscode live server and apache.

Now what to do with pathnames? just pass whatever is in the request to fopen? some checks? or full conversion and manual lookup for the file? what about access control?

extend this project to a simple minimalistic web library? or maybe architect it as a web library! and then use those parts to build the file browser thing and manually get stuff! two projects in one! but this means you have to make some considerations to the web frameworky parts.
