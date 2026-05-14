#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "builtincomponent.h"

#include <QRectF>
#include <QString>
#include <QVariant>

class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class Capacitor: public BuiltInComponent
{
    Q_OBJECT

public:
    explicit Capacitor(const QString& idParam,
                      const QString& typeParam,
                      QGraphicsItem* parent = nullptr);
    ~Capacitor() override = default;
    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem*,
               QWidget*) override;
};

#endif // CAPACITOR_H
