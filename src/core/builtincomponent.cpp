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

// !!!
QJsonObject BuiltInComponent::toJson() const {
    QJsonObject jsonComponent;

    jsonComponent["id"] = getID();
    jsonComponent["type"] = static_cast<int>(getType());

    jsonComponent["x"] = scenePos().x();
    jsonComponent["y"] = scenePos().y();
    jsonComponent["rotation"] = rotation();

    // QJsonObject parameters;
    // jsonComponent["parameters"] = parameters;

    return jsonComponent;
}

QVariant BuiltInComponent::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        return Snapping::snapToGrid(value.toPointF());
    }
    if (change == ItemPositionHasChanged || change == ItemRotationHasChanged) {
        for (Pin* pin : std::as_const(pins)) {
            Wire* connectedWire = pin->getWire();
            if (connectedWire) {
                connectedWire->updatePosition();
            }
        }
    }
    return QGraphicsObject::itemChange(change, value);
}
