#pragma once

#include "FileSelector.h"
#include "base.h"

namespace skinny {

//--------------------------------------------------------------
FileSelector::FileSelector(const std::string& message, bool load) :
    message_(message),
    load_(load)
{
}

//--------------------------------------------------------------
const std::filesystem::path& FileSelector::getPath() const
{
    return path_;
}

//--------------------------------------------------------------
bool FileSelector::isLoading() const
{
    return load_;
}

//--------------------------------------------------------------
void FileSelector::threadedFunction()
{
    assert(isThreadRunning());

    auto openFileResult = load_ ? ofSystemLoadDialog(message_) : ofSystemSaveDialog(DEFAULT_FILENAME, message_);
    if (openFileResult.bSuccess) {
        path_ = openFileResult.getPath();
    }

    stopThread();
}

} // namespace skinny
