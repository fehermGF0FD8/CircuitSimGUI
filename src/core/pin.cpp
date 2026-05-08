#include "pin.h"

#include "builtincomponent.h"

#include <QColor>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

Pin::Pin(QPointF relativePosParam, BuiltInComponent* parentCompParam):
    QGraphicsObject(parentCompParam),
    parentComp(parentCompParam),
    isHovered(false)
{
    setPos(relativePosParam);
    setAcceptHoverEvents(true);
}

QRectF Pin::boundingRect() const {
    return QRect(-2, -2, 4, 4);
}

void Pin::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCosmetic(true);

    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);

    if (isSelected()) {
        painter->setBrush(QColor(200, 220, 255));
    }
    else if (isHovered) {
        painter->setBrush(Qt::yellow);
    }
    else {
        painter->setBrush(Qt::white);
    }

    painter->drawEllipse(boundingRect());
}

void Pin::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = true;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void Pin::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = false;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void Pin::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    emit pinClicked(this);
    event->accept();
}
