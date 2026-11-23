#include "circuitview.h"

#include "entities.h"

#include "autorouter.h"
#include "circuit.h"
#include "networktopology.h"
#include "node.h"
#include "nodetype.h"
#include "pin.h"
#include "snapping.h"
#include "wire.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QStringList>
#include <QVector>
#include <QWheelEvent>
#include <QtMath>

template <typename T>
QList<T*> filterByPrefix(const QList<T*>& allItems, const QString& prefix) {
    QList<T*> filteredList;

    for (T* item : allItems) {
        if (item->getID().startsWith(prefix + "_")) {
            filteredList.append(item);
        }
    }
    return filteredList;
}

template <typename T>
bool compareByID(T* a, T* b) {
    int sepA = a->getID().lastIndexOf('_');
    int sepB = b->getID().lastIndexOf('_');

    int numA = (sepA != -1) ? a->getID().mid(sepA + 1).toInt() : 0;
    int numB = (sepB != -1) ? b->getID().mid(sepB + 1).toInt() : 0;

    return numA < numB;
}

template <typename T>
void reassignListIDs(QList<T*>& items, const QString& prefix, IDmanager& idManager) {
    std::sort(items.begin(), items.end(), compareByID<T>);

    idManager.resetPrefix(prefix);

    for (T* item : items) {
        QString newID = idManager.getNextID(prefix);
        item->setID(newID);
    }
}

CircuitView::CircuitView(Circuit* circuit, QWidget* parent):
    QGraphicsView(parent),
    circuitModel(circuit)
{
    QGraphicsScene* tempScene = new QGraphicsScene(this);
    tempScene->setSceneRect(-2000, -2000, 4000, 4000);
    this->setScene(tempScene);

    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    this->viewport()->setMouseTracking(true);

    router = std::make_unique<AutoRouter>(tempScene);

    if (circuitModel) {

        for (Component* comp : circuitModel->getComponents()) {
            if (comp) {
                tempScene->addItem(comp);

                if (auto* subComp = dynamic_cast<SubcircuitComponent*>(comp)) {
                    if (Circuit* ref = subComp->getReferencedCircuit()) {
                        connect(ref, &Circuit::extNodesChanged,
                                this, &CircuitView::rCircuitChanged,
                                Qt::UniqueConnection);
                    }
                }
            }
        }

        for (Node* node : circuitModel->getNodes()) {
            if (node) {
                tempScene->addItem(node);
            }
        }

        for (Wire* wire : circuitModel->getWires()) {
            if (wire) {
                tempScene->addItem(wire);
            }
        }
    }
}

CircuitView::~CircuitView() {
    delete circuitModel;
}

void CircuitView::wheelEvent(QWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        const double scaleFactor = 1.1;
        double factor = (event->angleDelta().y() > 0) ? scaleFactor : 1.0/scaleFactor;
        double currentScale = transform().m11();
        double nextScale = currentScale * factor;

        if (nextScale > 0.2 && nextScale < 2.0) {
            scale(factor, factor);
        }

        event->accept();
    }
    else {
        QGraphicsView::wheelEvent(event);
    }
}

void CircuitView::drawBackground(QPainter* painter, const QRectF& rect) {
    painter->fillRect(rect, Qt::white); // QColor(240, 240, 240)

    double currentScale = transform().m11();
    if (currentScale < 0.4) {
        return;
    }

    const int gridSize = Snapping::GRID_SIZE;

    int firstX = qFloor(rect.left() / gridSize);
    int lastX  = qCeil(rect.right() / gridSize);
    int firstY = qFloor(rect.top() / gridSize);
    int lastY  = qCeil(rect.bottom() / gridSize);

    QPen pen(QColor(200, 200, 200));
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setCosmetic(true);

    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);

    QVector<QPointF> points;
    for (int i = firstX; i <= lastX; ++i) {
        for (int j = firstY; j <= lastY; ++j) {
            points.append(QPointF(i * gridSize, j * gridSize));
        }
    }
    painter->drawPoints(points.data(), points.size());
}

