#pragma once

#include "ofMain.h"

class Layer {
public:
    Layer(int id);
    Layer(int id, const char * filename);
    ~Layer();

    float getWidth() { return player_.getWidth(); }
    float getHeight() { return player_.getHeight(); }

    bool reload(const char * filename);
    void play() { player_.setPaused(paused_ = false); }
    void pause() { player_.setPaused(paused_ = true); }
    void playPause() { player_.setPaused(paused_ = !paused_); }
    bool update();
    
private:
    ofVideoPlayer player_;
    bool paused_;
    const int id_;
};
