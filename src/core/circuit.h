#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "idmanager.h"
#include "nodetype.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QJsonObject>

class Component;
class Node;
class Wire;

class Circuit: public QObject
{
    Q_OBJECT
public:
    explicit Circuit(const QString& name, QObject* parent = nullptr);
    ~Circuit() override = default;

    bool isSubcircuit() const;
    bool dependsOn(const Circuit* rCircuitP) const;

    QString getName() const { return circuitName; }
    void setName(const QString& name) { circuitName = name; }

    void addComponent(Component* comp);
    void removeComponent(Component* comp);
    const QList<Component*>& getComponents() const {return components;}

    void addNode(Node* node);
    void removeNode(Node* node);
    const QList<Node*>& getNodes() const {return nodes;}

    void addWire(Wire* wire);
    void removeWire(Wire* wire);
    const QList<Wire*>& getWires() const {return wires;}

    IDmanager& getIdManager() { return idManager; }

    void notifyExtNodesChanged() { emit extNodesChanged(); }

    QJsonObject toJson() const;
    void loadNodesFromJson(const QJsonObject& json);
    void loadCompsAndWiresFromJson(const QJsonObject& json,
                                   const QMap<QString, Circuit*>& circuitReg);

signals:
    void extNodesChanged();

private:
    static bool isExternalNodeType(NodeType nType);

    QString circuitName;
    IDmanager idManager;

    QList<Component*> components;
    QList<Node*> nodes;
    QList<Wire*> wires;
};

#endif // CIRCUIT_H
