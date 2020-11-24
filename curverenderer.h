#ifndef CURVERENDERER_H
#define CURVERENDERER_H

#include <QOpenGLShaderProgram>

#include "renderer.h"
#include "subdivisioncurve.h"

class CurveRenderer : public Renderer
{
public:
    ~CurveRenderer();

    void init(QOpenGLFunctions_4_1_Core* f, Settings* s);

    void initShaders();
    void initBuffers();

    void updateBuffers(SubdivisionCurve& sc);
    void draw(SubdivisionCurve& sc);

private:

    GLuint vao, vbo;
    int currentShader;
    QVector<QOpenGLShaderProgram*> shaders;
};

#endif // CURVERENDERER_H
