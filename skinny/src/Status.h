#pragma once

#include "base.h"
#include "Show.h"
#include "Meta.h"
#include "Gui.h"

namespace skinny {

class Status
{
public:
    Status(Status const&) = delete;
    void operator=(Status const&) = delete;

    static Status& instance();

    using ShowPtr = std::shared_ptr<Show>;
    ShowPtr&          show();
    ShowDescription&  showDescription();
    Gui&              gui();

    bool exit = false;    //!< The app should exit.
    LoadDir loadDir = LoadDir::None;
    int jumpToIndex = 0;  //!< In the case of LoadDir::Jump

private:
    Status() = default;

    ShowPtr show_;
		ShowDescription showDescription_;
    Gui gui_;

};

Status& getStatus();

} // namespace skinny
