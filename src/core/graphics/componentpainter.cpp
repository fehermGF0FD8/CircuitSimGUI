#include "componentpainter.h"

#include "wire.h"

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
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setCosmetic(true);

    painter->setPen(pen);

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

void ComponentPainter::drawNode(QPainter* painter, NodeType type, bool isSelected,
                                bool isHovered, const QString& id) {
    painter->setRenderHint(QPainter::Antialiasing);

    QColor nodeColor;
    switch(type) {
        case NodeType::N:
            nodeColor = Qt::red;
            break;
        case NodeType::B:
            nodeColor = Qt::blue;
            break;
        //...
        default:
            nodeColor = Qt::black;
            break;
    }

    if (isSelected) {
        nodeColor = QColor(200, 220, 255);
    }
    else if (isHovered) {
        nodeColor = nodeColor.lighter(150);
    }

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(nodeColor));

    painter->drawEllipse(QRectF(-3, -3, 6, 6));

    QFont font;
    font.setPointSize(10);

    painter->setFont(font);
    painter->setPen(nodeColor);

    painter->drawText(QPointF(5, -5), id);
}

QRectF ComponentPainter::calculateWireBoundingRect(const Wire* wire) {
    if (!wire) return QRectF();
    // normalized(): normalized rect based on start and end points
    // adjusted(...): non-zero area even for perfectly horizontal or vertical wires
    return QRectF(wire->getStartPos(), wire->getEndPos()).normalized().adjusted(-4, -4, 4, 4);
}

void ComponentPainter::drawWire(QPainter* painter, const QPointF& start, const QPointF& end,
                                const QColor& baseColor, bool isSelected, bool isHovered) {
    painter->setRenderHint(QPainter::Antialiasing);

    QColor wireColor = baseColor;
    int wireThickness = 2;

    if (isSelected) {
        wireColor = QColor(200, 220, 255);
        wireThickness = 3;
    }
    else if (isHovered) {
        wireColor = wireColor.lighter(150);
        wireThickness = 3;
    }

    QPen pen(wireColor);
    pen.setWidth(wireThickness);
    pen.setCapStyle(Qt::RoundCap);
    pen.setCosmetic(true);

    painter->setPen(pen);
    painter->drawLine(start, end);
}