void CircuitView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        QMouseEvent panningEvent(
            event->type(),
            event->position(),
            event->globalPosition(),
            Qt::LeftButton,
            Qt::LeftButton,
            event->modifiers()
        );
        QGraphicsView::mousePressEvent(&panningEvent);
    }
    else if ((event->button() == Qt::LeftButton) && (currentMode == WireMode)) {
        Node* clickedNode = nullptr;
        Pin* clickedPin = nullptr;

        const QList<QGraphicsItem*> itemsUnderCursor = items(event->pos());

        for (QGraphicsItem* item : itemsUnderCursor) {
            if (item == tempWire) continue;
            if (Node* nodeUnderCursor = dynamic_cast<Node*>(item)) {
                clickedNode = nodeUnderCursor;
                break;
            }
            else if (Pin* pinUnderCursor = dynamic_cast<Pin*>(item)) {
                clickedPin = pinUnderCursor;
                break;
            }
        }

        if ((clickedNode || clickedPin) && tempWire == nullptr) {
            if (clickedPin && clickedPin->isConnected()) {
                return;
            }

            tempWire = new Wire();

            if (clickedNode) {
                tempWire->setStartItem(clickedNode);
                tempWire->setStartPos(clickedNode->scenePos());
            }
            else if (clickedPin) {
                tempWire->setStartItem(clickedPin);
                tempWire->setStartPos(clickedPin->scenePos());
            }

            if (router) {
                router->initRouting();
            }

            isRouting = true;

            QPointF sceneClickPos = mapToScene(event->pos());
            tempWire->setEndPos(sceneClickPos);

            scene()->addItem(tempWire);

            return;
        }
        else if (tempWire != nullptr) {
            bool isInvalidConnection = false;

            if ((!clickedNode && !clickedPin) ||
                (clickedNode && tempWire->getStartNode() == clickedNode) ||
                (clickedNode && tempWire->getStartNode() && tempWire->getStartNode()->getType() != clickedNode->getType()) ||
                (clickedPin && tempWire->getStartPin()) ||
                (clickedPin && clickedPin->isConnected()))
            {
                isInvalidConnection = true;
            }

            if (isInvalidConnection) {
                scene()->removeItem(tempWire);
                delete tempWire;
                tempWire = nullptr;
                isRouting = false;
                return;
            }

            if (router) {
                QPointF finalTargetPos = clickedNode ? clickedNode->scenePos() : clickedPin->scenePos();

                Direction startPED = Direction::None;
                if (tempWire->getStartPin()) {
                    startPED = router->getPinExitDirection(tempWire->getStartPin());
                }

                Direction endPED = Direction::None;
                if (clickedPin) {
                    endPED = router->getPinExitDirection(clickedPin);
                }

                QList<QPointF> finalPath = router->findPath(tempWire->getStartPos(), finalTargetPos, startPED, endPED);

                if (!finalPath.isEmpty()) {
                    tempWire->setPathPoints(finalPath);
                }
                else {
                    scene()->removeItem(tempWire);
                    delete tempWire;
                    tempWire = nullptr;
                    isRouting = false;
                    return;
                }
            }

            if (clickedNode) {
                tempWire->setEndItem(clickedNode);
                tempWire->setEndPos(clickedNode->scenePos());
            }
            else if (clickedPin) {
                tempWire->setEndItem(clickedPin);
                tempWire->setEndPos(clickedPin->scenePos());
            }

            if (clickedNode) {
                clickedNode->addWire(tempWire);
            }
            else if (clickedPin) {
                clickedPin->setWire(tempWire);
            }

            if (tempWire->getStartNode()) {
                tempWire->getStartNode()->addWire(tempWire);
            }
            else if (tempWire->getStartPin()) {
                tempWire->getStartPin()->setWire(tempWire);
            }

            if (clickedNode) {
                NetworkTopology::updateNetworkNames(clickedNode);
            }
            else if (tempWire->getStartNode()) {
                NetworkTopology::updateNetworkNames(tempWire->getStartNode());
            }

            // !!!
            circuitModel->addWire(tempWire);

            isRouting = false;
            tempWire = nullptr;

            return;
        }
    }
    else if ((event->button() == Qt::LeftButton) && (currentMode != WireMode)) {
        QGraphicsItem* clickedItem = itemAt(event->pos());
        if (clickedItem && clickedItem->parentItem()) {
            clickedItem = clickedItem->parentItem();
        }
        if (clickedItem && (dynamic_cast<Component*>(clickedItem) || dynamic_cast<Node*>(clickedItem))) {
            isTransforming = true;
            collectAndHideWires(clickedItem);
        }
        QGraphicsView::mousePressEvent(event);
    }
    else {
        QGraphicsView::mousePressEvent(event);
    }
}

