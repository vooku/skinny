#pragma once

#include "ofMain.h"
#include "Playable.h"

namespace skinny {

class Layer : public Playable {
public:
    enum class ErrorType {
        Invalid, Failed
    };

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
    float getAlpha() const { return clamp(ccValue_, 0, MAX_7BIT) / MAX_7BITF; }
    auto getRetrigger() const { return retrigger_; }

    void setBlendMode(BlendMode newMode) { blendMode_ = newMode; }
    void setRetrigger(bool value) { retrigger_ = value; }
    void setPalindrome(bool value);
    void setAlpha(int alpha) { ccValue_ = alpha; }

private:
    ofLoopType getLoopState() const;

    ofVideoPlayer player_;
    const int id_;
    std::string name_;
    bool valid_;
    bool retrigger_ = false;
    bool palindrome_ = false;
    BlendMode blendMode_;
};

} // namespace skinny
