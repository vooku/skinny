#pragma once

#include "ofMain.h"
#include "base.h"

namespace skinny {

//--------------------------------------------------------------
class MidiMonitor : public ofThread {
public:
	void init();
	void done();

	void onNoteOn(NoteMessage& msg);
	void onNoteOff(NoteMessage& msg);
	void onControlChange(ControlChangeMessage& msg);

	std::string getCurrentMsg() const;

	bool on_ = false;

private:
	void threadedFunction() override;

	std::string currentMsg;
	ofTimer timer_;

};

}