void CircuitView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        setDragMode(QGraphicsView::NoDrag);
    }
    QGraphicsView::mouseReleaseEvent(event);
    if (isTransforming) {
        isTransforming = false;
        rerouteAffectedWires();
    }
}

void CircuitView::mouseMoveEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->position().toPoint());
    QPointF snappedPos = Snapping::snapToGrid(scenePos);
    emit mouseMoved(scenePos);

    if (currentMode == WireMode && isRouting && tempWire != nullptr && router != nullptr) {
        Direction startPED = Direction::None;
        if (tempWire->getStartPin()) {
            startPED = router->getPinExitDirection(tempWire->getStartPin());
        }

        Direction endPED = Direction::None;
        QList<QGraphicsItem*> itemsAtTarget = scene()->items(snappedPos);
        for (QGraphicsItem* item : itemsAtTarget) {
            if (Pin* targetPin = dynamic_cast<Pin*>(item)) {
                endPED = router->getPinExitDirection(targetPin);
                break;
            }
        }

        QList<QPointF> path = router->findPath(tempWire->getStartPos(), snappedPos, startPED, endPED);

        if (!path.isEmpty()) {
            tempWire->setPathPoints(path);
        }
        else {
            tempWire->setPathPoints({});
        }
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void CircuitView::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && currentMode == WireMode) {
        return;
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

void CircuitView::keyPressEvent(QKeyEvent* event) {
    if(!scene()) {
        QGraphicsView::keyPressEvent(event);
        return;
    }
    if (event->key() == Qt::Key_R) {
        for (QGraphicsItem* item : this->scene()->selectedItems()) {
            collectAndHideWires(item);
        }
        for (QGraphicsItem* item : this->scene()->selectedItems()) {
            item->setRotation(item->rotation() == 0 ? 90 : 0);
        }
        rerouteAffectedWires();
    }
    else if (event->key() == Qt::Key_Delete) {
        const QList<QGraphicsItem*> itemsList = this->scene()->selectedItems();
        for (QGraphicsItem* item : itemsList) {
            Pin* pin = dynamic_cast<Pin*>(item);
            Node* node = dynamic_cast<Node*>(item);
            Wire* wire = dynamic_cast<Wire*>(item);
            Component* component = dynamic_cast<Component*>(item);

            if (pin) {
                continue;
            }
            else if (node) {
                QList<Node*> neighborsToUpdate;
                for (Wire* connectedWire : node->getWires()) {
                    if (connectedWire->getStartNode() && connectedWire->getStartNode() != node) {
                        neighborsToUpdate.append(connectedWire->getStartNode());
                    }
                    else if (connectedWire->getEndNode() && connectedWire->getEndNode() != node) {
                        neighborsToUpdate.append(connectedWire->getEndNode());
                    }
                    circuitModel->removeWire(connectedWire);
                }
                scene()->removeItem(item);
                // !!!
                circuitModel->removeNode(node);
                delete item;
                for (Node* neighbor : neighborsToUpdate) {
                    if (!itemsList.contains(neighbor)) {
                        NetworkTopology::updateNetworkNames(neighbor);
                    }
                }
                continue;
            }
            else if (wire) {
                Node* startNode = wire->getStartNode();
                Node* endNode = wire->getEndNode();

                scene()->removeItem(item);
                // !!!
                circuitModel->removeWire(wire);
                delete item;

                if (startNode) {
                    NetworkTopology::updateNetworkNames(startNode);
                }
                if (endNode) {
                    NetworkTopology::updateNetworkNames(endNode);
                }

                continue;
            }
            else if (component) {
                // !!!
                for (Pin* componentPin : component->getPins()) {
                    if (Wire* pinWire = componentPin->getWire()) {
                        circuitModel->removeWire(pinWire);
                    }
                }
                scene()->removeItem(item);
                // !!!
                circuitModel->removeComponent(component);
                delete item;
            }
            else {
                scene()->removeItem(item);
                delete item;
            }
        }
    }
    else {
        QGraphicsView::keyPressEvent(event);
    }
}

void CircuitView::contextMenuEvent(QContextMenuEvent* event) {
    if (currentMode == WireMode) {
        event->accept();
        return;
    }

    contextMenuPosition = event->pos();

    QMenu contextMenu(this);
    QMenu* addMenu = contextMenu.addMenu("Add");
    QMenu* subcircuitMenu = addMenu->addMenu("Subcircuit");
    QMenu* builtInComponentMenu = addMenu->addMenu("Built-in Component");
    QMenu* nodeMenu = addMenu->addMenu("Node");

    if (availableSubcircuits.isEmpty()) {
        subcircuitMenu->setEnabled(false);
    }
    else {
        for (Circuit* sub : std::as_const(availableSubcircuits)) {
            if (!sub) continue;
            QAction* actionAddSub = subcircuitMenu->addAction(sub->getName());
            actionAddSub->setData(QVariant::fromValue(sub));
            connect(actionAddSub, &QAction::triggered, this, &CircuitView::addSubcircuit);
        }
    }

    QAction* actionAddResistor = builtInComponentMenu->addAction("Resistor");
    actionAddResistor->setData(static_cast<int>(ComponentType::Resistor));
    QAction* actionAddConductance = builtInComponentMenu->addAction("Conductance");
    actionAddConductance->setData(static_cast<int>(ComponentType::Conductance));
    QAction* actionAddCapacitor = builtInComponentMenu->addAction("Capacitor");
    actionAddCapacitor->setData(static_cast<int>(ComponentType::Capacitor));
    QAction* actionAddCurrentSource = builtInComponentMenu->addAction("Current Source");
    actionAddCurrentSource->setData(static_cast<int>(ComponentType::CurrentSource));
    QAction* actionAddVoltageSource = builtInComponentMenu->addAction("Voltage Source");
    actionAddVoltageSource->setData(static_cast<int>(ComponentType::VoltageSource));
    //...

    QAction* actionAddNTypeNode = nodeMenu->addAction("N-type Node");
    actionAddNTypeNode->setData(static_cast<int>(NodeType::N));
    QAction* actionAddBTypeNode = nodeMenu->addAction("B-type Node");
    actionAddBTypeNode->setData(static_cast<int>(NodeType::B));
    QAction* actionAddXTypeNode = nodeMenu->addAction("X-type Node");
    actionAddXTypeNode->setData(static_cast<int>(NodeType::X));
    QAction* actionAddYTypeNode = nodeMenu->addAction("Y-type Node");
    actionAddYTypeNode->setData(static_cast<int>(NodeType::Y));
    QAction* actionAddATypeNode = nodeMenu->addAction("A-type Node");
    actionAddATypeNode->setData(static_cast<int>(NodeType::A));
    QAction* actionAddOTypeNode = nodeMenu->addAction("O-type Node");
    actionAddOTypeNode->setData(static_cast<int>(NodeType::O));


    connect(actionAddResistor, &QAction::triggered, this, &CircuitView::addComponent);
    connect(actionAddConductance, &QAction::triggered, this, &CircuitView::addComponent);
    connect(actionAddCapacitor, &QAction::triggered, this, &CircuitView::addComponent);
    connect(actionAddCurrentSource, &QAction::triggered, this, &CircuitView::addComponent);
    connect(actionAddVoltageSource, &QAction::triggered, this, &CircuitView::addComponent);
    //...

    connect(actionAddNTypeNode, &QAction::triggered, this, &CircuitView::addNode);
    connect(actionAddBTypeNode, &QAction::triggered, this, &CircuitView::addNode);
    connect(actionAddXTypeNode, &QAction::triggered, this, &CircuitView::addNode);
    connect(actionAddYTypeNode, &QAction::triggered, this, &CircuitView::addNode);
    connect(actionAddATypeNode, &QAction::triggered, this, &CircuitView::addNode);
    connect(actionAddOTypeNode, &QAction::triggered, this, &CircuitView::addNode);

    contextMenu.addSeparator();

    QAction* actionReassignIDs = contextMenu.addAction("Reassign IDs");

    connect(actionReassignIDs, &QAction::triggered, this, &CircuitView::reassignIDs);

    contextMenu.exec(event->globalPos());
}

void CircuitView::addSubcircuit() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) return;

    Circuit* subcircuit = action->data().value<Circuit*>();
    if (!subcircuit) return;

    QPointF scenePos = mapToScene(contextMenuPosition);
    addSubcircuitAt(subcircuit, scenePos);
}

