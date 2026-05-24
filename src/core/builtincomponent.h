#ifndef BUILTINCOMPONENT_H
#define BUILTINCOMPONENT_H

#include "componenttype.h"

#include <QGraphicsObject>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QRectF>
#include <QString>
#include <QVariant>

class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class Pin;

class BuiltInComponent: public QGraphicsObject
{
    Q_OBJECT

public:
    explicit BuiltInComponent(const QString& idParam,
                              ComponentType typeParam,
                              QGraphicsItem* parent = nullptr);
    virtual ~BuiltInComponent() = default;

    QRectF boundingRect() const override = 0;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem*,
               QWidget*) override = 0;

    void setID(const QString& compIDParam) {id = compIDParam;}
    QString getID() const {return id;}

    ComponentType getType() const {return type;}

    void addPin(Pin* pinParam);
    const QList<Pin*>& getPins() const {return pins;}

    // void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    // !!!
    QJsonObject toJson() const;
protected:
    QString id;
    ComponentType type;
    // QMap<QString, QVariant> parameters;
    QList<Pin*> pins;

    // virtual void showParameterDialog();

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif // BUILTINCOMPONENT_H
