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
    // Notify itemChange() on position changes to update connected pins/wires
    setFlag(ItemSendsGeometryChanges);
    // Set rotation origin to the top-left
    setTransformOriginPoint(0, 0);
}

void BuiltInComponent::addPin(Pin* pinParam) {
    if (pinParam) {
        pins.append(pinParam);
    }
}

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
    return QGraphicsObject::itemChange(change, value);
}
