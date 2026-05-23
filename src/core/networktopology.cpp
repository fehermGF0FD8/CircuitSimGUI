#include "networktopology.h"

#include "node.h"
#include "wire.h"

#include <QSet>
#include <QList>

/*
    Hálózat-összevonó és sziget-kereső algoritmus (Breadth-First Search).
    A fizikai csatlakozások bejárásának logikája és a Qt konténerekre
    (QSet, QList) való optimális adaptálása mesterséges intelligencia
    (Gemini) iránymutatása és kódjavaslata alapján készült.
*/

void NetworkTopology::updateNetworkNames(Node* startNode) {
    if (!startNode) return;

    QSet<Node*> visitedNodes;
    QList<Node*> queue;

    queue.append(startNode);
    visitedNodes.insert(startNode);

    QString smallestID = startNode->getID();

    while (!queue.isEmpty()) {
        Node* current = queue.takeFirst();

        if (current->getID() < smallestID) {
            smallestID = current->getID();
        }

        for (Wire* wire : current->getWires()) {
            Node* neighbor = nullptr;

            if (wire->getStartNode() != nullptr && wire->getStartNode() != current) {
                neighbor = wire->getStartNode();
            } else if (wire->getEndNode() != nullptr && wire->getEndNode() != current) {
                neighbor = wire->getEndNode();
            }

            if (neighbor && !visitedNodes.contains(neighbor)) {
                visitedNodes.insert(neighbor);
                queue.append(neighbor);
            }
        }
    }

    for (Node* node : visitedNodes) {
        node->setNetName(smallestID);
    }
}

