#include "curverenderer.h"

CurveRenderer::~CurveRenderer() {

}

QVector<QVector2D> computeAdjacencyBuffer(QVector<QVector2D> input);
void CurveRenderer::init(QOpenGLFunctions_4_1_Core* f, Settings* s) {
    gl = f;
    settings = s;
    currentShader = 2;
    initShaders();
    initBuffers();
}

void CurveRenderer::initShaders() {

    //we use the qt wrapper functions for shader objects
    //This is the shader generating curvature combs

    auto curvatureShader = new QOpenGLShaderProgram();
    curvatureShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    curvatureShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");
    curvatureShader->link();

    shaders.append(curvatureShader);

    auto curvatureCombShader = new QOpenGLShaderProgram();
    curvatureCombShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    curvatureCombShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");
    curvatureCombShader->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/curvature_comb.glsl");
    curvatureCombShader->link();

    shaders.append(curvatureCombShader);

    //This is the shader generating curvature colors
    auto curvatureColorShader = new QOpenGLShaderProgram();
    curvatureColorShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    curvatureColorShader->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/curvature_color.glsl");
    curvatureColorShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");
    curvatureColorShader->link();

    shaders.append(curvatureColorShader);

    //This is the shader generating curvature circles
    auto curvatureCircleShader = new QOpenGLShaderProgram();
    curvatureCircleShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    curvatureCircleShader->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/curvature_circle.glsl");
    curvatureCircleShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");
    curvatureCircleShader->link();

    shaders.append(curvatureCircleShader);
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
    QVector<QVector2D> netCoords = sc.getSubdivisionCoords();
    netCoords = computeAdjacencyBuffer(netCoords);

    gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*netCoords.size(), netCoords.data(), GL_DYNAMIC_DRAW);
}

void CurveRenderer::draw(SubdivisionCurve& sc) {

    QVector<QVector2D> netCoords = sc.getSubdivisionCoords();
    netCoords = computeAdjacencyBuffer(netCoords);

    // Bind shader program.
    shaders[currentShader]->bind();
    // Sadly I need to do name matching instead of by location.
    // That's not supported on my system unfortunately.
    shaders[currentShader]->setUniformValue("inputColor", 0.0, 1.0, 0.0, 1.0);

    gl->glBindVertexArray(vao);

    // Draw control net
    gl->glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, netCoords.size());
    gl->glLineWidth(3.0);

    gl->glBindVertexArray(0);


    shaders[currentShader]->release();
}

QVector<QVector2D> computeAdjacencyBuffer(QVector<QVector2D> input) {
    if(input.empty())
        return input;

    QVector<QVector2D> buffer;
    buffer.push_back(input.first());

    for(int i = 0; i < input.size(); ++i) {
        buffer.push_back(input[i]);
    }
    buffer.push_back(input.back());
    return buffer;
}
