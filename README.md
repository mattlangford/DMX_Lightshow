# DMX_Lightshow

To get building:

You'll need libportaudio library, so follow [the instructions here](http://portaudio.com/docs/v19-doxydocs/compile_linux.html).

There are some boost includes, so make sure those exist

Then `bazel build //...:all --cxxopt='-std=c++1z'`
