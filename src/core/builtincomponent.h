#ifndef BUILTINCOMPONENT_H
#define BUILTINCOMPONENT_H

#include "component.h"

// #include <QMap>

class BuiltInComponent: public Component
{
    Q_OBJECT

public:
    explicit BuiltInComponent(const QString& idParam,
                              ComponentType typeParam,
                              QGraphicsItem* parent = nullptr);
    virtual ~BuiltInComponent() = default;

    // void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    QJsonObject toJson() const;
protected:
    // QMap<QString, QVariant> parameters;

    // virtual void showParameterDialog();
};

#endif // BUILTINCOMPONENT_H
