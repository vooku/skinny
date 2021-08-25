#include "MidiController.h"
#include "Status.h"

namespace skinny {

//--------------------------------------------------------------
void MidiController::setup(bool verbose)
{ 
	if (verbose) {
		ofxMidiIn tmpMidiIn;
		tmpMidiIn.listInPorts();
	}

	ofxMidiIn tmpMidiIn;
	for (auto i = 0; i < tmpMidiIn.getNumInPorts(); ++i) {
		midiInputs_.push_back(std::make_unique<ofxMidiIn>());
		midiInputs_.back()->openPort(i);
		midiInputs_.back()->addListener(this);
		midiInputs_.back()->setVerbose(/*settings_.verbose*/false);
	}
}

//--------------------------------------------------------------
void MidiController::exit()
{
	for (auto& midiInput : midiInputs_) {
		midiInput->closePort();
	}
}

//--------------------------------------------------------------
void MidiController::newMidiMessage(ofxMidiMessage& msg)
{
	if (msg.channel != getStatus().showDescription().getMidiChannel()) {
		ofLog(OF_LOG_WARNING, "Received a MIDI message on an incorrect channel: %d %d %d.", msg.channel, msg.status, msg.pitch);
		return;
	}

	if (msg.status == MIDI_NOTE_ON && msg.pitch == getStatus().showDescription().getSwitchNote()) {
		Status::instance().loadDir = LoadDir::Forward;
	}
	else {
		auto activeMappables = getStatus().show()->newMidiMessage(msg);

		for (const auto& layer : activeMappables.layers) {
			getStatus().gui().setActiveLayer(layer.first, layer.second);
		}
		for (const auto& effect : activeMappables.effects) {
			getStatus().gui().setActiveEffect(effect.first, effect.second);
		}
	}
}

}
