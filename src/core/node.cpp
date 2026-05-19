#include "node.h"

#include "componentpainter.h"
#include "pin.h"
// #include "wire.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

Node::Node(const QString& idParam, NodeType typeParam, QGraphicsItem* parent):
    QGraphicsObject(parent),
    id(idParam),
    type(typeParam),
    isHovered(false)
{
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
}

QRectF Node::boundingRect() const {
    return ComponentPainter::NODE_BOUND_RECT;
}

void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    ComponentPainter::drawNode(painter, getType(), isSelected(), isHovered, getID());
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        QPointF newPos = value.toPointF();
        int gridSize = 20;

        qreal snappedX = qRound(newPos.x() / gridSize) * gridSize;
        qreal snappedY = qRound(newPos.y() / gridSize) * gridSize;

        return QPointF(snappedX, snappedY);
    }
    return QGraphicsObject::itemChange(change, value);
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = true;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = false;
    update();
    QGraphicsObject::hoverLeaveEvent(event);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    emit nodeClicked(this);
    event->accept();
}
