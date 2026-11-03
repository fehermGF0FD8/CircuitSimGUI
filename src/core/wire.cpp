#include "wire.h"

#include "builtincomponent.h"
#include "componentpainter.h"
#include "node.h"
#include "nodetype.h"
#include "pin.h"

#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QJsonArray>
#include <QPainter>
#include <QPainterPath>
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
    // using stroker in order to widen the collision shape of the wire
    QPainterPathStroker stroker;
    stroker.setWidth(6);
    return stroker.createStroke(getPath());
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

QPainterPath Wire::getPath() const {
    QPainterPath path;

    if (!pathPoints.isEmpty()) {
        path.moveTo(pathPoints.first());
        for (int i = 1; i < pathPoints.size(); ++i) {
            path.lineTo(pathPoints[i]);
        }
    }

    return path;
}

void Wire::setPathPoints(const QList<QPointF>& newPath) {
    prepareGeometryChange();
    pathPoints = newPath;

    if (!pathPoints.isEmpty()) {
        startPos = pathPoints.first();
        endPos = pathPoints.last();
    }
    update();
}

void Wire::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QColor baseColor = getBaseColor();
    ComponentPainter::drawWire(painter, getPath(), baseColor, isSelected(), isHovered);
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

// !!!
QJsonObject Wire::toJson() const {
    QJsonObject jsonWire;

    QJsonObject jsonStart;
    if (getStartNode()) {
        jsonStart["type"] = "Node";
        jsonStart["id"] = getStartNode()->getID();
    }
    else if (getStartPin()) {
        jsonStart["type"] = "Pin";
        jsonStart["componentId"] = getStartPin()->getParentComponent()->getID();
        jsonStart["pinIndex"] = getStartPin()->getPinIndex();
    }
    jsonWire["start"] = jsonStart;

    QJsonObject jsonEnd;
    if (getEndNode()) {
        jsonEnd["type"] = "Node";
        jsonEnd["id"] = getEndNode()->getID();
    }
    else if (getEndPin()) {
        jsonEnd["type"] = "Pin";
        jsonEnd["componentId"] = getEndPin()->getParentComponent()->getID();
        jsonEnd["pinIndex"] = getEndPin()->getPinIndex();
    }
    jsonWire["end"] = jsonEnd;

    QJsonArray pointsArray;
    for (const QPointF& pt : pathPoints) {
        QJsonObject pointObj;
        pointObj["x"] = pt.x();
        pointObj["y"] = pt.y();
        pointsArray.append(pointObj);
    }
    jsonWire["pathPoints"] = pointsArray;

    return jsonWire;
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
