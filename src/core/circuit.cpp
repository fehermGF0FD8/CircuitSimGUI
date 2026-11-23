#include "circuit.h"

#include "entities.h"

#include "component.h"
#include "componenttype.h"
#include "node.h"
#include "nodetype.h"
#include "pin.h"
#include "wire.h"

#include <QJsonArray>
#include <QMap>
#include <QPointF>

Circuit::Circuit(const QString& name, QObject* parent):
    QObject(parent),
    circuitName(name)
{}

bool Circuit::isExternalNodeType(NodeType nType) {
    return (nType == NodeType::X ||
            nType == NodeType::Y ||
            nType == NodeType::A ||
            nType == NodeType::O);
}

bool Circuit::isSubcircuit() const {
    for (Node* node : std::as_const(nodes)) {
        if (!node) continue;
        NodeType t = node->getType();
        if (isExternalNodeType(t)) {
            return true;
        }
    }
    return false;
}

/*
 * Prevents circular subcircuit references to avoid infinite recursion &
 * Block tab closure if the circuit is still referenced by other open circuits
 */
bool Circuit::dependsOn(const Circuit* rCircuitP) const {
    if (!rCircuitP) return false;

    if (this == rCircuitP) return true;

    for (Component* comp : std::as_const(components)) {
        if (auto* subComp = dynamic_cast<SubcircuitComponent*>(comp)) {
            Circuit* ref = subComp->getReferencedCircuit();
            if (ref && ref->dependsOn(rCircuitP)) {
                return true;
            }
        }
    }
    return false;
}

void Circuit::addComponent(Component* comp) {
    if (comp != nullptr && !components.contains(comp)) {
        components.append(comp);
    }
}

void Circuit::removeComponent(Component* comp) {
    if (comp != nullptr) {
        components.removeOne(comp);
    }
}

void Circuit::addNode(Node* node) {
    if (node != nullptr && !nodes.contains(node)) {
        nodes.append(node);
        if (isExternalNodeType(node->getType())) {
            emit extNodesChanged();
        }
    }
}

void Circuit::removeNode(Node* node) {
    if (node != nullptr) {
        nodes.removeOne(node);
        if (isExternalNodeType(node->getType())) {
            emit extNodesChanged();
        }
    }
}

void Circuit::addWire(Wire* wire) {
    if (wire != nullptr && !wires.contains(wire)) {
        wires.append(wire);
    }
}

void Circuit::removeWire(Wire* wire) {
    if (wire != nullptr) {
        wires.removeOne(wire);
    }
}

QJsonObject Circuit::toJson() const {
    QJsonObject jsonCircuit;

    jsonCircuit["circuitName"] = getName();

    QJsonArray componentsArray;
    for (Component* comp : getComponents()) {
        if (comp) {
            componentsArray.append(comp->toJson());
        }
    }
    jsonCircuit["components"] = componentsArray;

    QJsonArray nodesArray;
    for (Node* node : getNodes()) {
        if (node) {
            nodesArray.append(node->toJson());
        }
    }
    jsonCircuit["nodes"] = nodesArray;

    QJsonArray wiresArray;
    for (Wire* wire : getWires()) {
        if (wire) {
            wiresArray.append(wire->toJson());
        }
    }
    jsonCircuit["wires"] = wiresArray;

    return jsonCircuit;
}

