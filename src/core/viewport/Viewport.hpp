#pragma once
#include <glad/glad.h>

class Viewport {
public:
    Viewport(int width, int height);
    ~Viewport();

    void resize(int width, int height);
    void bind();
    void unbind();

    unsigned int getTexture() const;
    int getWidth() const;
    int getHeight() const;

private:
    unsigned int fbo = 0;
    unsigned int colorTexture = 0;
    unsigned int rbo = 0;

    int width;
    int height;

    void invalidate();
};
