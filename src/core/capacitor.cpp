#include "capacitor.h"

#include "componenttype.h"
#include "pin.h"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPen>

Capacitor::Capacitor(const QString& idParam, QGraphicsItem* parent):
    BuiltInComponent(idParam, ComponentType::Capacitor, parent) {
    addPin(new Pin(QPointF(-40, 0), this));
    addPin(new Pin(QPointF(40, 0), this));
}

QRectF Capacitor::boundingRect() const {
    return QRectF(-42, -30, 84, 45);
}

void Capacitor::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setCosmetic(true);
    if (isSelected()) {
        pen.setColor(QColor(200, 220, 255));
    } else {
        pen.setColor(Qt::black);
    }

    painter->setPen(pen);

    painter->drawLine(-5, -12, -5, 12);
    painter->drawLine(5, -12, 5, 12);

    painter->drawLine(-40, 0, -5, 0);
    painter->drawLine(5, 0, 40, 0);

    QFont font;
    font.setPointSize(10);

    painter->setFont(font);
    painter->setPen(Qt::black);

    QRectF idBox(-20, -30, 40, 15);

    painter->drawText(idBox, Qt::AlignCenter, id);
}

