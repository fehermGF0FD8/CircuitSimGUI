#include "capacitor.h"

#include "componentpainter.h"
#include "componenttype.h"
#include "pin.h"

#include <QPainter>

Capacitor::Capacitor(const QString& idParam, QGraphicsItem* parent):
    BuiltInComponent(idParam, ComponentType::Capacitor, parent) {
    addPin(new Pin(QPointF(-40, 0), this));
    addPin(new Pin(QPointF(40, 0), this));
}

QRectF Capacitor::boundingRect() const {
    return ComponentPainter::CAPACITOR_BOUND_RECT;
}

QRectF Capacitor::getObstacleRect() const {
    return QRectF(-10, -15, 20, 30);
}

void Capacitor::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    ComponentPainter::drawCapacitor(painter, isSelected(), id);
}
