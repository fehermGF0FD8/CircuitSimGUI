#ifndef COMPONENT_H
#define COMPONENT_H

#include "componenttype.h"

#include <QGraphicsObject>
#include <QJsonObject>
#include <QList>
#include <QRectF>
#include <QString>
#include <QVariant>

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class Pin;

class Component : public QGraphicsObject {
    Q_OBJECT
public:
    explicit Component(const QString& idParam, ComponentType typeParam, QGraphicsItem* parent = nullptr);
    virtual ~Component() = default;

    QRectF boundingRect() const override = 0;
    virtual QRectF getObstacleRect() const = 0;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override = 0;

    virtual QJsonObject toJson() const = 0;

    void setID(const QString& compIDParam) { id = compIDParam; }
    virtual QString getID() const { return id; }
    ComponentType getType() const { return type; }

    void addPin(Pin* pinParam);
    const QList<Pin*>& getPins() const { return pins; }

protected:
    QString id;
    ComponentType type;
    QList<Pin*> pins;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif // COMPONENT_H
