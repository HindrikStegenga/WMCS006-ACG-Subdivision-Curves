#ifndef SUBDIVISIONCURVE_H
#define SUBDIVISIONCURVE_H

#include <QVector>
#include <QVector2D>
#include <QString>

class SubdivisionCurve
{
public:
    SubdivisionCurve();

    inline QVector<QVector2D> getNetCoords() { return netCoords; }

    void setMask(QString stringMask);
    void presetNet(int preset);

    int findClosest(QVector2D p);

    void addPoint(QVector2D p);
    void setPointPosition(int idx, QVector2D p);
    void removePoint(int idx);

    void setSubdivisionSteps(int steps);
    void recomputeCurve();
private:
    QVector<int> subdivMask, firstStencil, secondStencil;
    int lastSubdivisonSteps;
    QVector<QVector2D> netCoords, subdividedCurve;
};

#endif // SUBDIVISIONCURVE_H
