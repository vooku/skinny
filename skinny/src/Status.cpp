#include "Status.h"

namespace skinny {

//--------------------------------------------------------------
Status& Status::instance()
{
    static Status status;
    return status;
}

//--------------------------------------------------------------
Status& getStatus()
{
  return Status::instance();
}

}