#ifndef RESISTOR_H
#define RESISTOR_H

#include "builtincomponent.h"

#include <QRectF>
#include <QString>
#include <QVariant>

class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class Resistor: public BuiltInComponent
{
    Q_OBJECT

public:
    explicit Resistor(const QString& idParam,
                      QGraphicsItem* parent = nullptr);
    ~Resistor() override = default;
    QRectF boundingRect() const override;
    QRectF getObstacleRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem*,
               QWidget*) override;
};

#endif // RESISTOR_H
