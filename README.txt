Stereo Odometry for Tara Stereo Camera

This package is built on the libviso2 visual odometry package that can be found at http://www.cvlibs.net/software/libviso/. Install package in your home folder for out of the box compatibility with this implementation. Else, change the source directoriy in the CMake file.

To build project

cmake .
make

Contact Lawrence Papincak at larry12193@gmail.com to get access to test data sets to run

To use

./stereo_viso /path/to/rectified/images >> /path/to/pose.txt

The rectified images are named specifically, look in stereo_viso.cpp @line 72-74 to get idea of how this is done. The file format that is compiled to understand is 'L_######.png' and 'R_######.png' for left and right frames, respectively. Change as you see fit for different naming scheme.
