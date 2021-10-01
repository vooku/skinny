#include "MidiController.h"
#include "Status.h"
#include <algorithm>
#include <cctype>
#include <string>

namespace skinny {

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
	if (msg.status == MIDI_NOTE_ON)
	{
		auto noteMsg = NoteMessage{ msg.channel, static_cast<midiNote>(msg.pitch) };
		ofNotifyEvent(noteOnEvent, noteMsg);
	}

	if (msg.status == MIDI_NOTE_OFF)
	{
		auto noteMsg = NoteMessage{ msg.channel, static_cast<midiNote>(msg.pitch) };
		ofNotifyEvent(noteOffEvent, noteMsg);
	}

	if (msg.status == MIDI_CONTROL_CHANGE)
	{
		auto ccMsg = ControlChangeMessage{ msg.channel, static_cast<midiNote>(msg.control), msg.value };
		ofNotifyEvent(controlChangeEvent, ccMsg);
	}
}

//--------------------------------------------------------------
// hack; I didn't want it come to this
std::string stripFollowingNumbers(std::string deviceName)
{
	if (deviceName.empty())
		return deviceName;

	auto i = deviceName.size() - 1;
	while (isdigit(deviceName[i]) && i > 0)
		--i;

	return deviceName.substr(0, i);
}

//--------------------------------------------------------------
Devices MidiController::getPorts() const
{
	ofxMidiIn tmpMidiIn;
	Devices devices;

	for (int i = 0; i < tmpMidiIn.getNumInPorts(); ++i)
	{
		// multiple identical devices not supported, sorry
		const auto name = stripFollowingNumbers(tmpMidiIn.getInPortName(i));
		const auto open = 0 < std::count_if(midiInputs_.begin(), midiInputs_.end(), [&name](const MidiInputs::value_type& input)
		{
			return stripFollowingNumbers(input->getName()) == name;
		});

		devices.push_back({ open, name });
	}

	return devices;
}

//--------------------------------------------------------------
bool MidiController::connect(const std::string& deviceName)
{
	const auto connectedPort = std::find_if(midiInputs_.begin(), midiInputs_.end(), [&deviceName](const MidiInputs::value_type& input)
	{
		return stripFollowingNumbers(input->getName()) == deviceName;
	});

	if (connectedPort != midiInputs_.end())
		return true;

	auto input = std::make_unique<ofxMidiIn>();

	int portIndex = -1;
	for (int i = 0; i < input->getNumInPorts(); ++i)
	{
		const auto name = stripFollowingNumbers(input->getInPortName(i));
		if (name == deviceName)
		{
			portIndex = i;
			break;
		}
	}

	const auto portOpen = input->openPort(portIndex);
	if (portOpen)
	{
		input->addListener(this);
		midiInputs_.emplace(std::move(input));
	}

	return portOpen;
}

//--------------------------------------------------------------
void MidiController::disconnect(const std::string& deviceName)
{
	const auto connectedPort = std::find_if(midiInputs_.begin(), midiInputs_.end(), [&deviceName](const MidiInputs::value_type& input)
	{
		return stripFollowingNumbers(input->getName()) == deviceName;
	});

	if (connectedPort != midiInputs_.end())
	{
		(*connectedPort)->closePort();
		midiInputs_.erase(connectedPort);
	}
}

}
