#include "component.h"
#include "snapping.h"

#include <QGraphicsScene>

Component::Component(const QString& idParam, ComponentType typeParam, QGraphicsItem* parent)
    : QGraphicsObject(parent),
    id(idParam),
    type(typeParam)
{
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    // Notify itemChange() on position changes to update connected pins/wires
    setFlag(ItemSendsGeometryChanges);
    // Set rotation origin to the top-left
    setTransformOriginPoint(0, 0);
}

void Component::addPin(Pin* pinParam) {
    if (pinParam) {
        pins.append(pinParam);
    }
}

QVariant Component::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        return Snapping::snapToGrid(value.toPointF());
    }
    return QGraphicsObject::itemChange(change, value);
}
