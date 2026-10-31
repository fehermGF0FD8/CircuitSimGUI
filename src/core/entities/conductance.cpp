#include "conductance.h"

#include "componentpainter.h"
#include "componenttype.h"
#include "pin.h"

#include <QPainter>

Conductance::Conductance(const QString& idParam, QGraphicsItem* parent):
    BuiltInComponent(idParam, ComponentType::Conductance, parent) {
    addPin(new Pin(QPointF(-40, 0), this));
    addPin(new Pin(QPointF(40, 0), this));
}

QRectF Conductance::boundingRect() const {
    return ComponentPainter::ZIGZAG_BOUND_RECT;
}

QRectF Conductance::getObstacleRect() const {
    return QRectF(-25, -12, 50, 24);
}

void Conductance::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    ComponentPainter::drawZigZagComponent(painter, isSelected(), id);
}
