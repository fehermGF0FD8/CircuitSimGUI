#ifndef NETWORKTOPOLOGY_H
#define NETWORKTOPOLOGY_H

#include <QList>
#include <QString>

class Component;
class Node;

namespace NetworkTopology
{
    /*
    struct ComponentConnection {
        QString componentID;
        QString componentType;
        QList<QString> netNames;
    };
    */

    void updateNetworkNames(Node* startNode);
    // QList<ComponentConnection> generateNetlist(const QList<Component*>& allComponents);
    // bool checkForShortCircuits(const QList<Component*>& allComponents);
    // QList<QString> findFloatingNetworks(const QList<Node*>& allNodes);
    // void applyGroundReference(Component* groundComponent);
};

#endif // NETWORKTOPOLOGY_H
