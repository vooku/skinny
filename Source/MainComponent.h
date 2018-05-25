#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class MainComponent   : public OpenGLAppComponent
{
public:
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
    void enableAttributes();

    std::unique_ptr<OpenGLShaderProgram> shader;
    const char* vertexShader;
    const char* fragmentShader;
    GLuint vertexBuffer, indexBuffer;
    int nVertices, attsPerVertex;
    std::unique_ptr<OpenGLShaderProgram::Attribute> position, texCoordIn;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
