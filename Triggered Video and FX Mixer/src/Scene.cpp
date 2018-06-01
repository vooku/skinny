#include "Scene.h"

Scene::Scene(const SceneDescription & description) {
    for (const auto& layer : description.layers) {
        auto newLayer = std::make_unique<Layer>(layer.id, layer.video);
        newLayer->setBlendMode(layer.blendMode);
        newLayer->replaceMidiMap(layer.midiMap);
        layers_.push_back(std::move(newLayer));
    }
}

void Scene::bindTextures() {
    for (auto& layer : layers_)
        layer->bind();
}

bool Scene::isFrameNew() {
    bool newFrame = false;
    for (auto& layer : layers_)
        newFrame |= layer->isFrameNew();
    return newFrame;
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
}

void Scene::playPauseLayer(int idx) {
    if (idx < layers_.size())
        layers_[idx]->playPause();
}

void Scene::setupUniforms(ofShader& shader) const {
    uniforms.nLayers = layers_.size();
    for (int i = 0; i < layers_.size() && i < maxLayers; ++i) {
        uniforms.active[i] = layers_[i]->isPlaying();
        uniforms.dimensions[i] = { layers_[i]->getWidth(), layers_[i]->getHeight() };
        uniforms.blendingModes[i] = static_cast<int>(layers_[i]->getBlendMode());
    }

    shader.setUniform1i("nLayers", uniforms.nLayers);
    shader.setUniform1iv("active", uniforms.active, uniforms.nLayers);
    shader.setUniform2iv("dimensions", reinterpret_cast<int*>(uniforms.dimensions), uniforms.nLayers);
    shader.setUniform1iv("blendingModes", uniforms.blendingModes, uniforms.nLayers);
}
