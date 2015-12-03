AVSyn
=============

A Cinder project to explore live music visualization. Intended to be projected behind bands or DJs at concerts.


### Usage

- Make sure you have a mic connected or a software output monitor (and it's selected in Recording Devices)
- Build with Visual Studio 2015
- If you have a second monitor available, the visualizations will appear there and the controls on the main monitor.
- Browse through the different visualizations and play with the parameters!

### Current Visualizations
- Buffer: a visual representation of the time buffer
- Circular: circles mapped to the frequency spectrum
- Dots: based on average volume of different parts of the frequency spectrum
- EQPointCloud: similar to dots, but in a random cloud of point
- Flocking: (based on Robert Hodgin's [Cinder tutorial](http://libcinder.org/docs/v0.8.3/flocking_chapter2.html))
- Fluid: a smoke simulation (using Navier-Stokes equations)
- Neurons: simple neuron simulation with a base layer of neurons mapped to the frequency spectrum
- Mix: mix two other visualizations on top of one another
- Rotate: same as Buffer except rotating
- Simple: rainbow colored spectrum in a circle

### Effects
- Fade: renders the previous frame at a certain opacity along with the current frame
- Scale: scales the previous frame up or down
- OffsetY: offsets the previous frame up or down
- ManipFade: the opacity value for scale and offsety

### WebGL

There's also a [WebGL version](https://github.com/ulyssesp/webgl-shader-jockey-ts) that you can [view online](https://ulyssesp.github.io/webgl-shader-jockey-ts).