/*
void Circuit::fromJson(const QJsonObject& json) {
    setName(json["circuitName"].toString());

    QMap<QString, Node*> nodeMap;
    QMap<QString, Component*> compMap;

    QJsonArray compsArray = json["components"].toArray();
    for (const QJsonValue& val : std::as_const(compsArray)) {
        QJsonObject compObj = val.toObject();

        QString id = compObj["id"].toString();
        ComponentType type = static_cast<ComponentType>(compObj["type"].toInt());

        QPointF scenePos = QPointF(compObj["x"].toDouble(), compObj["y"].toDouble());
        double rotationAngle = compObj["rotation"].toDouble();

        Component* newComponent = nullptr;

        switch (type) {
        case ComponentType::Resistor:
            newComponent = new Resistor(id);
            break;
        case ComponentType::Conductance:
            newComponent = new Conductance(id);
            break;
        case ComponentType::Capacitor:
            newComponent = new Capacitor(id);
            break;
        case ComponentType::CurrentSource:
            newComponent = new CurrentSource(id);
            break;
        case ComponentType::VoltageSource:
            newComponent = new VoltageSource(id);
            break;
        //...
        default:
            break;
        }

        if (newComponent) {
            newComponent->setPos(scenePos);
            newComponent->setRotation(rotationAngle);

            this->addComponent(newComponent);
            compMap[id] = newComponent;
        }
    }

    QJsonArray nodesArray = json["nodes"].toArray();
    for (const QJsonValue& val : std::as_const(nodesArray)) {
        QJsonObject nodeObj = val.toObject();

        QString id = nodeObj["id"].toString();
        QString netName = nodeObj["netName"].toString();
        NodeType type = static_cast<NodeType>(nodeObj["type"].toInt());

        QPointF scenePos = QPointF(nodeObj["x"].toDouble(), nodeObj["y"].toDouble());

        Node* newNode = new Node(id, type);

        newNode->setNetName(netName);
        newNode->setPos(scenePos);

        this->addNode(newNode);
        nodeMap[id] = newNode;
    }

    QJsonArray wiresArray = json["wires"].toArray();
    for (const QJsonValue& val : std::as_const(wiresArray)) {
        QJsonObject wireObj = val.toObject();

        Wire* newWire = new Wire();

        QJsonObject startObj = wireObj["start"].toObject();
        if (startObj["type"].toString() == "Node") {
            Node* startNode = nodeMap[startObj["id"].toString()];
            newWire->setStartItem(startNode);
            newWire->setStartPos(startNode->scenePos());
            startNode->addWire(newWire);

        }
        else if (startObj["type"].toString() == "Pin") {
            Component* comp = compMap[startObj["componentId"].toString()];
            Pin* startPin = comp->getPins()[startObj["pinIndex"].toInt()];
            newWire->setStartItem(startPin);
            newWire->setStartPos(startPin->scenePos());
            startPin->setWire(newWire);
        }

        QJsonObject endObj = wireObj["end"].toObject();
        if (endObj["type"].toString() == "Node") {
            Node* endNode = nodeMap[endObj["id"].toString()];
            newWire->setEndItem(endNode);
            newWire->setEndPos(endNode->scenePos());
            endNode->addWire(newWire);

        }
        else if (endObj["type"].toString() == "Pin") {
            Component* comp = compMap[endObj["componentId"].toString()];
            Pin* endPin = comp->getPins()[endObj["pinIndex"].toInt()];
            newWire->setEndItem(endPin);
            newWire->setEndPos(endPin->scenePos());
            endPin->setWire(newWire);
        }

        QList<QPointF> loadedPath;
        QJsonArray pointsArray = wireObj["pathPoints"].toArray();
        for (const QJsonValue& ptVal : std::as_const(pointsArray)) {
            QJsonObject ptObj = ptVal.toObject();
            loadedPath.append(QPointF(ptObj["x"].toDouble(), ptObj["y"].toDouble()));
        }
        newWire->setPathPoints(loadedPath);

        addWire(newWire);
    }
}
*/

void Circuit::loadNodesFromJson(const QJsonObject& json) {
    setName(json["circuitName"].toString());

    const QJsonArray nodesArray = json["nodes"].toArray();
    for (const QJsonValue& val : nodesArray) {
        QJsonObject nodeObj = val.toObject();

        QString id = nodeObj["id"].toString();
        QString netName = nodeObj["netName"].toString();
        NodeType type = static_cast<NodeType>(nodeObj["type"].toInt());
        QPointF scenePos(nodeObj["x"].toDouble(), nodeObj["y"].toDouble());

        Node* newNode = new Node(id, type);
        newNode->setNetName(netName);
        newNode->setPos(scenePos);

        this->addNode(newNode);
        idManager.registerID(id);
    }
}

