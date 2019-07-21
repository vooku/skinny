#include "Scene.h"

void Scene::reload(const SceneDescription & description)
{
    name_ = description.name;
    alphaControl_ = description.alphaControl;
    valid_ = true;

    for (auto i = 0; i < MAX_LAYERS; ++i) {
        if (!description.layers[i].valid) {
            layers_[i].reset(/*new Layer(i, Layer::ErrorType::Invalid)*/);
        }
        else {
            if (!layers_[i] || description.layers[i].path.filename() != layers_[i]->getName()) {
                auto newLayer = std::make_unique<Layer>(i, description.layers[i].path, description.layers[i].note);
                if (newLayer->isValid())
                    layers_[i].reset(newLayer.release());
                else
                    layers_[i].reset(new Layer(i, Layer::ErrorType::Failed));
            }

            if (layers_[i]) {
                layers_[i]->setBlendMode(description.layers[i].blendMode);
                layers_[i]->setAlphaControl(description.layers[i].alphaControl);
                layers_[i]->setRetrigger(description.layers[i].retrigger);
            }
        }
    }
}

void Scene::bind()
{
    for (auto& layer : layers_) {
         if (layer)
            layer->bind();
    }
}


void Scene::unbind()
{
    for (auto& layer : layers_) {
        if (layer)
            layer->unbind();
    }
}

bool Scene::isFrameNew()
{
    auto newFrame = false;
    for (auto& layer : layers_) {
        if (layer)
            newFrame |= layer->isFrameNew();
    }
    return newFrame;
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

    if (cc && alphaControl_ ==  control) {
        setAlpha(value);
        return {};
    }

    FoundMappables result;
    for (auto& layer : layers_) {
        if (!layer)
            continue;

        if (layer->getNote() == note) {
            if (noteOn) {
                layer->play();
                result.layers.emplace_back(layer->getId(), true);
            } else if (noteOff) {
                layer->pause();
                result.layers.emplace_back(layer->getId(), false);
            }
        }

        if (cc && layer->getAlphaControl() == control) {
            layer->setAlpha(value);
        }
    }

    return result;
}

void Scene::playPauseLayer(int idx)
{
    if (idx < layers_.size() && layers_[idx])
        layers_[idx]->playPause();
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
    shader.setUniform1f("masterAlpha", uniforms_.alpha);

}
