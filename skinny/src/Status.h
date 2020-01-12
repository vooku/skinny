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

    void resetLoad()
    {
        load = false;
        loadDir = LoadDir::None;
    }

    bool exit = false;    //!< The app should exit.
    bool load = false;    //!< Load a scene according to loadDir.
    LoadDir loadDir = LoadDir::None;

private:
    Status() = default;

};

} // namespace skinny
