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

    if (player)
        player->terminate = true;
    if (playThread)
        playThread->join();
}

void MainComponent::render()
{
    jassert(OpenGLHelpers::isContextActive());

    OpenGLHelpers::clear (Colours::pink);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    auto desktopScale = (float)openGLContext.getRenderingScale();
    glViewport(0, 0, roundToInt(desktopScale * getWidth()), roundToInt(desktopScale * getHeight()));

    shader->use();
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    enableAttributes();

    sendUniforms();

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

        "void main() {\n"
        "    texCoordOut = texCoordIn;\n"
        "    gl_Position = vec4(position, 1.0);\n"
        "}\n";

    fragmentShader =
        "varying vec2 texCoordOut;\n"

        "uniform sampler2D tex;\n"
        "uniform vec2 texCoordsDeform;\n"

        "void main() {\n"
        "    vec2 nCoords = vec2(texCoordOut.x, 1.0 - texCoordOut.y) * texCoordsDeform;\n"
        "    gl_FragColor = vec4(nCoords, 0.0, 1.0);\n"
        "    gl_FragColor = vec4(texture2D(tex, nCoords).rgb, 1.0);\n"
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

    texCoordsDeform = std::make_unique<OpenGLShaderProgram::Uniform>(*shader.get(), "texCoordsDeform");

    //glGenTextures(1, &textureID);
    //glBindTexture(GL_TEXTURE_2D, textureID);

    //float pixels[] = {
    //    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
    //    1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 0.0f
    //};
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
    
    

    
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

void MainComponent::sendUniforms()
{
//#define DBG_TEXTURE
#ifdef DBG_TEXTURE
    int w = 640, h = 360;
    const auto r = sqrt(w * w + h * h);
    auto pixels = std::vector<PixelARGB>(w * h);
    for (int y = 0; y < h; y++)
    {
        int line = y * w;
        for (int x = 0; x < w; x++)
        {
            auto rtmp = sqrt((x - 0.5 * w) * (x - 0.5 * w) + (y - 0.5 * h) * (y - 0.5 * h));
            juce::uint8 red = (
                    (x > 0.5 * w - 2 && x < 0.5 * w + 2) ||
                    (y > 0.5 * h - 2 && y < 0.5 * h + 2) ||
                    (x < 4) || (x > w - 4) ||
                    (y < 4) || (y > h - 4) ||
                    (rtmp > 0.07 * r && rtmp < 0.0725 * r)
                ) && (rtmp > 0.07 * r) 
                ? 255 : 0;
            pixels[line + x] = { 255, red, 0, 64 };
        }
    }
    texture.loadARGB(pixels.data(), w, h);
    texCoordsDeform->set(w / (float)texture.getWidth(), h / (float)texture.getHeight());
#else
    if (player && player->newTex) {
        texture.loadARGB(player->pixels.data(), player->w, player->h);
        player->newTex = false;
        texCoordsDeform->set(player->w / (float)texture.getWidth(), player->h / (float)texture.getHeight());
    }
#endif
    texture.bind();

    //shader->setUniform("tex", 0);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
