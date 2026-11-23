#include "builtincomponent.h"

BuiltInComponent::BuiltInComponent(const QString& idParam, ComponentType typeParam, QGraphicsItem* parent):
    Component(idParam, typeParam, parent) {}

QJsonObject BuiltInComponent::toJson() const {
    QJsonObject jsonComponent;

    jsonComponent["id"] = getID();
    jsonComponent["type"] = static_cast<int>(getType());

    jsonComponent["x"] = scenePos().x();
    jsonComponent["y"] = scenePos().y();
    jsonComponent["rotation"] = rotation();

    // QJsonObject parameters;
    // jsonComponent["parameters"] = parameters;

    return jsonComponent;
}
