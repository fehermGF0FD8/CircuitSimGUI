#ifndef CONDUCTANCE_H
#define CONDUCTANCE_H

#include "builtincomponent.h"

#include <QRectF>
#include <QString>
#include <QVariant>

class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class Conductance: public BuiltInComponent
{
    Q_OBJECT

public:
    explicit Conductance(const QString& idParam,
                         QGraphicsItem* parent = nullptr);
    ~Conductance() override = default;
    QRectF boundingRect() const override;
    QRectF getObstacleRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem*,
               QWidget*) override;
};

#endif // CONDUCTANCE_H
