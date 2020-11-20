#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QVector2D>
#include <QMouseEvent>

#include "curverenderer.h"
#include "curvenetrenderer.h"
#include "subdivisioncurve.h"



class MainView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core {

    Q_OBJECT

public:
    MainView(QWidget *parent = 0);
    ~MainView();


protected:
    void initializeGL();
    void paintGL();

    void updateBuffers();

    QVector2D mouseToScene(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);



private:
    QOpenGLDebugLogger* debugLogger;
    bool updateUniformsRequired;
    void updateUniforms();


    //add some renderers here
    CurveNetRenderer cnr;
    CurveRenderer curveRenderer;

    //we make mainwindow a friend so it can access the subcurve and settings
    friend class MainWindow;
    Settings settings;
    SubdivisionCurve subCurve;
private slots:
    void onMessageLogged( QOpenGLDebugMessage message );

};

#endif // MAINVIEW_H