void CircuitView::addComponent() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) return;

    ComponentType type = static_cast<ComponentType>(action->data().toInt());

    QPointF scenePos = mapToScene(contextMenuPosition);
    addComponentAt(type, scenePos);
}

void CircuitView::addNode() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) return;

    NodeType type = static_cast<NodeType>(action->data().toInt());

    QPointF scenePos = mapToScene(contextMenuPosition);
    addNodeAt(type, scenePos);
}

void CircuitView::addSubcircuitAt(Circuit* subcircuit, const QPointF& position) {
    if (!scene() || !circuitModel || !subcircuit) return;
    if (!subcircuit->isSubcircuit() || subcircuit->dependsOn(circuitModel)) return;

    QString newID = circuitModel->getIdManager().getNextID(subcircuit->getName());
    auto* subComp = new SubcircuitComponent(newID, subcircuit);

    scene()->addItem(subComp);
    circuitModel->addComponent(subComp);
    subComp->setPos(Snapping::snapToGrid(position));

    connect(subcircuit, &Circuit::extNodesChanged,
            this, &CircuitView::rCircuitChanged,
            Qt::UniqueConnection);
}

void CircuitView::addComponentAt(ComponentType type, const QPointF& position) {
    if (!scene()) return;

    QString prefix;
    BuiltInComponent* newComponent = nullptr;

    switch (type) {
    case ComponentType::Resistor:
        prefix = "R";
        newComponent = new Resistor("?");
        break;
    case ComponentType::Conductance:
        prefix = "G";
        newComponent = new Conductance("?");
        break;
    case ComponentType::Capacitor:
        prefix = "C";
        newComponent = new Capacitor("?");
        break;
    case ComponentType::CurrentSource:
        prefix = "I";
        newComponent = new CurrentSource("?");
        break;
    case ComponentType::VoltageSource:
        prefix = "V";
        newComponent = new VoltageSource("?");
        break;
    //...
    default:
        break;
    }

    if (newComponent) {
        // !!!
        QString newCompID = circuitModel->getIdManager().getNextID(prefix);
        newComponent->setID(newCompID);

        scene()->addItem(newComponent);
        // !!!
        circuitModel->addComponent(newComponent);
        newComponent->setPos(position);
    }
}

