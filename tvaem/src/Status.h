#pragma once

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

    bool exit = false;     //!< The app should exit.
    bool forward = false;  //!< Next scene should be loaded.
    bool backward = false; //!< Previous scene should be loaded.
    bool reload = false;   //!< Current scene should be reloaded.

private:
    Status() = default;

};

} // namespace skinny
