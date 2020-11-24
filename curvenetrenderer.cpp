#include "curvenetrenderer.h"

CurveNetRenderer::~CurveNetRenderer() {
    gl->glDeleteVertexArrays(1, &vao);
    gl->glDeleteBuffers(1, &vbo);

    delete shaderProg;
}

void CurveNetRenderer::init(QOpenGLFunctions_4_1_Core* f, Settings* s) {
    gl = f;
    settings = s;

    initShaders();
    initBuffers();
}

void CurveNetRenderer::initShaders() {
    //we use the qt wrapper functions for shader objects
    shaderProg = new QOpenGLShaderProgram();

    //you can grab shaders from the resource file
    //you can add additional shaders by right-clicking resources.qrc->add existing files
    shaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    shaderProg->link();
}

void CurveNetRenderer::initBuffers() {
    // Pure OpenGL functions used here

    //create vao
    gl->glGenVertexArrays(1, &vao);
    //bind vao
    gl->glBindVertexArray(vao);
    //generate single buffer object
    gl->glGenBuffers(1, &vbo);
    //bind it
    gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //enable vertex attribute array with index 0 in the currently bound vao
    gl->glEnableVertexAttribArray(0);
    //tell the currently bound vao what the layout is of the vbo
    gl->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    //unbind
    gl->glBindVertexArray(0);
}

void CurveNetRenderer::updateBuffers(SubdivisionCurve& sc) {
    QVector<QVector2D> netCoords = sc.getNetCoords();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*netCoords.size(), netCoords.data(), GL_DYNAMIC_DRAW);

}


void CurveNetRenderer::draw(SubdivisionCurve& sc) {
    QVector<QVector2D> netCoords = sc.getNetCoords();

    shaderProg->bind();
    shaderProg->setUniformValue("inputColor", 1.0, 0.0, 0.0, 0.0);
    gl->glBindVertexArray(vao);

    // Draw control net
    gl->glDrawArrays(GL_LINE_STRIP, 0, netCoords.size());
    gl->glPointSize(12.0);
    gl->glLineWidth(3.0);
    gl->glDrawArrays(GL_POINTS, 0, netCoords.size());

    // Highlight selected control point
    if (settings->selectedPt > -1) {
        gl->glPointSize(20.0);
        gl->glDrawArrays(GL_POINTS, settings->selectedPt, 1);
    }

    gl->glBindVertexArray(0);


    shaderProg->release();
}
