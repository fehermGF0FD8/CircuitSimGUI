#include "circuit.h"

#include "entities.h"

#include "componenttype.h"
#include "pin.h"

#include <QJsonArray>
#include <QMap>

Circuit::Circuit(const QString& name):
    circuitName(name)
{

}
Circuit::~Circuit() {
    components.clear();
    nodes.clear();
    wires.clear();
}

void Circuit::addComponent(BuiltInComponent* comp) {
    if (comp != nullptr && !components.contains(comp)) {
        components.append(comp);
    }
}

void Circuit::removeComponent(BuiltInComponent* comp) {
    if (comp != nullptr) {
        components.removeOne(comp);
    }
}

void Circuit::addNode(Node* node) {
    if (node != nullptr && !nodes.contains(node)) {
        nodes.append(node);
    }
}

void Circuit::removeNode(Node* node) {
    if (node != nullptr) {
        nodes.removeOne(node);
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
    for (BuiltInComponent* comp : getComponents()) {
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

void Circuit::fromJson(const QJsonObject& json) {
    setName(json["circuitName"].toString());

    QMap<QString, Node*> nodeMap;
    QMap<QString, BuiltInComponent*> compMap;

    QJsonArray compsArray = json["components"].toArray();
    for (const QJsonValue& val : std::as_const(compsArray)) {
        QJsonObject compObj = val.toObject();

        QString id = compObj["id"].toString();
        ComponentType type = static_cast<ComponentType>(compObj["type"].toInt());

        QPointF scenePos = QPointF(compObj["x"].toDouble(), compObj["y"].toDouble());
        double rotationAngle = compObj["rotation"].toDouble();

        BuiltInComponent* newComponent = nullptr;

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

        bool isHorizontalFirst = wireObj["isHorizontalFirst"].toBool();

        Wire* newWire = new Wire();

        newWire->setIsHorizontalFirst(isHorizontalFirst);

        QJsonObject startObj = wireObj["start"].toObject();
        if (startObj["type"].toString() == "Node") {
            Node* startNode = nodeMap[startObj["id"].toString()];
            newWire->setStartItem(startNode);
            newWire->setStartPos(startNode->scenePos());
            startNode->addWire(newWire);

        }
        else if (startObj["type"].toString() == "Pin") {
            BuiltInComponent* comp = compMap[startObj["componentId"].toString()];
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
            BuiltInComponent* comp = compMap[endObj["componentId"].toString()];
            Pin* endPin = comp->getPins()[endObj["pinIndex"].toInt()];
            newWire->setEndItem(endPin);
            newWire->setEndPos(endPin->scenePos());
            endPin->setWire(newWire);
        }

        addWire(newWire);
    }
}
