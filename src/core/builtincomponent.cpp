#include "builtincomponent.h"

#include "pin.h"

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
        QPointF newPos = value.toPointF();
        int gridSize = 20;

        qreal snappedX = qRound(newPos.x() / gridSize) * gridSize;
        qreal snappedY = qRound(newPos.y() / gridSize) * gridSize;

        return QPointF(snappedX, snappedY);
    }
    return QGraphicsObject::itemChange(change, value);
}
