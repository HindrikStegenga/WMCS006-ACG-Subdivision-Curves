#include "subdivisioncurve.h"
#include <QTextStream>
#include <QDebug>


QVector<QVector2D> subdivideCurve(QVector<QVector2D>& input, QVector<int>& odd, QVector<int>& even);

SubdivisionCurve::SubdivisionCurve()
{

}

void SubdivisionCurve::presetNet(int preset) {
    netCoords.clear();

    switch (preset) {
    case 0:
        // 'Pentagon'
        netCoords.reserve(5);
        netCoords.append(QVector2D(-0.25, -0.5));
        netCoords.append(QVector2D(-0.75, 0.0));
        netCoords.append(QVector2D(-0.25, 0.75));
        netCoords.append(QVector2D(0.75, 0.5));
        netCoords.append(QVector2D(0.5, -0.75));
        break;
    case 1:
        // 'Basis'
        netCoords.reserve(9);
        netCoords.append(QVector2D(-1.0, -0.25));
        netCoords.append(QVector2D(-0.75, -0.25));
        netCoords.append(QVector2D(-0.5, -0.25));
        netCoords.append(QVector2D(-0.25, -0.25));
        netCoords.append(QVector2D(0.0, 0.50));
        netCoords.append(QVector2D(0.25, -0.25));
        netCoords.append(QVector2D(0.5, -0.25));
        netCoords.append(QVector2D(0.75, -0.25));
        netCoords.append(QVector2D(1.0, -0.25));
        break;
    case 2:
        // 'G'
        netCoords.reserve(14);
        netCoords.append(QVector2D(0.75, 0.35));
        netCoords.append(QVector2D(0.75, 0.75));
        netCoords.append(QVector2D(-0.75, 0.75));
        netCoords.append(QVector2D(-0.75, -0.75));
        netCoords.append(QVector2D(0.75, -0.75));
        netCoords.append(QVector2D(0.75, 0.0));
        netCoords.append(QVector2D(0.0, 0.0));
        netCoords.append(QVector2D(0.0, -0.2));
        netCoords.append(QVector2D(0.55, -0.2));
        netCoords.append(QVector2D(0.55, -0.55));
        netCoords.append(QVector2D(-0.55, -0.55));
        netCoords.append(QVector2D(-0.55, 0.55));
        netCoords.append(QVector2D(0.55, 0.55));
        netCoords.append(QVector2D(0.55, 0.35));
        break;
    }
}

void SubdivisionCurve::addPoint(QVector2D p) {
    netCoords.append(p);
    //you should probably recalculate the curve
    recomputeCurve();
}

void SubdivisionCurve::setPointPosition(int idx, QVector2D p) {
    netCoords[idx] = p;
    //you should probably recalculate the curve
    recomputeCurve();
}

void SubdivisionCurve::removePoint(int idx) {
    netCoords.remove(idx);
    //you should probably recalculate the curve
    recomputeCurve();
}

void SubdivisionCurve::setMask(QString stringMask) {
    subdivMask.clear();

    QString trimmedMask;
    trimmedMask = stringMask.trimmed();

    // Convert to sequence of integers
    QTextStream intSeq(&trimmedMask);
    while (!intSeq.atEnd()) {
        int k;
        intSeq >> k;
        subdivMask.append(k);
    }

    // Stencils represent affine combinations (i.e. they should sum to unity)
    float normalizeValue = 0;

    firstStencil.clear();
    secondStencil.clear();

    for (int k = 0; k < subdivMask.size(); ++k) {
        if (k % 2) {
            normalizeValue += subdivMask[k];
            firstStencil.append(subdivMask[k]);
        }
        else {
            secondStencil.append(subdivMask[k]);
        }
    }

    qDebug() << ":: Extracted stencils" << firstStencil << "and" << secondStencil;
}

void SubdivisionCurve::setSubdivisionSteps(int steps) {
    this->lastSubdivisonSteps = steps;
}

int SubdivisionCurve::findClosest(QVector2D p) {
    int ptIndex = -1;
    float currentDist, minDist = 4;


    for (int k = 0; k < netCoords.size(); k++) {
        currentDist = netCoords[k].distanceToPoint(p);
        if (currentDist < minDist) {
            minDist = currentDist;
            ptIndex = k;
        }
    }

    return ptIndex;
}

void SubdivisionCurve::recomputeCurve() {

    int subdivisions = this->lastSubdivisonSteps;
    QVector<QVector2D> result = netCoords;
    for (int i = 0; i < subdivisions; ++i) {
        result = subdivideCurve(result, secondStencil, firstStencil);
    }
    subdividedCurve = result;
}

QVector<QVector2D> subdivideCurve(QVector<QVector2D>& input, QVector<int>& odd, QVector<int>& even) {
    QVector<QVector2D> result;
    result.push_back(input.first());

    for (int i = 1; i < input.size(); ++ i) {
        int denominator = 0;
        for (int j = 0; j < even.size(); ++j) {
            denominator += even[j];
        }
        QVector2D nominator = (input[i - 1] * even[0]) + (input[i] * even[1]);
        result.push_back(nominator / denominator);

        if (i != input.size() - 1) {
            int denominator = 0;
            for (int j = 0; j < odd.size(); ++j) {
                denominator += odd[j];
            }
            QVector2D nominator = (input[i - 1] * odd[0]) + (input[i] * odd[1]) + (input[i + 1] * odd[2]);
            result.push_back(nominator / denominator);
        }
    }

    result.push_back(input.last());
    return result;
}
