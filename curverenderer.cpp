#include "curverenderer.h"

CurveRenderer::~CurveRenderer() {

}

void CurveRenderer::init(QOpenGLFunctions_4_1_Core* f, Settings* s) {
    gl = f;
    settings = s;

    initShaders();
    initBuffers();
}

void CurveRenderer::initShaders() {

    //we use the qt wrapper functions for shader objects
    shaderProg = new QOpenGLShaderProgram();

    shaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    //add your geometry shader here
    //shaderProg->addShaderFromSourceCode(QOpenGLShader::Geometry, ":/???");

    shaderProg->link();
}

void CurveRenderer::initBuffers() {
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

void CurveRenderer::updateBuffers(SubdivisionCurve& sc) {
    QVector<QVector2D> netCoords = sc.getNetCoords();

    gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*netCoords.size(), netCoords.data(), GL_DYNAMIC_DRAW);
}


void CurveRenderer::draw(SubdivisionCurve& sc) {

    QVector<QVector2D> netCoords = sc.getNetCoords();
    shaderProg->bind();

    gl->glBindVertexArray(vao);

    // Draw control net
    gl->glDrawArrays(GL_LINE_STRIP, 0, netCoords.size());
    gl->glPointSize(1.0);
    gl->glDrawArrays(GL_POINTS, 0, netCoords.size());

    // Highlight selected control point
    if (settings->selectedPt > -1) {
        gl->glPointSize(12.0);
        gl->glDrawArrays(GL_POINTS, settings->selectedPt, 1);
    }

    gl->glBindVertexArray(0);


    shaderProg->release();
}