void CircuitView::addNodeAt(NodeType type, const QPointF& position) {
    if (!scene()) return;

    QString prefix;
    Node* newNode = nullptr;

    switch (type) {
    case NodeType::N:
        prefix = "N";
        break;
    case NodeType::B:
        prefix = "B";
        break;
    case NodeType::X:
        prefix = "X";
        break;
    case NodeType::Y:
        prefix = "Y";
        break;
    case NodeType::A:
        prefix = "A";
        break;
    case NodeType::O:
        prefix = "O";
        break;
    default: break;
    }

    // !!!
    QString newNodeID = circuitModel->getIdManager().getNextID(prefix);
    newNode = new Node(newNodeID, type);

    scene()->addItem(newNode);
    // !!!
    circuitModel->addNode(newNode);
    newNode->setPos(position);
}

void CircuitView::reassignIDs() {
    if (!scene()) return;

    QList<BuiltInComponent*> allComps;
    QList<Node*> allNodes;
    QMap<QString, QList<SubcircuitComponent*>> subGroups;
    for (QGraphicsItem* item : scene()->items()) {
        if (auto* comp = dynamic_cast<BuiltInComponent*>(item)) {
            allComps.append(comp);
        }
        else if (auto* subComp = dynamic_cast<SubcircuitComponent*>(item)) {
            if (Circuit* ref = subComp->getReferencedCircuit()) {
                subGroups[ref->getName()].append(subComp);
            }
        }
        else if (auto* node = dynamic_cast<Node*>(item)) {
            allNodes.append(node);
        }
    }

    QList<BuiltInComponent*> resistors = filterByPrefix(allComps, "R");
    QList<BuiltInComponent*> conductances = filterByPrefix(allComps, "G");
    QList<BuiltInComponent*> capacitors = filterByPrefix(allComps, "C");
    QList<BuiltInComponent*> currentSources = filterByPrefix(allComps, "I");
    QList<BuiltInComponent*> voltageSources = filterByPrefix(allComps, "V");

    QList<Node*> nNodes = filterByPrefix(allNodes, "N");
    QList<Node*> bNodes = filterByPrefix(allNodes, "B");
    QList<Node*> xNodes = filterByPrefix(allNodes, "X");
    QList<Node*> yNodes = filterByPrefix(allNodes, "Y");
    QList<Node*> aNodes = filterByPrefix(allNodes, "A");
    QList<Node*> oNodes = filterByPrefix(allNodes, "O");


    // !!!
    reassignListIDs(resistors, "R", circuitModel->getIdManager());
    reassignListIDs(conductances, "G", circuitModel->getIdManager());
    reassignListIDs(capacitors, "C", circuitModel->getIdManager());
    reassignListIDs(currentSources, "I", circuitModel->getIdManager());
    reassignListIDs(voltageSources, "V", circuitModel->getIdManager());

    for (auto [subName, subList] : subGroups.asKeyValueRange()) {
        reassignListIDs(subList, subName, circuitModel->getIdManager());
    }

    reassignListIDs(nNodes, "N", circuitModel->getIdManager());
    reassignListIDs(bNodes, "B", circuitModel->getIdManager());
    reassignListIDs(xNodes, "X", circuitModel->getIdManager());
    reassignListIDs(yNodes, "Y", circuitModel->getIdManager());
    reassignListIDs(aNodes, "A", circuitModel->getIdManager());
    reassignListIDs(oNodes, "O", circuitModel->getIdManager());

    for (Node* node: std::as_const(allNodes)) {
        if (node) {
            NetworkTopology::updateNetworkNames(node);
        }
    }

    circuitModel->notifyExtNodesChanged();

    scene()->update();
}

