#pragma once

#include "ofMain.h"
#include "Layer.h"

namespace skinny {

class VideoLayer : public Layer {
public:
    VideoLayer(int id, const std::filesystem::path& path, midiNote note = -1, midiNote control = -1);
    VideoLayer(int id, ErrorType error);
    virtual ~VideoLayer() override;

		virtual bool reload(const std::filesystem::path& path) override;
		virtual bool isLoaded() const;
    virtual void bind() override;
    virtual void unbind() override;
    virtual bool isFrameNew() override;
    virtual void update() override;

    virtual void play() override;
    virtual void pause() override;
    virtual void playPause() override;

    virtual float getWidth() const override { return player_.getWidth(); }
    virtual float getHeight() const override { return player_.getHeight(); }

private:
    ofVideoPlayer player_;

};

} // namespace skinny
