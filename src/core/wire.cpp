#include "wire.h"

#include "componentpainter.h"
#include "node.h"
#include "nodetype.h"
#include "pin.h"

#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

Wire::Wire(QGraphicsItem* parent):
    QGraphicsObject(parent),
    startNode(nullptr),
    startPin(nullptr),
    endNode(nullptr),
    endPin(nullptr),
    isHovered(false)
{
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);

    startPos = QPointF(0, 0);
    endPos = QPointF(0, 0);
}

Wire::~Wire() {
    if (startNode != nullptr) {
        startNode->removeWire(this);
    } else if (startPin != nullptr) {
        startPin->removeWire();
    }

    if (endNode != nullptr) {
        endNode->removeWire(this);
    } else if (endPin != nullptr) {
        endPin->removeWire();
    }
}

QRectF Wire::boundingRect() const {
    return ComponentPainter::calculateWireBoundingRect(this);
}

QPainterPath Wire::shape() const {
    QPainterPath path;
    path.moveTo(startPos);
    path.lineTo(endPos);

    // using stroker in order to widen the collision shape of the wire
    QPainterPathStroker stroker;
    stroker.setWidth(6);
    return stroker.createStroke(path);
}

QColor Wire::getBaseColor() const {
    if ((startNode && startNode->getType() == NodeType::N) ||
        (endNode && endNode->getType() == NodeType::N)) {
        return Qt::red;
    }
    else if ((startNode && startNode->getType() == NodeType::B) ||
             (endNode && endNode->getType() == NodeType::B)) {
        return Qt::blue;
    }
    return Qt::black;
}

void Wire::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QColor baseColor = getBaseColor();
    ComponentPainter::drawWire(painter, startPos, endPos, baseColor, isSelected(), isHovered);
}

void Wire::setStartPos(const QPointF& startPosParam) {
    prepareGeometryChange();
    startPos = startPosParam;
    update();
}

void Wire::setEndPos(const QPointF& endPosParam) {
    prepareGeometryChange();
    endPos = endPosParam;
    update();
}

void Wire::setStartItem(Node* startNodeParam) {
    startNode = startNodeParam;
    startPin = nullptr;
}

void Wire::setStartItem(Pin* startPinParam) {
    startPin = startPinParam;
    startNode = nullptr;
}

void Wire::setEndItem(Node* endNodeParam) {
    endNode = endNodeParam;
    endPin = nullptr;
}

void Wire::setEndItem(Pin* endPinParam) {
    endPin = endPinParam;
    endNode = nullptr;
}

void Wire::updatePosition() {
    if (startNode) {
        setStartPos(startNode->scenePos());
    } else if (startPin) {
        setStartPos(startPin->scenePos());
    }

    if (endNode) {
        setEndPos(endNode->scenePos());
    } else if (endPin) {
        setEndPos(endPin->scenePos());
    }
}

void Wire::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = true;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void Wire::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = false;
    update();
    QGraphicsObject::hoverLeaveEvent(event);
}

void Wire::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    emit wireClicked(this);
    event->accept();
}
