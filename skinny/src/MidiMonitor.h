#pragma once

#include "ofMain.h"
#include "base.h"
#include "Mappable.h"

namespace skinny {

//--------------------------------------------------------------
class MidiMonitor : public ofThread, public Mappable {
public:
	virtual void setup() override;
	virtual void exit() override;

	virtual void onNoteOn(NoteMessage& msg) override;
	virtual void onNoteOff(NoteMessage& msg) override;
	virtual void onControlChange(ControlChangeMessage& msg) override;

	std::string getCurrentMsg() const;

	bool on_ = false;

private:
	void threadedFunction() override;

	std::string currentMsg;
	ofTimer timer_;

};

}
