#include "componentpainter.h"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPen>

void ComponentPainter::drawZigZagComponent(QPainter* painter, bool isSelected, const QString& id) {
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setCosmetic(true);
    if (isSelected) {
        pen.setColor(QColor(200, 220, 255));
    } else {
        pen.setColor(Qt::black);
    }

    painter->setPen(pen);

    painter->drawLine(-20,  0, -15, -8);
    painter->drawLine(-15, -8, -10,  8);
    painter->drawLine(-10,  8,  -5, -8);
    painter->drawLine( -5, -8,   0,  8);
    painter->drawLine(  0,  8,   5, -8);
    painter->drawLine(  5, -8,  10,  8);
    painter->drawLine( 10,  8,  15, -8);
    painter->drawLine( 15, -8,  20,  0);

    painter->drawLine(-40, 0, -20, 0);
    painter->drawLine(20, 0, 40, 0);

    QFont font;
    font.setPointSize(10);

    painter->setFont(font);
    painter->setPen(Qt::black);

    painter->drawText(QRectF(-20, -28, 40, 18), Qt::AlignCenter, id);
}

void ComponentPainter::drawCapacitor(QPainter* painter, bool isSelected, const QString& id) {
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setCosmetic(true);
    if (isSelected) {
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

    painter->drawText(QRectF(-20, -35, 40, 20), Qt::AlignCenter, id);
}

void ComponentPainter::drawCurrentSource(QPainter* painter, bool isSelected, const QString& id) {
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setCosmetic(true);
    if (isSelected) {
        pen.setColor(QColor(200, 220, 255));
    } else {
        pen.setColor(Qt::black);
    }

    painter->setPen(pen);

    painter->drawEllipse(-15, -15, 30, 30);

    painter->drawLine(-40, 0, -15, 0);
    painter->drawLine(15, 0, 40, 0);

    painter->drawLine(-10, 0, 10, 0);

    painter->drawLine(10, 0, 4, -4);
    painter->drawLine(10, 0, 4, 4);

    QFont font;
    font.setPointSize(10);

    painter->setFont(font);
    painter->setPen(Qt::black);

    painter->drawText(QRectF(-20, -38, 40, 18), Qt::AlignCenter, id);
}

void ComponentPainter::drawVoltageSource(QPainter* painter, bool isSelected, const QString& id) {
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setCosmetic(true);
    if (isSelected) {
        pen.setColor(QColor(200, 220, 255));
    } else {
        pen.setColor(Qt::black);
    }

    painter->setPen(pen);

    painter->drawEllipse(-15, -15, 30, 30);

    painter->drawLine(-40, 0, -15, 0);
    painter->drawLine(15, 0, 40, 0);

    painter->drawLine(-10, 0, -4, 0);
    painter->drawLine(-7, -3, -7, 3);

    painter->drawLine(7, -3, 7, 3);

    QFont font;
    font.setPointSize(10);

    painter->setFont(font);
    painter->setPen(Qt::black);

    painter->drawText(QRectF(-20, -38, 40, 18), Qt::AlignCenter, id);
}

void ComponentPainter::drawPin(QPainter* painter, bool isSelected, bool isHovered) {
    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCosmetic(true);

    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);

    if (isSelected) {
        painter->setBrush(QColor(200, 220, 255));
    }
    else if (isHovered) {
        painter->setBrush(Qt::yellow);
    }
    else {
        painter->setBrush(Qt::white);
    }

    painter->drawEllipse(ComponentPainter::PIN_BOUND_RECT);
}
