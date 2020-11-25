#include "subdivisioncurve.h"
#include <QTextStream>
#include <QDebug>

// Represents a subdivided point
struct SubdividedPoint {
    // Actual computed point
    QVector2D point;
    // Associated control point indices
    QVector<int> controlPointIndices;
};


QVector<SubdividedPoint> subdivideCurve(QVector<SubdividedPoint>& input, QVector<int>& odd, QVector<int>& even);

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
    QVector<SubdividedPoint> result(netCoords.size());
    for (int i = 0; i < result.size(); ++i) {

        auto point = SubdividedPoint();
        auto cpVec = QVector<int>();
        cpVec.push_back(i);
        point.point = netCoords[i];
        point.controlPointIndices = cpVec;
        result[i] = point;
    }

    for (int i = 0; i < subdivisions; ++i) {
        result = subdivideCurve(result, secondStencil, firstStencil);
    }

    // Construct the arrays with points controlled by each control point.
    // This gives only the modified control points, in other words,
    // the start and end line segments of the controller segments, also move, but the connected control point does not.
    // It's a bit hard to explain above, but I hope it's clear when you select a point and move it.
    QVector<QVector<QVector2D>> influenceSegments;
    for (int i = 0; i < netCoords.size(); ++i) {
        QVector<QVector2D> influencedPoints;
        for (int j = 0; j < result.size(); ++j) {
            if (result[j].controlPointIndices.contains(i)) {
                influencedPoints.push_back(result[j].point);
            }
        }
        influenceSegments.push_back(influencedPoints);
    }
    influenceCurves = influenceSegments;


    QVector<QVector2D> result2;
    for(auto& elem : result) {
        result2.push_back(elem.point);
    }

    subdividedCurve = result2;
}

QVector<SubdividedPoint> subdivideCurve(QVector<SubdividedPoint>& input, QVector<int>& odd, QVector<int>& even) {
    QVector<SubdividedPoint> result;

    result.push_back(input.first());

    // Iterate over input curve points
    for (int i = 0; i < input.size(); ++ i) {

        // First compute even stencil if it fits for points up to i + stencil size
        if (i + even.size() - 1 < input.size()) {
            int denominator = std::accumulate(even.begin(), even.end(), 0);
            QVector2D numerator;
            for (int k = 0; k < even.size(); ++k) {
                numerator += (input[i + k].point * even[k]);
            }
            auto point = SubdividedPoint();
            point.point = numerator / denominator;
            point.controlPointIndices = QVector<int>();

            // Generate control point influence data
            for (int k = 0; k < even.size(); ++k) {
                for (auto& elem : input[i + k].controlPointIndices) {
                    point.controlPointIndices.push_back(elem);
                }
            }

            result.push_back(point);
        }

        // Secondly compute odd stencil if it fits for points up to i + stencil size
        if (i + odd.size() - 1 < input.size()) {
            int denominator = std::accumulate(odd.begin(), odd.end(), 0);
            QVector2D numerator;
            for (int k = 0; k < odd.size(); ++k) {
                numerator += (input[i + k].point * odd[k]);
            }
            auto point = SubdividedPoint();
            point.point = numerator / denominator;
            point.controlPointIndices = QVector<int>();

            // Generate control point influence data
            for (int k = 0; k < odd.size(); ++k) {
                for (auto& elem : input[i + k].controlPointIndices) {
                    point.controlPointIndices.push_back(elem);
                }
            }

            result.push_back(point);
        }
    }


    result.push_back(input.last());

    return result;
}
