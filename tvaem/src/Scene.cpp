#include "Scene.h"

void Scene::reload(const SceneDescription & description)
{
    name_ = description.name;
    valid_ = true;

    for (auto i = 0; i < MAX_LAYERS; ++i) {
        if (!description.layers[i].valid) {
            layers_[i].reset();
        }
        else {
            if (!layers_[i] || description.layers[i].path.filename() != layers_[i]->getName()) {
                auto newLayer = std::make_unique<Layer>(i, description.layers[i].path);
                if (newLayer->isValid())
                    layers_[i].reset(newLayer.release());
                else
                    valid_ = false;
            }
            layers_[i]->setBlendMode(description.layers[i].blendMode);
            layers_[i]->setMapping(description.layers[i].midiMap);
        }
    }

    for (const auto& effect : description.effects) {
        effects_[effect.type] = Effect(effect.midiMap);
    }
}

void Scene::bindTextures()
{
    for (auto& layer : layers_) {
         if (layer)
            layer->bind();
    }
}

bool Scene::isFrameNew()
{
    bool newFrame = false;
    for (auto& layer : layers_) {
        if (layer)
            newFrame |= layer->isFrameNew();
    }
    return newFrame;
}

bool Scene::hasActiveFX() const
{
    return uniforms_.inverse || uniforms_.reducePalette || uniforms_.colorShift;
}

Scene::FoundMappables Scene::newMidiMessage(const ofxMidiMessage & msg) {
    const auto noteOn = msg.status == MIDI_NOTE_ON;
    const auto noteOff = msg.status == MIDI_NOTE_OFF;
    const auto note = msg.pitch;
    const auto cc = msg.status == MIDI_CONTROL_CHANGE;
    const auto control = msg.control;
    const auto value = msg.value;

    if (!noteOn && !noteOff && !cc)
        return {};

    FoundMappables result;
    for (auto& layer : layers_) {
        if (!layer)
            break;

        if (layer->containsMidiNote(note)) {
            if (noteOn) {
                layer->play();
                result.layers.insert({ layer->getId(), true });
            } else if (noteOff) {
                layer->pause();
                result.layers.insert({ layer->getId(), false });
            }
        }

        if (cc && layer->getAlphaControl() == control) {
            layer->setAlpha(value);
        }
    }

    for (auto& effect : effects_) {
        if (effect.second.containsMidiNote(note)) {
            if (noteOn) {
                effect.second.play();
                result.effects.insert({ effect.first, true });
            } else if (noteOff) {
                effect.second.pause();
                result.effects.insert({ effect.first, false });
            }
        }
    }

    return result;
}

void Scene::playPauseLayer(int idx)
{
    if (idx < layers_.size() && layers_[idx])
        layers_[idx]->playPause();
}

void Scene::playPauseEffect(Effect::Type type)
{
    effects_.at(type).playPause();
}

void Scene::setupUniforms(ofShader& shader) const
{
    uniforms_.nLayers = static_cast<int>(layers_.size());
    for (auto i = 0; i < layers_.size(); ++i) {
        if (layers_[i]) {
            uniforms_.playing[i] = layers_[i]->isPlaying();
            uniforms_.dimensions[i] = { layers_[i]->getWidth(), layers_[i]->getHeight() };
            uniforms_.blendingModes[i] = static_cast<int>(layers_[i]->getBlendMode());
            uniforms_.alphas[i] = layers_[i]->getAlpha();
        } else {
            uniforms_.playing[i] = false;
        }
    }

    shader.setUniform1i("nLayers", uniforms_.nLayers);
    shader.setUniform1iv("playing", uniforms_.playing, uniforms_.nLayers);
    shader.setUniform2iv("dimensions", reinterpret_cast<int*>(uniforms_.dimensions), uniforms_.nLayers);
    shader.setUniform1iv("blendingModes", uniforms_.blendingModes, uniforms_.nLayers);
    shader.setUniform1fv("alphas", uniforms_.alphas, uniforms_.nLayers);

    auto it = effects_.find(Effect::Type::Inverse);
    if (it != effects_.end())
        uniforms_.inverse = it->second.isPlaying();
    it = effects_.find(Effect::Type::ReducePalette);
    if (it != effects_.end())
        uniforms_.reducePalette = it->second.isPlaying();
    it = effects_.find(Effect::Type::ColorShift);
    if (it != effects_.end())
        uniforms_.colorShift = it->second.isPlaying();
    it = effects_.find(Effect::Type::ColorShift2);
    if (it != effects_.end())
        uniforms_.colorShift2 = it->second.isPlaying();

    shader.setUniform1i("inverse", uniforms_.inverse);
    shader.setUniform1i("reducePalette", uniforms_.reducePalette);
    shader.setUniform1i("colorShift", uniforms_.colorShift);
    shader.setUniform1i("colorShift2", uniforms_.colorShift2);
}
