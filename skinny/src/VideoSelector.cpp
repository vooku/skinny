#pragma once

#include "VideoSelector.h"

namespace skinny {

//--------------------------------------------------------------
VideoSelector::Context::Context(IndexType index) :
    index(index)
{
}

//--------------------------------------------------------------
VideoSelector::VideoSelector(IndexType index)
    : ctx_(index)
{
}

//--------------------------------------------------------------
const VideoSelector::Context& VideoSelector::getContext() const
{
    return ctx_;
}

//--------------------------------------------------------------
void VideoSelector::threadedFunction()
{
    assert(isThreadRunning());

    auto openFileResult = ofSystemLoadDialog("Select a video on layer " + std::to_string(ctx_.index));
    if (openFileResult.bSuccess) {
        ctx_.path = openFileResult.getPath();
    }

    stopThread();
}

} // namespace skinny
