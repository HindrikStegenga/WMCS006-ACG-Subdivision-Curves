#ifndef CURVENETRENDERER_H
#define CURVENETRENDERER_H


#include <QOpenGLShaderProgram>

#include "renderer.h"
#include "subdivisioncurve.h"

class CurveNetRenderer : public Renderer
{
public:
    CurveNetRenderer() {}
    ~CurveNetRenderer();

    void init(QOpenGLFunctions_4_1_Core* f, Settings* s);

    void initShaders();
    void initBuffers();

    void updateBuffers(SubdivisionCurve& sc);
    void draw(SubdivisionCurve& sc);

private:

    GLuint vao, vbo;
    QOpenGLShaderProgram* shaderProg;
};

#endif // CURVENETRENDERER_H
