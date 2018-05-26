#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize (1280, 720);
}

MainComponent::~MainComponent()
{
    shutdownOpenGL();
}

//==============================================================================
void MainComponent::initialise()
{
    initShaders();
    initBuffers();
    initAttributes();
    initUniforms();
}

void MainComponent::shutdown()
{
    openGLContext.extensions.glDeleteBuffers(1, &vertexBuffer);
    openGLContext.extensions.glDeleteBuffers(1, &indexBuffer);
    texture.release();
    playThread->join();
}

void MainComponent::render()
{
    jassert(OpenGLHelpers::isContextActive());

    OpenGLHelpers::clear (Colours::pink);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    openGLContext.extensions.glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    auto desktopScale = (float)openGLContext.getRenderingScale();
    glViewport(0, 0, roundToInt(desktopScale * getWidth()), roundToInt(desktopScale * getHeight()));

    shader->use();
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    enableAttributes();

    if (player->newTex) {
        texture.loadARGBFlipped(player->pixels.data(), player->w, player->h);
        player->newTex = false;
    }

    texture.bind();
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //openGLContext.extensions.glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, textureID);
    //shader->setUniform("texture", (int)textureID);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // You can add your component specific drawing code here!
    // This will draw over the top of the openGL background.
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

bool MainComponent::initShaders()
{
    vertexShader =
        "attribute vec3 position;\n"
        "attribute vec2 texCoordIn;\n"

        "varying vec2 texCoordOut;\n"

        "void main()\n"
        "{\n"
        "    texCoordOut = texCoordIn;\n"
        "    gl_Position = vec4(position, 1.0);\n"
        "}\n";

    fragmentShader =
        "varying vec2 texCoordOut;\n"

        "uniform sampler2D texture;"

        "void main()\n"
        "{\n"
        //"    vec4 tex = texture2D(texture, texCoordOut)\n;"
        //"    gl_FragColor = vec4(mix(vec3(texCoordOut, 0.0), tex.rgb, tex.a), 1.0);\n"
        "    gl_FragColor = vec4(texture2D(texture, texCoordOut).rgb, 1.0);\n"
        "}\n";

    bool success = true;
    String status;
    shader = std::make_unique<OpenGLShaderProgram>(openGLContext);
    success &= shader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(vertexShader));
    success &= shader->addFragmentShader(OpenGLHelpers::translateFragmentShaderToV3(fragmentShader));
    success &= shader->link();

    if (!success) {
        status = shader->getLastError();
    }
    else {
        status = "GLSL: v" + String(OpenGLShaderProgram::getLanguageVersion(), 2);
    }

    DBG(status);
    return success;
}

void MainComponent::initBuffers()
{
    nVertices = 4;
    attsPerVertex = 5;
    const GLfloat vertices[] = {
        -1, -1, 0,    0, 0,
        -1,  1, 0,    0, 1,
         1,  1, 0,    1, 1,
         1, -1, 0,    1, 0
    };

    const GLuint indices[] = {
        0, 2, 1,
        0, 3, 2
    };

    openGLContext.extensions.glGenBuffers(1, &vertexBuffer);
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, nVertices * attsPerVertex * sizeof(float), vertices, GL_STATIC_DRAW);

    openGLContext.extensions.glGenBuffers(1, &indexBuffer);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

bool MainComponent::initAttributes()
{
    position = std::make_unique<OpenGLShaderProgram::Attribute>(*shader.get(), "position");
    texCoordIn = std::make_unique<OpenGLShaderProgram::Attribute>(*shader.get(), "texCoordIn");
    return true;
}

bool MainComponent::initUniforms()
{   
    player = std::make_unique<Player>("./videos/Britney_Spears_-_Toxic_Official_Video.mp4");
    playThread = std::make_unique<std::thread>(&Player::play, player.get());

    //glGenTextures(1, &textureID);
    //glBindTexture(GL_TEXTURE_2D, textureID);

    
    //pixels.resize(w * h);
    
    

    
    //Image image = ImageFileFormat::loadFrom(File::getCurrentWorkingDirectory().getChildFile("./nge.jpg").getFullPathName());
    
    //texture.loadImage(image);
    //Image::BitmapData srcData(image, Image::BitmapData::readOnly);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), image.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, srcData.data);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    return true;
}

void MainComponent::enableAttributes()
{
    if (position.get() != nullptr) {
        openGLContext.extensions.glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, attsPerVertex * sizeof(float), 0);      
        openGLContext.extensions.glEnableVertexAttribArray(position->attributeID);
    }

    if (texCoordIn.get() != nullptr) {
        openGLContext.extensions.glVertexAttribPointer(texCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, attsPerVertex * sizeof(float), (GLvoid*)(sizeof(float) * 3));
        openGLContext.extensions.glEnableVertexAttribArray(texCoordIn->attributeID);
    }
}
