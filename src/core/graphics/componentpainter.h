#ifndef COMPONENTPAINTER_H
#define COMPONENTPAINTER_H

#include <QRectF>
#include <QString>

class QPainter;

namespace ComponentPainter {
    const QRectF ZIGZAG_BOUND_RECT = QRectF(-42, -35, 84, 50);
    void drawZigZagComponent(QPainter* painter, bool isSelected, const QString& id);
    const QRectF CAPACITOR_BOUND_RECT = QRectF(-42, -35, 84, 50);
    void drawCapacitor(QPainter* painter, bool isSelected, const QString& id);
    const QRectF CURRENTSOURCE_BOUND_RECT = QRectF(-42, -45, 84, 60);
    void drawCurrentSource(QPainter* painter, bool isSelected, const QString& id);
    const QRectF VOLTAGESOURCE_BOUND_RECT = QRectF(-42, -45, 84, 60);
    void drawVoltageSource(QPainter* painter, bool isSelected, const QString& id);

    const QRectF PIN_BOUND_RECT = QRect(-2, -2, 4, 4);
    void drawPin(QPainter* painter, bool isSelected, bool isHovered);

}

#endif // COMPONENTPAINTER_H
