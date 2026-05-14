#include "resistor.h"

#include "pin.h"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPen>

Resistor::Resistor(const QString& idParam, const QString& typeParam, QGraphicsItem* parent):
    BuiltInComponent(idParam, typeParam, parent) {
    addPin(new Pin(QPointF(-40, 0), this));
    addPin(new Pin(QPointF(40, 0), this));
}

QRectF Resistor::boundingRect() const {
    return QRectF(-42, -30, 84, 45);
}

void Resistor::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
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

    painter->drawRect(-20, -5, 40, 10);

    painter->drawLine(-40, 0, -20, 0);
    painter->drawLine(20, 0, 40, 0);

    QFont font;
    font.setPointSize(10);

    painter->setFont(font);
    painter->setPen(Qt::black);

    QRectF idBox(-20, -25, 40, 15);

    painter->drawText(idBox, Qt::AlignCenter, id);
}