void CircuitView::collectAndHideWires(QGraphicsItem* item) {
    if (!item) return;

    affectedWires.clear();

    for (QGraphicsItem* sceneItem : scene()->items()) {
        if (Wire* wire = dynamic_cast<Wire*>(sceneItem)) {
            bool isConnected = false;

            if (Component* comp = dynamic_cast<Component*>(item)) {
                for (Pin* pin : comp->getPins()) {
                    if (wire->getStartPin() == pin || wire->getEndPin() == pin) {
                        isConnected = true;
                        break;
                    }
                }
            }

            else if (Node* node = dynamic_cast<Node*>(item)) {
                if (wire->getStartNode() == node || wire->getEndNode() == node) {
                    isConnected = true;
                }
            }

            if (isConnected) {
                wire->hide();
                affectedWires.append(wire);
            }
        }
    }
}

void CircuitView::rerouteAffectedWires() {
    if (affectedWires.isEmpty() || !router) return;

    // invokes updateObstacleMap()
    router->initRouting();

    for (Wire* wire : affectedWires) {
        QPointF startPos = wire->getStartPin() ? wire->getStartPin()->scenePos() : wire->getStartNode()->scenePos();
        Direction startPED = wire->getStartPin() ? router->getPinExitDirection(wire->getStartPin()) : Direction::None;

        QPointF endPos = wire->getEndPin() ? wire->getEndPin()->scenePos() : wire->getEndNode()->scenePos();
        Direction endPED = wire->getEndPin() ? router->getPinExitDirection(wire->getEndPin()) : Direction::None;

        QList<QPointF> newPath = router->findPath(startPos, endPos, startPED, endPED);

        if (!newPath.isEmpty()) {
            wire->setPathPoints(newPath);
            wire->show();
        }
        else {
            Node* startNode = wire->getStartNode();
            Node* endNode = wire->getEndNode();

            scene()->removeItem(wire);
            // circuitModel->removeWire(wire);
            delete wire;

            if (startNode) {
                NetworkTopology::updateNetworkNames(startNode);
            }
            if (endNode) {
                NetworkTopology::updateNetworkNames(endNode);
            }
        }
    }
    affectedWires.clear();
}

