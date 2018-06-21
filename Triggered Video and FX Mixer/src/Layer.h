#pragma once

#include "ofMain.h"
#include "Mappable.h"

class Layer : public Mappable {
public:
    enum class BlendMode {
        Invalid = -1,
        Normal = 0,      // s
        Multiply = 1,    // b * s
        Screen = 2,      // 1 - ((1 - b) * (1 - s))
        Darken = 3,      // min(b, s)
        Lighten = 4,     // max(b, s)
        LinearDodge = 5, // s + b
        Difference = 6,  // |b - s|
        Exclusion = 7    // b + s + 2 * b * s
    };

    Layer(int id);
    Layer(int id, const std::string& filename);
    Layer(int id, const std::string& filename, const MidiMap& map);
    ~Layer() override;

    bool reload(const std::string& filename);
    void bind();
    bool isFrameNew();

    void play() override;
    void pause() override;
    void playPause() override;

    float getWidth() const { return player_.getWidth(); }
    float getHeight() const { return player_.getHeight(); }
    BlendMode getBlendMode() const { return blendMode_; }

    void setBlendMode(BlendMode newMode) { blendMode_ = newMode; }

private:
    ofVideoPlayer player_;
    const int id_;
    BlendMode blendMode_;
};
