#pragma once

#include "base.h"

namespace skinny {

class Status
{
public:
    Status(Status const&) = delete;
    void operator=(Status const&) = delete;

    static Status& instance()
    {
        static Status status;
        return status;
    }

    bool exit = false;    //!< The app should exit.
    LoadDir loadDir = LoadDir::None;
    int jumpToIndex = 0;  //!< In the case of LoadDir::Jump

private:
    Status() = default;

};

} // namespace skinny
