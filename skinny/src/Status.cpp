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
    return show_;
}

//--------------------------------------------------------------
ShowDescription& Status::showDescription()
{
    return showDescription_;
}

//--------------------------------------------------------------
Gui& Status::gui()
{
    return gui_;
}

//--------------------------------------------------------------
Status& getStatus()
{
  return Status::instance();
}

}