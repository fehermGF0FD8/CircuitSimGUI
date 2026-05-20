#include "pin.h"

#include "builtincomponent.h"

#include "componentpainter.h"
#include "node.h"
#include "wire.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

Pin::Pin(QPointF relativePosParam, BuiltInComponent* parentCompParam):
    QGraphicsObject(parentCompParam),
    parentComp(parentCompParam),
    isHovered(false)
{
    setPos(relativePosParam);
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable);
}

Pin::~Pin() {
    if (connectedWire != nullptr) {
        Wire* wireToDelete = connectedWire;
        connectedWire = nullptr;
        delete wireToDelete;
    }
}

QRectF Pin::boundingRect() const {
    return ComponentPainter::PIN_BOUND_RECT;
}

void Pin::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    ComponentPainter::drawPin(painter, isSelected(), isHovered);
}

void Pin::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = true;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void Pin::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = false;
    update();
    QGraphicsObject::hoverLeaveEvent(event);
}
