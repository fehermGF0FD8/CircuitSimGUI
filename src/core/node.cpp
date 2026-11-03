#include "node.h"

#include "componentpainter.h"
#include "pin.h"
#include "snapping.h"
#include "wire.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <utility>

Node::Node(const QString& idParam, NodeType typeParam, QGraphicsItem* parent):
    QGraphicsObject(parent),
    id(idParam),
    netName(idParam),
    type(typeParam),
    isHovered(false)
{
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
}

Node::~Node() {
    while(!connectedWires.isEmpty()) {
        Wire* wireToDelete = connectedWires.takeFirst();
        delete wireToDelete;
    }
}

QRectF Node::boundingRect() const {
    return ComponentPainter::NODE_BOUND_RECT;
}

void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    ComponentPainter::drawNode(painter, getType(), isSelected(), isHovered, getNetName());
}

// !!!
QJsonObject Node::toJson() const {
    QJsonObject jsonNode;

    jsonNode["id"] = getID();
    jsonNode["netName"] = getNetName();
    jsonNode["type"] = static_cast<int>(getType());

    jsonNode["x"] = scenePos().x();
    jsonNode["y"] = scenePos().y();

    return jsonNode;
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        return Snapping::snapToGrid(value.toPointF());
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
