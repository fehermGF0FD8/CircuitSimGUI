#ifndef NETWORKTOPOLOGY_H
#define NETWORKTOPOLOGY_H

#include <QList>
#include <QString>

class BuiltInComponent;
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
    // QList<ComponentConnection> generateNetlist(const QList<BuiltInComponent*>& allComponents);
    // bool checkForShortCircuits(const QList<BuiltInComponent*>& allComponents);
    // QList<QString> findFloatingNetworks(const QList<Node*>& allNodes);
    // void applyGroundReference(BuiltInComponent* groundComponent);
};

#endif // NETWORKTOPOLOGY_H
