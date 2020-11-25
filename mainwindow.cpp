#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    qDebug() << "✓✓ MainWindow constructor";
    ui->setupUi(this);

    ui->controlNet->setChecked(true);
    ui->curvePoints->setChecked(false);

    // Input restrictions for the Mask
    ui->subdivMask->setValidator(new QRegularExpressionValidator(QRegularExpression("(-?\\d+\\s)+(-?\\d+\\s?)")));



    // Initialise mask
    ui->mainView->subCurve.setMask(ui->subdivMask->text());
}

MainWindow::~MainWindow() {
    qDebug() << "✗✗ MainWindow destructor";
    delete ui;
}

void MainWindow::on_controlNet_toggled(bool checked) {
    ui->mainView->settings.showNet = checked;
    ui->mainView->update();
}

void MainWindow::on_curvePoints_toggled(bool checked) {
    ui->mainView->settings.showCurvePts = checked;
    ui->mainView->subCurve.recomputeCurve();
    ui->mainView->updateBuffers();
    ui->mainView->update();
}

void MainWindow::on_netPresets_currentIndexChanged(int index) {
    if (ui->mainView->isValid()) {
        ui->mainView->subCurve.presetNet(index);
    }
    ui->mainView->subCurve.recomputeCurve();
    ui->mainView->updateBuffers();
}

void MainWindow::on_subdivMask_returnPressed() {
    ui->mainView->subCurve.setMask(ui->subdivMask->text());
    ui->mainView->subCurve.recomputeCurve();
    ui->mainView->updateBuffers();
    ui->mainView->update();
}

void MainWindow::on_subdivSteps_valueChanged(int arg1) {
    ui->mainView->subCurve.setSubdivisionSteps(arg1);
    ui->mainView->subCurve.recomputeCurve();
    ui->mainView->updateBuffers();
    ui->mainView->update();
}

void MainWindow::on_curveShader_currentIndexChanged(int arg1) {
    ui->mainView->curveRenderer.setShaderIndex(arg1);
    ui->mainView->updateBuffers();
    ui->mainView->update();
}

