#include "builtincomponent.h"

#include "pin.h"
#include "snapping.h"
#include "wire.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QPointF>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QtMath>

BuiltInComponent::BuiltInComponent(const QString& idParam, ComponentType typeParam, QGraphicsItem* parent):
    QGraphicsObject(parent),
    id(idParam),
    type(typeParam)
{
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setTransformOriginPoint(0, 0);
}

void BuiltInComponent::addPin(Pin* pinParam) {
    if (pinParam) {
        pins.append(pinParam);
    }
}

QVariant BuiltInComponent::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        return Snapping::snapToGrid(value.toPointF());
    }
    if (change == ItemPositionHasChanged) {
        for (Pin* pin : pins) {
            Wire* connectedWire = pin->getWire();
            if (connectedWire) {
                connectedWire->updatePosition();
            }
        }
    }
    return QGraphicsObject::itemChange(change, value);
}
