# Skinny Mixer

Mix video loops and apply simple effects triggered by MIDI events.

Using [openFrameworks](https://openframeworks.cc) with additional plugins [ofxMidi](https://github.com/danomatika/ofxMidi), [ofxArgs](https://github.com/outsidecontext/ofxArgs), [ofxDatGui](https://braitsch.github.io/ofxDatGui/index.html) and [ofxSpout](https://github.com/elliotwoods/ofxSpout).

Originally prototyped at [FEL CTU](https://www.fel.cvut.cz/en).

## Known Issues

Multiple MIDI devices of the same kind (specifically with the same name) not supported because of the [MIDI middleware implementation](https://github.com/danomatika/ofxMidi/issues/82).
