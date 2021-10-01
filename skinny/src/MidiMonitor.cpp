#include "MidiMonitor.h"
#include "Status.h"

namespace skinny {

//--------------------------------------------------------------
void MidiMonitor::init()
{
	timer_.setPeriodicEvent(MIDI_MSG_REFRESH_PERIOD);
	startThread();

	ofAddListener(getStatus().midi->noteOnEvent, this, &MidiMonitor::onNoteOn);
	ofAddListener(getStatus().midi->noteOffEvent, this, &MidiMonitor::onNoteOff);
	ofAddListener(getStatus().midi->controlChangeEvent, this, &MidiMonitor::onControlChange);
}

//--------------------------------------------------------------
void MidiMonitor::done()
{
	waitForThread();

	ofRemoveListener(getStatus().midi->noteOnEvent, this, &MidiMonitor::onNoteOn);
	ofRemoveListener(getStatus().midi->noteOffEvent, this, &MidiMonitor::onNoteOff);
	ofRemoveListener(getStatus().midi->controlChangeEvent, this, &MidiMonitor::onControlChange);
}

//--------------------------------------------------------------
void MidiMonitor::onNoteOn(NoteMessage& msg)
{
	std::stringstream sstream;
	sstream << msg.channel_ << " On " << msg.note_;
	currentMsg = sstream.str();
	timer_.reset();
}

//--------------------------------------------------------------
void MidiMonitor::onNoteOff(NoteMessage& msg)
{
	std::stringstream sstream;
	sstream << msg.channel_ << " Off " << msg.note_;
	currentMsg = sstream.str();
	timer_.reset();
}

//--------------------------------------------------------------
void MidiMonitor::onControlChange(ControlChangeMessage& msg)
{
	std::stringstream sstream;
	sstream << msg.channel_ << " CC " << msg.control_ << " " << msg.value_;
	currentMsg = sstream.str();
	timer_.reset();
}

//--------------------------------------------------------------
std::string MidiMonitor::getCurrentMsg() const
{
	return currentMsg;
}

//--------------------------------------------------------------
void MidiMonitor::threadedFunction()
{
	while (isThreadRunning())
	{
		timer_.waitNext();
		currentMsg.clear();
	}
}

}
