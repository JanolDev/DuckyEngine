#include "Viewport.hpp"
#include <iostream>

Viewport::Viewport(int w, int h)
    : width(w), height(h) {
    invalidate();
}

Viewport::~Viewport() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &colorTexture);
    glDeleteRenderbuffers(1, &rbo);
}

void Viewport::invalidate() {
    if (fbo) {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &colorTexture);
        glDeleteRenderbuffers(1, &rbo);
    }

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer incomplete\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewport::resize(int w, int h) {
    if (w <= 0 || h <= 0) return;
    width = w;
    height = h;
    invalidate();
}

void Viewport::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
}

void Viewport::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Viewport::getTexture() const {
    return colorTexture;
}

int Viewport::getWidth() const {
    return width;
}

int Viewport::getHeight() const {
    return height;
}
