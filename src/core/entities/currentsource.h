#ifndef CURRENTSOURCE_H
#define CURRENTSOURCE_H

#include "builtincomponent.h"

#include <QRectF>
#include <QString>
#include <QVariant>

class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class CurrentSource: public BuiltInComponent
{
    Q_OBJECT

public:
    explicit CurrentSource(const QString& idParam,
                           QGraphicsItem* parent = nullptr);
    ~CurrentSource() override = default;
    QRectF boundingRect() const override;
    QRectF getObstacleRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem*,
               QWidget*) override;
};

#endif // CURRENTSOURCE_H
