#include "resistor.h"

#include "componentpainter.h"
#include "componenttype.h"
#include "pin.h"

#include <QPainter>

Resistor::Resistor(const QString& idParam, QGraphicsItem* parent):
    BuiltInComponent(idParam, ComponentType::Resistor, parent) {
    addPin(new Pin(QPointF(-40, 0), this));
    addPin(new Pin(QPointF(40, 0), this));
}

QRectF Resistor::boundingRect() const {
    return ComponentPainter::ZIGZAG_BOUND_RECT;
}

void Resistor::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    ComponentPainter::drawZigZagComponent(painter, isSelected(), id);
}
