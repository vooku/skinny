#pragma once

#include "base.h"
#include "ofMain.h"
#include "ofxMidi.h"

namespace skinny {

//--------------------------------------------------------------
struct MidiDeviceInfo
{
  bool open;
  string name;
};

using Devices = std::vector<MidiDeviceInfo>;

//--------------------------------------------------------------
class MidiController : public ofxMidiListener
{
public:
  void exit();

  void newMidiMessage(ofxMidiMessage& msg) override;

  Devices getPorts() const;
  bool connect(const std::string& deviceName);
  void disconnect(const std::string& deviceName);

  ofEvent<NoteMessage> noteOnEvent;
  ofEvent<NoteMessage> noteOffEvent;
  ofEvent<ControlChangeMessage> controlChangeEvent;

private:
  using MidiInputs = std::set<std::unique_ptr<ofxMidiIn>>;
  MidiInputs midiInputs_;
};

} // namespace skinny