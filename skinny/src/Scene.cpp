#include "Scene.h"

namespace skinny {

//--------------------------------------------------------------
void Scene::init()
{
  // nothing, layer init in reload
}

//--------------------------------------------------------------
void Scene::done()
{
  for (auto& layer : layers_)
  {
    if (layer)
      layer->exit();
  }
}

//--------------------------------------------------------------
void Scene::reload(const SceneDescription & description)
{
    name_ = description.name;
    valid_ = true;

    for (auto i = 0; i < MAX_LAYERS; ++i) {
        if (layers_[i])
          layers_[i]->exit();

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
                layers_[i]->setCc(description.layers[i].cc);
                layers_[i]->setAlpha(description.layers[i].alpha);
                layers_[i]->setRetrigger(description.layers[i].retrigger);
                layers_[i]->setup();
            }
        }
    }
}

//--------------------------------------------------------------
void Scene::bind()
{
    for (auto& layer : layers_) {
         if (layer)
            layer->bind();
    }
}

//--------------------------------------------------------------
void Scene::unbind()
{
    for (auto& layer : layers_) {
        if (layer)
            layer->unbind();
    }
}

//--------------------------------------------------------------
bool Scene::isFrameNew()
{
    auto newFrame = false;
    for (auto& layer : layers_) {
        if (layer)
            newFrame |= layer->isFrameNew();
    }
    return newFrame;
}

//--------------------------------------------------------------
void Scene::update()
{
    for (auto& layer : layers_) {
        if (layer) {
            layer->update();
        }
    }
}

//--------------------------------------------------------------
void Scene::playPauseLayer(int idx)
{
    if (idx < layers_.size() && layers_[idx])
        layers_[idx]->playPause();
}

//--------------------------------------------------------------
void Scene::setupUniforms(ofShader& shader) const
{
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

    shader.setUniform1iv("playing", uniforms_.playing, MAX_LAYERS);
    shader.setUniform2fv("dimensions", reinterpret_cast<float*>(uniforms_.dimensions), MAX_LAYERS);
    shader.setUniform1iv("blendingModes", uniforms_.blendingModes, MAX_LAYERS);
    shader.setUniform1fv("alphas", uniforms_.alphas, MAX_LAYERS);
}

} // namespace skinny
