#ifndef VOLTAGESOURCE_H
#define VOLTAGESOURCE_H

#include "builtincomponent.h"

#include <QRectF>
#include <QString>
#include <QVariant>

class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class VoltageSource: public BuiltInComponent
{
    Q_OBJECT

public:
    explicit VoltageSource(const QString& idParam,
                           QGraphicsItem* parent = nullptr);
    ~VoltageSource() override = default;
    QRectF boundingRect() const override;
    QRectF getObstacleRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem*,
               QWidget*) override;
};

#endif // VOLTAGESOURCE_H
