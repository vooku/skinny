#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "player.h"
#include <thread>

class MainComponent : public OpenGLAppComponent
{
public:
    friend class Player;

    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void initialise() override;
    void shutdown() override;
    void render() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    bool initShaders();
    void initBuffers();
    bool initAttributes();
    bool initUniforms();

    void enableAttributes();
    void sendUniforms();

    std::unique_ptr<OpenGLShaderProgram> shader;
    const char* vertexShader;
    const char* fragmentShader;
    GLuint vertexBuffer, indexBuffer;
    int nVertices, attsPerVertex;
    std::unique_ptr<OpenGLShaderProgram::Attribute> position, texCoordIn;
    OpenGLTexture texture;
    std::unique_ptr<OpenGLShaderProgram::Uniform> texCoordsDeform;
    GLuint textureID;

    std::unique_ptr<Player> player;
    std::unique_ptr<std::thread> playThread;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
