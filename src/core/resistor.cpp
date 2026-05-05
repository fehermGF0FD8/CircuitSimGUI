#include "resistor.h"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QtMath>

Resistor::Resistor(const QString& idParam, const QString& typeParam, QGraphicsItem* parent):
    BuiltInComponent(idParam, typeParam, parent) {}

QRectF Resistor::boundingRect() const {
    return QRectF(-20, -15, 40, 30);
}

void Resistor::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setCosmetic(true);

    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (isSelected()) {
        painter->setBrush(QColor(200, 220, 255));
    } else {
        painter->setBrush(Qt::white);
    }

    QFont font;
    font.setPointSize(10);

    painter->setFont(font);

    painter->drawRect(-15, -5, 30, 10);

    painter->drawLine(-20, 0, -15, 0);
    painter->drawLine(15, 0, 20, 0);

    painter->drawText(-15, -6, id);
}
