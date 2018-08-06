#pragma once

struct Status {
    bool redraw = false;   //!< The canvas should be redrawn.
    bool exit = false;     //!< The app should exit.
    bool forward = false;  //!< Next scene should be loaded.
    bool backward = false; //!< Previous scene should be loaded.
};