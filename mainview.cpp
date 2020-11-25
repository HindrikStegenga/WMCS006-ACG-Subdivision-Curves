#include "mainview.h"
#include "math.h"

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "✓✓ MainView constructor";

    updateUniformsRequired = true;
}

MainView::~MainView() {
    qDebug() << "✗✗ MainView destructor";
    debugLogger->stopLogging();

    delete debugLogger;
}

void MainView::initializeGL() {

    initializeOpenGLFunctions();
    qDebug() << ":: OpenGL initialized";

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ), this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    // If the application crashes here, try setting "MESA_GL_VERSION_OVERRIDE = 4.1"
    // and "MESA_GLSL_VERSION_OVERRIDE = 410" in Projects (left panel) -> Build Environment

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    //grab the opengl context
    QOpenGLFunctions_4_1_Core *functions = this->context()->versionFunctions<QOpenGLFunctions_4_1_Core>();

    // initialize renderers here with the current context
    cnr.init(functions, &settings);
    curveRenderer.init(functions, &settings);

    //load preset curve net
    subCurve.presetNet(0);

    //update buffers
    updateBuffers();
}

void MainView::updateUniforms() {

    //  glUniform...();

    updateUniformsRequired = false;
}

void MainView::updateBuffers() {
    cnr.updateBuffers(subCurve);
    curveRenderer.updateBuffers(subCurve);
    update();
}

void MainView::paintGL() {

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if (updateUniformsRequired) {
        updateUniforms();
    }

    if (settings.showNet) {
        cnr.draw(subCurve);
    }

    if (settings.showCurvePts) {
        curveRenderer.draw(subCurve);
    }

}

QVector2D MainView::mouseToScene(QMouseEvent *event) {
    float xRatio, yRatio, xScene, yScene;

    xRatio = float(event->x()) / float(width());
    yRatio = float(event->y()) / float(height());

    // By default, the drawing canvas is the square [-1,1]^2:
    xScene = (1-xRatio)*-1 + xRatio*1;
    // Note that the origin of the canvas is in the top left corner (not the lower left).
    yScene = yRatio*-1 + (1-yRatio)*1;

    return QVector2D(xScene, yScene);
}

void MainView::mousePressEvent(QMouseEvent *event) {
    // In order to allow keyPressEvents:
    setFocus();

    QVector2D scenePos = mouseToScene(event);

    switch (event->buttons()) {
    case Qt::LeftButton:
        if (settings.selectedPt > -1) {
            // De-select control point
            settings.selectedPt = -1;
            setMouseTracking(false);
            update();
        } else {
            // Add new control point
            subCurve.addPoint(scenePos);
            updateBuffers();
            update();
        }
        break;
    case Qt::RightButton:
        // Select control point
        settings.selectedPt = subCurve.findClosest(scenePos);
        updateBuffers();
        update();
        break;
    }

}

void MainView::mouseMoveEvent(QMouseEvent *event) {
    if (settings.selectedPt > -1) {
        QVector2D scenePos = mouseToScene(event);

        // Update position of the control point
        subCurve.setPointPosition(settings.selectedPt, scenePos);
        updateBuffers();
    }
}

void MainView::keyPressEvent(QKeyEvent *event) {

    // Only works when the widget has focus!

    switch(event->key()) {
    case 'G':
        if (settings.selectedPt > -1) {
            // Grab selected control point
            setMouseTracking(true);
        }
        break;
    case 'X':
        if (settings.selectedPt > -1) {
            // Remove selected control point
            subCurve.removePoint(settings.selectedPt);
            settings.selectedPt = -1;
            updateBuffers();
        }
        break;
    }

}


void MainView::onMessageLogged( QOpenGLDebugMessage message ) {
    bool log = false;

    // Format based on source
#define CASE(c) case QOpenGLDebugMessage::c: :
    switch (message.source())
    {
      case QOpenGLDebugMessage::APISource :
      case QOpenGLDebugMessage::WindowSystemSource :
      case QOpenGLDebugMessage::ThirdPartySource :
      case QOpenGLDebugMessage::ApplicationSource :
      case QOpenGLDebugMessage::OtherSource :
      case QOpenGLDebugMessage::InvalidSource :
      case QOpenGLDebugMessage::AnySource : {
            log = true;
            break;
        }
      case QOpenGLDebugMessage::ShaderCompilerSource : break;
    }
#undef CASE

    if(log) qDebug() << "  → Log:" << message;
}
