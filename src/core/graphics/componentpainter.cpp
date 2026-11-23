#include "componentpainter.h"

#include "snapping.h"
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

QRectF ComponentPainter::calculateSubcircuitBoundRect(qreal boxWidthP, qreal boxHeightP, qreal leadLengthP) {
    return QRectF(-5.0, -22.0, boxWidthP + 2 * leadLengthP + 10.0, boxHeightP + 27.0);
}

void ComponentPainter::drawSubcircuit(QPainter* painter, bool isSelected,
                                      const QString& id,
                                      qreal boxWidth, qreal boxHeight, qreal leadLength,
                                      const QStringList& leftLabels, const QStringList& rightLabels) {
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setCosmetic(true);
    if (isSelected) {
        pen.setColor(QColor(200, 220, 255));
    }
    else {
        pen.setColor(Qt::black);
    }
    painter->setPen(pen);

    QRectF box(leadLength, 0.0, boxWidth, boxHeight);
    painter->drawRect(box);

    // leads on the left
    const qreal gridStep = Snapping::GRID_SIZE;
    for (int i = 0; i < leftLabels.size(); ++i) {
        painter->drawLine(QPointF(0.0, (i + 1) * gridStep),
                          QPointF(leadLength, (i + 1) * gridStep));
    }

    // leads on the right
    for (int i = 0; i < rightLabels.size(); ++i) {
        painter->drawLine(QPointF(leadLength + boxWidth, (i + 1) * gridStep),
                          QPointF(boxWidth + 2 * leadLength, (i + 1) * gridStep));
    }

    // pin labels (X_1, Y_2, etc.)
    QFont pFont;
    pFont.setPointSize(8);
    painter->setFont(pFont);
    painter->setPen(Qt::black);

    const qreal halfWidth = boxWidth / 2.0;
    for (int i = 0; i < leftLabels.size(); ++i) {
        QRectF labelRect(leadLength + 4.0, ((i + 1) * gridStep) - 8.0, halfWidth - 6.0, 16.0);
        painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, leftLabels[i]);
    }
    for (int i = 0; i < rightLabels.size(); ++i) {
        QRectF labelRect(leadLength + halfWidth + 2.0, ((i + 1) * gridStep) - 8.0, halfWidth - 6.0, 16.0);
        painter->drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, rightLabels[i]);
    }

    // header (subc name&id)
    QFont hFont;
    hFont.setPointSize(10);
    painter->setFont(hFont);
    painter->setPen(Qt::black);

    painter->drawText(QRectF(0.0, -20.0, boxWidth + 2 * leadLength, 18.0),
                      Qt::AlignCenter | Qt::TextDontClip, id);
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
                                bool isHovered, const QString& netName) {
    painter->setRenderHint(QPainter::Antialiasing);

    QColor nodeColor = getNodeColor(type);

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

    painter->drawText(QPointF(5, -5), netName);
}

QColor ComponentPainter::getNodeColor(NodeType nTypeP) {
    switch (nTypeP) {
        case NodeType::N: return Qt::red;
        case NodeType::B: return Qt::blue;
        case NodeType::X: return Qt::darkGreen;
        case NodeType::Y: return QColor(230, 130, 0);
        case NodeType::A: return Qt::darkMagenta;
        case NodeType::O: return Qt::darkCyan;
        default:          return Qt::black;
    }
}

QRectF ComponentPainter::calculateWireBoundingRect(const Wire* wire) {
    if (!wire) return QRectF();

    return wire->shape().boundingRect();
}

void ComponentPainter::drawWire(QPainter* painter, const QPainterPath& path,
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
    painter->drawPath(path);
}