void Circuit::loadCompsAndWiresFromJson(const QJsonObject& json,
                                        const QMap<QString, Circuit*>& circuitReg) {
    QMap<QString, Node*> nodeMap;
    for (Node* n : std::as_const(nodes)) {
        if (n) nodeMap[n->getID()] = n;
    }

    QMap<QString, Component*> compMap;
    const QJsonArray compsArray = json["components"].toArray();
    for (const QJsonValue& val : compsArray) {
        QJsonObject compObj = val.toObject();

        QString id = compObj["id"].toString();
        ComponentType type = static_cast<ComponentType>(compObj["type"].toInt());
        QPointF scenePos(compObj["x"].toDouble(), compObj["y"].toDouble());
        double rotationAngle = compObj["rotation"].toDouble();

        Component* newComponent = nullptr;
        switch (type) {
            case ComponentType::Resistor:      newComponent = new Resistor(id); break;
            case ComponentType::Conductance:   newComponent = new Conductance(id); break;
            case ComponentType::Capacitor:     newComponent = new Capacitor(id); break;
            case ComponentType::CurrentSource: newComponent = new CurrentSource(id); break;
            case ComponentType::VoltageSource: newComponent = new VoltageSource(id); break;
            case ComponentType::Subcircuit: {
                QString subName = compObj["subcircuitName"].toString();
                if (Circuit* refCircuit = circuitReg.value(subName, nullptr)) {
                    newComponent = new SubcircuitComponent(id, refCircuit);
                }
                break;
            }
            default: break;
        }

        if (newComponent) {
            newComponent->setPos(scenePos);
            newComponent->setRotation(rotationAngle);
            this->addComponent(newComponent);
            compMap[id] = newComponent;
            idManager.registerID(id);
        }
    }

    const QJsonArray wiresArray = json["wires"].toArray();
    for (const QJsonValue& val : wiresArray) {
        QJsonObject wireObj = val.toObject();
        Wire* newWire = new Wire();

        QJsonObject startObj = wireObj["start"].toObject();
        if (startObj["type"].toString() == "Node") {
            if (Node* startNode = nodeMap.value(startObj["id"].toString(), nullptr)) {
                newWire->setStartItem(startNode);
                newWire->setStartPos(startNode->scenePos());
                startNode->addWire(newWire);
            }
        } else if (startObj["type"].toString() == "Pin") {
            Component* comp = compMap.value(startObj["componentId"].toString(), nullptr);
            int pinIdx = startObj["pinIndex"].toInt();
            if (comp && pinIdx >= 0 && pinIdx < comp->getPins().size()) {
                Pin* startPin = comp->getPins()[pinIdx];
                newWire->setStartItem(startPin);
                newWire->setStartPos(startPin->scenePos());
                startPin->setWire(newWire);
            }
        }

        QJsonObject endObj = wireObj["end"].toObject();
        if (endObj["type"].toString() == "Node") {
            if (Node* endNode = nodeMap.value(endObj["id"].toString(), nullptr)) {
                newWire->setEndItem(endNode);
                newWire->setEndPos(endNode->scenePos());
                endNode->addWire(newWire);
            }
        } else if (endObj["type"].toString() == "Pin") {
            Component* comp = compMap.value(endObj["componentId"].toString(), nullptr);
            int pinIdx = endObj["pinIndex"].toInt();
            if (comp && pinIdx >= 0 && pinIdx < comp->getPins().size()) {
                Pin* endPin = comp->getPins()[pinIdx];
                newWire->setEndItem(endPin);
                newWire->setEndPos(endPin->scenePos());
                endPin->setWire(newWire);
            }
        }

        QList<QPointF> loadedPath;
        const QJsonArray pointsArray = wireObj["pathPoints"].toArray();
        for (const QJsonValue& ptVal : pointsArray) {
            QJsonObject ptObj = ptVal.toObject();
            loadedPath.append(QPointF(ptObj["x"].toDouble(), ptObj["y"].toDouble()));
        }
        newWire->setPathPoints(loadedPath);
        addWire(newWire);
    }
}
