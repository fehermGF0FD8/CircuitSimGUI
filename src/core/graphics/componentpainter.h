#ifndef COMPONENTPAINTER_H
#define COMPONENTPAINTER_H

#include "../nodetype.h"

#include <QColor>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>
#include <QString>

class Wire;

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
    //...

    const QRectF PIN_BOUND_RECT = QRectF(-2, -2, 4, 4);
    void drawPin(QPainter* painter, bool isSelected, bool isHovered);

    const QRectF NODE_BOUND_RECT = QRectF(-5, -15, 40, 20);
    void drawNode(QPainter* painter, NodeType type, bool isSelected,
                  bool isHovered, const QString& netName);

    QRectF calculateWireBoundingRect(const Wire* wire);
    void drawWire(QPainter* painter, const QPainterPath& path,
                  const QColor& baseColor, bool isSelected, bool isHovered);
}

#endif // COMPONENTPAINTER_H
