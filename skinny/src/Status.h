#pragma once

#include "base.h"
#include "Show.h"
#include "Meta.h"
#include "Gui.h"
#include "MidiController.h"

namespace skinny {

//--------------------------------------------------------------
class Status
{
public:
    Status(Status const&) = delete;
    void operator=(Status const&) = delete;

    static Status& instance();

    bool exit = false;    //!< The app should exit.
    LoadDir loadDir = LoadDir::None;
    int jumpToIndex = 0;  //!< In the case of LoadDir::Jump

    // shortcuts to objects from ofApp
    std::shared_ptr<Show> show;
    std::shared_ptr<ShowDescription> showDescription;
    std::shared_ptr<MidiController> midi;
    std::shared_ptr<Gui> gui;

private:
    Status() = default;

};

//--------------------------------------------------------------
Status& getStatus();

} // namespace skinny
