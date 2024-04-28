#pragma once

#include "Layer.h"

namespace skinny {

class ImageLayer : public Layer {
public:
	ImageLayer(int id, const std::filesystem::path& path, midiNote note = -1, midiNote control = -1);
	ImageLayer(int id, ErrorType error);
	virtual ~ImageLayer() override = default;

	virtual bool reload(const std::filesystem::path& path) override;
	virtual bool isLoaded() const;
	virtual void bind() override;
	virtual void unbind() override;
	virtual bool isFrameNew() override;

	virtual float getWidth() const override { return image_.getWidth(); }
	virtual float getHeight() const override { return image_.getHeight(); }

private:
	ofImage image_;

};

} // namespace skinny
