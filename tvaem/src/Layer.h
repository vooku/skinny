#pragma once

#include "ofMain.h"
#include "Mappable.h"

class Layer : public Mappable {
public:
    enum class BlendMode {
        Invalid     = -1,
        Normal      =  0, // s
        Multiply    =  1, // b * s
        Screen      =  2, // 1 - ((1 - b) * (1 - s))
        Darken      =  3, // min(b, s)
        Lighten     =  4, // max(b, s)
        LinearDodge =  5, // s + b
        Difference  =  6, // |b - s|
        Exclusion   =  7, // b + s + 2 * b * s
        Count       =  8  // Used for iteration
    };

    enum class ErrorType {
        Invalid, Failed
    };

    static const int MIDI_OFFSET = 0;
    static const int ALPHA_MIDI_OFFSET = 0;

    static char* c_str(BlendMode blendMode);

    // #TODO use layer description?
    Layer(int id, const std::filesystem::path& path, midiNote note = -1, midiNote control = -1);
    Layer(int id, ErrorType error);
    ~Layer() override;

    bool reload(const std::filesystem::path& path);
    void bind();
    void unbind();
    bool isFrameNew();
    void update();

    virtual void play() override;
    virtual void pause() override;
    virtual void playPause() override;

    auto getWidth() const { return player_.getWidth(); }
    auto getHeight() const { return player_.getHeight(); }
    auto getId() const { return id_; }
    const auto& getName() const { return name_; }
    auto getBlendMode() const { return blendMode_; }
    bool isValid() const { return valid_; }
    float getAlpha() const { return alpha_; }
    auto getRetrigger() const { return retrigger_; }

    void setBlendMode(BlendMode newMode) { blendMode_ = newMode; }
    void setAlpha(int alpha) { alpha_ = (alpha < 0 ? 0 : alpha > 127 ? 127 : alpha) / 127.0f; }
    void setRetrigger(bool value) { retrigger_ = value; }

private:
    ofVideoPlayer player_;
    const int id_;
    std::string name_;
    bool valid_;
    bool retrigger_ = false;
    BlendMode blendMode_;
    float alpha_;
};
