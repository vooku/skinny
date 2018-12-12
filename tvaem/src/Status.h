#pragma once

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

    bool redraw = false;   //!< The canvas should be redrawn.
    bool exit = false;     //!< The app should exit.
    bool forward = false;  //!< Next scene should be loaded.
    bool backward = false; //!< Previous scene should be loaded.
    bool reload = false;   //!< Current scene should be reloaded.

private:
    Status() {}

};