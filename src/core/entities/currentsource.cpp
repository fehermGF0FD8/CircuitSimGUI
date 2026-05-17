#include "currentsource.h"

#include "componentpainter.h"
#include "componenttype.h"
#include "pin.h"

#include <QPainter>

CurrentSource::CurrentSource(const QString& idParam, QGraphicsItem* parent):
    BuiltInComponent(idParam, ComponentType::CurrentSource, parent) {
    addPin(new Pin(QPointF(-40, 0), this));
    addPin(new Pin(QPointF(40, 0), this));
}

QRectF CurrentSource::boundingRect() const {
    return ComponentPainter::CURRENTSOURCE_BOUND_RECT;
}

void CurrentSource::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    ComponentPainter::drawCurrentSource(painter, isSelected(), id);
}