void CircuitView::removeAndDelWire(Wire* wire) {
    if (!wire) return;
    Node* startNode = wire->getStartNode();
    Node* endNode = wire->getEndNode();

    if (scene()) scene()->removeItem(wire);
    if (circuitModel) circuitModel->removeWire(wire);
    delete wire;

    if (startNode) NetworkTopology::updateNetworkNames(startNode);
    if (endNode) NetworkTopology::updateNetworkNames(endNode);
}

void CircuitView::rCircuitChanged() {
    if (!scene() || !circuitModel) return;

    Circuit* modifiedCircuit = qobject_cast<Circuit*>(sender());
    if (!modifiedCircuit) return;

    QList<SubcircuitComponent*> instances;
    for (Component* comp : circuitModel->getComponents()) {
        if (auto* subComp = dynamic_cast<SubcircuitComponent*>(comp)) {
            if (subComp->getReferencedCircuit() == modifiedCircuit) {
                instances.append(subComp);
            }
        }
    }
    if (instances.isEmpty()) return;

    // all external nodes are removed => not a subc anymore
    if (!modifiedCircuit->isSubcircuit()) {
        for (SubcircuitComponent* subComp : instances) {
            for (Pin* pin : subComp->getPins()) {
                if (Wire* pinWire = pin->getWire()) {
                    removeAndDelWire(pinWire);
                    pin->setWire(nullptr);
                }
            }
            scene()->removeItem(subComp);
            circuitModel->removeComponent(subComp);
            delete subComp;
        }
        scene()->update();
        return;
    }

    // remaining pins != 0 -> deleting wires connected to deadpins, updating box and routing
    for (SubcircuitComponent* subComp : instances) {
        const QList<Pin*> obsoletePins = subComp->getObsoletePins();
        for (Pin* deadPin : obsoletePins) {
            if (Wire* pinWire = deadPin->getWire()) {
                removeAndDelWire(pinWire);
                deadPin->setWire(nullptr);
            }
        }

        affectedWires.clear();
        collectAndHideWires(subComp);
        subComp->updatePinsAndGeometry();
        rerouteAffectedWires();
    }

    scene()->update();
}
