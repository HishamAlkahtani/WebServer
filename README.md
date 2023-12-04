# JWebServer

goal: build a simple multi-threaded static http web server. look into servers and thier architecture and problems (just a little) before you start doing this, and plan ahead a little.

Since this is going to be in C/C++, first thing I need to figure out is how to build this thing, I can go raw, but I should consider CMake or makefile, I'll look into it.

I also need to figure out the specific details of http, I need to figure out what NEEDS TO BE DONE.

Then after having a solid idea of what I need to do, I need to start defining subsystems and architecture, and splitting the program in parts, and defining responsibilites and concerns...

then we start coding... god why did choose C this language is a pain in the ass

features brain dump key words stuff:
- multi-threaded
- external config files
- limitless request size ( until pc crashes )
- Mostly C with a hint of C++ or any other technologies when needed.
- static web resoureces
- http protocol
