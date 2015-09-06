AVSyn
=============

A Cinder project to explore live music visualization. Intended to be projected behind bands or DJs at concerts.


### Usage

- Make sure you have a mic connected or a software output monitor (and it's selected in Recording Devices)
- Build with Visual Studio 2015
- Optionally connect a Kinect v2 for the Kinect visualization. NOTE: If you don't have a Kinect use the `no-kinect` branch!
- If you have a second monitor available, the visualizations will appear there and the controls on the main monitor.
- Browse through the different visualizations and play with the parameters!

### Current Visualizations
- Smoke: (using Navier-Stokes equations)
- Flocking: (based on Robert Hodgin's [Cinder tutorial](http://libcinder.org/docs/v0.8.3/flocking_chapter2.html))
- Dots: based on average volume of different parts of the frequency spectrum
- EQPointCloud: similar to dots, but in a random cloud of points
- Circular: circles mapped to the frequency spectrum


### WebGL

There's also a [WebGL version](https://github.com/ulyssesp/webgl-shader-jockey-ts) that you can [view online](https://ulyssesp.github.io/webgl-shader-jockey-ts).
