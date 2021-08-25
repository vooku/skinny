#pragma  once

#include "ofxMidi.h"

namespace skinny {
	
//--------------------------------------------------------------
class MidiController : public ofxMidiListener {
public:
	void setup(bool verbose);
	void exit();

	void newMidiMessage(ofxMidiMessage& msg) override;

private:
	std::vector<std::unique_ptr<ofxMidiIn>> midiInputs_;

};

}