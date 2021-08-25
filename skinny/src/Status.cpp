#include "Status.h"

namespace skinny {

//--------------------------------------------------------------
Status& Status::instance()
{
    static Status status;
    return status;
}

//--------------------------------------------------------------
Status::ShowPtr& Status::show()
{
    static ShowPtr show_;
    return show_;
}

//--------------------------------------------------------------
ShowDescription& Status::showDescription()
{
    static ShowDescription showDescription_;
    return showDescription_;
}

//--------------------------------------------------------------
Gui& Status::gui()
{
    static Gui gui_;
    return gui_;
}

//--------------------------------------------------------------
MidiController& Status::midi()
{
  static MidiController midiController;
  return midiController;
}

//--------------------------------------------------------------
Status& getStatus()
{
  return Status::instance();
}

}