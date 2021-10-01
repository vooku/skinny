#include "MidiMonitor.h"
#include "Status.h"

namespace skinny {

//--------------------------------------------------------------
void MidiMonitor::init()
{
	timer_.setPeriodicEvent(MIDI_MSG_REFRESH_PERIOD);
	startThread();

	Mappable::init();
}

//--------------------------------------------------------------
void MidiMonitor::done()
{
	waitForThread();

	Mappable::done();
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
