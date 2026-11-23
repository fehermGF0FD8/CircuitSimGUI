#include "subcircuitcomponent.h"

#include "circuit.h"
#include "componentpainter.h"
#include "node.h"
#include "pin.h"
#include "snapping.h"


#include <QPainter>
#include <QStringList>
#include <algorithm>

SubcircuitComponent::SubcircuitComponent(const QString& idParam, Circuit* rCircuitParam, QGraphicsItem* parent):
    Component(idParam, ComponentType::Subcircuit, parent),
    rCircuit(rCircuitParam)
{
    updatePinsAndGeometry();
}

// Determining pins and box size based on external nodes
void SubcircuitComponent::updatePinsAndGeometry() {
    if (!rCircuit) return;

    prepareGeometryChange();

    leftNodes.clear();
    rightNodes.clear();

    // Gathering nodes from referenced circuit
    for (Node* node : rCircuit->getNodes()) {
        if (!node) continue;
        NodeType type = node->getType();
        if (type == NodeType::X || type == NodeType::Y) {
            leftNodes.append(node);
        }
        else if (type == NodeType::A || type == NodeType::O) {
            rightNodes.append(node);
        }
    }

    // Determining the height of the box
    const qreal gridStep = Snapping::GRID_SIZE;
    int maxSidePins = std::max(leftNodes.size(), rightNodes.size());
    boxWidth = 4 * gridStep;
    boxHeight = std::max(2 * gridStep, (maxSidePins + 1) * gridStep);
    leadLength =  gridStep;

    QMap<Node*, Pin*> existingNodePins;
    for (auto [pin, node] : std::as_const(pinToNodeMap).asKeyValueRange()) {
        existingNodePins.insert(node, pin);
    }

    pins.clear();
    pinToNodeMap.clear();

    for (int i = 0; i < leftNodes.size(); ++i) {
        Node* node = leftNodes[i];
        QPointF pinPos(0.0, (i + 1) * gridStep);
        Pin* pin = existingNodePins.take(node);
        if (pin) {
            pin->setPos(pinPos);
        }
        else {
            pin = new Pin(pinPos, this);
        }
        addPin(pin);
        pinToNodeMap.insert(pin, node);
    }

    for (int i = 0; i < rightNodes.size(); ++i) {
        Node* node = rightNodes[i];
        QPointF pinPos(boxWidth + 2 * leadLength, (i + 1) * gridStep);
        Pin* pin = existingNodePins.take(node);
        if (pin) {
            pin->setPos(pinPos);
        } else {
            pin = new Pin(pinPos, this);
        }
        addPin(pin);
        pinToNodeMap.insert(pin, node);
    }

    for (Pin* deadPin : existingNodePins.values()) {
        deadPin->setWire(nullptr);
        delete deadPin;
    }

    update();
}

QList<Pin*> SubcircuitComponent::getObsoletePins() const {
    QList<Pin*> obsolete;
    if (!rCircuit) return pins;

    const QList<Node*>& activeNodes = rCircuit->getNodes();
    for (auto [pin, node] : pinToNodeMap.asKeyValueRange()) {
        if (!activeNodes.contains(node)) {
            obsolete.append(pin);
        }
    }
    return obsolete;
}

QRectF SubcircuitComponent::boundingRect() const {
    return ComponentPainter::calculateSubcircuitBoundRect(boxWidth, boxHeight, leadLength);
}

QRectF SubcircuitComponent::getObstacleRect() const {
    return QRectF(leadLength, 0.0, boxWidth, boxHeight);
}

void SubcircuitComponent::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QString name = rCircuit ? rCircuit->getName() : "";

    QStringList leftLabels;
    for (Node* node : std::as_const(leftNodes)) {
        leftLabels.append(node ? node->getNetName() : "");
    }

    QStringList rightLabels;
    for (Node* node : std::as_const(rightNodes)) {
        rightLabels.append(node ? node->getNetName() : "");
    }

    ComponentPainter::drawSubcircuit(painter, isSelected(), getID(),
                                     boxWidth, boxHeight, leadLength,
                                     leftLabels, rightLabels);
}

QJsonObject SubcircuitComponent::toJson() const {
    QJsonObject jsonComponent;

    jsonComponent["id"] = getID();
    jsonComponent["type"] = static_cast<int>(getType());
    jsonComponent["x"] = scenePos().x();
    jsonComponent["y"] = scenePos().y();
    jsonComponent["rotation"] = rotation();

    if (rCircuit) {
        jsonComponent["subcircuitName"] = rCircuit->getName();
    }

    return jsonComponent;
}
