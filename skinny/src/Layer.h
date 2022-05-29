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
	virtual ~Layer() override = default;

	virtual bool reload(const std::filesystem::path& path) = 0;
	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual bool isFrameNew() = 0;
	virtual void update() { };

	virtual float getWidth() const = 0;
	virtual float getHeight() const = 0;

	auto getId() const { return id_; }
	const auto& getName() const { return name_; }
	auto getBlendMode() const { return blendMode_; }
	bool isValid() const { return valid_; }
	float getAlpha() const { return clamp(ccValue_, 0, MAX_7BIT) / MAX_7BITF; }
	auto getRetrigger() const { return retrigger_; }

	void setBlendMode(BlendMode newMode) { blendMode_ = newMode; }
	void setRetrigger(bool value) { retrigger_ = value; }
	void setAlpha(int alpha) { ccValue_ = alpha; }

protected:
	const int id_;
	std::string name_;
	bool valid_;
	bool retrigger_ = false;
	BlendMode blendMode_;
};

} // namespace skinny
