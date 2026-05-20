#ifndef SNAPPING_H
#define SNAPPING_H

#include <QPointF>
#include <QtMath>

namespace Snapping {
    const int GRID_SIZE = 20;
    inline QPointF snapToGrid(const QPointF& position) {
        qreal snappedX = qRound(position.x() / (qreal)GRID_SIZE) * GRID_SIZE;
        qreal snappedY = qRound(position.y() / (qreal)GRID_SIZE) * GRID_SIZE;
        return QPointF(snappedX, snappedY);
    }
}

#endif // SNAPPING_H
