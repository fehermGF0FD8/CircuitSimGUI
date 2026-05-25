#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "idmanager.h"

#include <QString>
#include <QList>
#include <QJsonObject>

class BuiltInComponent;
class Node;
class Wire;

class Circuit
{
public:
    explicit Circuit(const QString& name);
    virtual ~Circuit();

    QString getName() const { return circuitName; }
    void setName(const QString& name) { circuitName = name; }

    void addComponent(BuiltInComponent* comp);
    void removeComponent(BuiltInComponent* comp);
    const QList<BuiltInComponent*>& getComponents() const {return components;}
    void addNode(Node* node);
    void removeNode(Node* node);
    const QList<Node*>& getNodes() const {return nodes;}
    void addWire(Wire* wire);
    void removeWire(Wire* wire);
    const QList<Wire*>& getWires() const {return wires;}

    IDmanager& getIdManager() { return idManager; }

    QJsonObject toJson() const;

    void fromJson(const QJsonObject& json);

private:
    QString circuitName;
    IDmanager idManager;

    QList<BuiltInComponent*> components;
    QList<Node*> nodes;
    QList<Wire*> wires;
};

#endif // CIRCUIT_H
