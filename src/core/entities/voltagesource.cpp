#include "voltagesource.h"

#include "componentpainter.h"
#include "componenttype.h"
#include "pin.h"

#include <QPainter>

VoltageSource::VoltageSource(const QString& idParam, QGraphicsItem* parent):
    BuiltInComponent(idParam, ComponentType::VoltageSource, parent) {
    addPin(new Pin(QPointF(-40, 0), this));
    addPin(new Pin(QPointF(40, 0), this));
}

QRectF VoltageSource::boundingRect() const {
    return ComponentPainter::VOLTAGESOURCE_BOUND_RECT;
}

void VoltageSource::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    ComponentPainter::drawVoltageSource(painter, isSelected(), id);
}
