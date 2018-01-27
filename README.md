# DMX_Lightshow

I haven't made a CMake file yet, so to build run:

`g++ main.cc lib/libportaudio.a -lrt -lm -lasound -pthread -o main --std=c++11`

You'll need that libportaudio library, so follow [the instructions here](http://portaudio.com/docs/v19-doxydocs/compile_linux.html).

There are some boost includes, so make sure those exist
