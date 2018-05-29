#pragma once

#include "ofMain.h"

class Layer {
public:
    Layer(int id);
    Layer(int id, const char * filename);
    ~Layer();

    bool reload(const char * filename);
    void play();
    void pause();
    void playPause();
    bool update();
    
    float getWidth() const { return player_.getWidth(); }
    float getHeight() const { return player_.getHeight(); }
    float getAlpha() const { return globalAlpha_; }

private:
    ofVideoPlayer player_;
    bool paused_;
    const int id_;
    float globalAlpha_;
};
