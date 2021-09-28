#pragma  once

#include "ofMain.h"
#include "ofxMidi.h"
#include "base.h"

namespace skinny {

//--------------------------------------------------------------
class MidiController : public ofxMidiListener {
public:
	void setup(bool verbose);
	void exit();

	void newMidiMessage(ofxMidiMessage& msg) override;

	ofEvent<midiNote> noteOnEvent;
	ofEvent<midiNote> noteOffEvent;
	ofEvent<ControlChange> controlChangeEvent;

private:
	std::vector<std::unique_ptr<ofxMidiIn>> midiInputs_;

};

}