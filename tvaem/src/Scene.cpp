#include "Scene.h"

Scene::Scene(const SceneDescription & description) :
    name_(description.name),
    layerNames(description),
    effectNames(description),
    valid_(true)
{
    for (const auto& layer : description.layers) {
        if (layers_.size() >= maxLayers)
            break;

        auto newLayer = std::make_unique<Layer>(layers_.size(), layer.path, layer.midiMap);
        newLayer->setBlendMode(layer.blendMode);
        if (newLayer->isValid())
            layers_.push_back(std::move(newLayer));
        else
            valid_ = false;
    }

    for (const auto& effect : description.effects) {
        effects_.emplace(effect.type, effect.midiMap);
    }

    uniforms_.inverse = false;
    uniforms_.reducePalette = false;
    uniforms_.colorShift = 0;
}

void Scene::bindTextures()
{
    for (auto& layer : layers_)
        layer->bind();
}

bool Scene::isFrameNew()
{
    bool newFrame = false;
    for (auto& layer : layers_)
        newFrame |= layer->isFrameNew();
    return newFrame;
}

bool Scene::hasActiveFX() const
{
    return uniforms_.inverse || uniforms_.reducePalette || uniforms_.colorShift;
}

void Scene::newMidiMessage(ofxMidiMessage & msg) {
    auto noteOn = msg.status == MIDI_NOTE_ON;
    auto noteOff = msg.status == MIDI_NOTE_OFF;
    int note = msg.pitch;

    if (!noteOn && !noteOff)
        return;

    for (auto& layer : layers_) {
        if (layer->containsMidiNote(note)) {
            if (noteOn)
                layer->play();
            else if (noteOff)
                layer->pause();
        }
    }

    for (auto& effect : effects_) {
        if (effect.second.containsMidiNote(note)) {
            if (noteOn)
                effect.second.play();
            else if (noteOff)
                effect.second.pause();
        }
    }
}

void Scene::playPauseLayer(int idx) 
{
    if (idx < layers_.size())
        layers_[idx]->playPause();
}

void Scene::setupUniforms(ofShader& shader) const 
{
    uniforms_.nLayers = layers_.size();
    for (int i = 0; i < layers_.size() && i < maxLayers; ++i) {
        uniforms_.playing[i] = layers_[i]->isPlaying();
        uniforms_.dimensions[i] = { layers_[i]->getWidth(), layers_[i]->getHeight() };
        uniforms_.blendingModes[i] = static_cast<int>(layers_[i]->getBlendMode());
    }

    shader.setUniform1i("nLayers", uniforms_.nLayers);
    shader.setUniform1iv("playing", uniforms_.playing, uniforms_.nLayers);
    shader.setUniform2iv("dimensions", reinterpret_cast<int*>(uniforms_.dimensions), uniforms_.nLayers);
    shader.setUniform1iv("blendingModes", uniforms_.blendingModes, uniforms_.nLayers);

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

Scene::LayerNames::LayerNames(const SceneDescription & description)
{
    for (const auto& layer : description.layers) {
        names.push_back(layer.path.filename().string());
    }
}

Scene::EffectNames::EffectNames(const SceneDescription & description)
{
    for (const auto& effect : description.effects) {
        names.push_back(Effect::c_str(effect.type));
    }
}
