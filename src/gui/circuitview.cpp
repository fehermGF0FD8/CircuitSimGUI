#include "circuitview.h"

#include "entities.h"

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
    QStringList partsA = a->getID().split("_");
    QStringList partsB = b->getID().split("_");

    int numA = 0;
    int numB = 0;

    if (partsA.size() > 1) {
        numA = partsA[1].toInt();
    }
    if (partsB.size() > 1) {
        numB = partsB[1].toInt();
    }

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

// !!!
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

    if (circuitModel) {

        for (BuiltInComponent* comp : circuitModel->getComponents()) {
            if (comp) {
                tempScene->addItem(comp);
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
                wire->updatePosition();
            }
        }
    }
}

// !!!
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

    const int gridSize = 20;

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

// !!!
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
                return;
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

            tempWire = nullptr;

            return;
        }
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
}

void CircuitView::mouseMoveEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->position().toPoint());
    QPointF snappedPos = Snapping::snapToGrid(scenePos);
    emit mouseMoved(scenePos);
    if (currentMode == WireMode && tempWire != nullptr) {
        tempWire->setEndPos(snappedPos);
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

// !!!
void CircuitView::keyPressEvent(QKeyEvent* event) {
    if(!scene()) {
        QGraphicsView::keyPressEvent(event);
        return;
    }
    if (event->key() == Qt::Key_R) {
        for (QGraphicsItem* item : this->scene()->selectedItems()) {
            item->setRotation(item->rotation() == 0 ? 90 : 0);
        }
    }
    else if (event->key() == Qt::Key_Delete) {
        const QList<QGraphicsItem*> itemsList = this->scene()->selectedItems();
        for (QGraphicsItem* item : itemsList) {
            Pin* pin = dynamic_cast<Pin*>(item);
            Node* node = dynamic_cast<Node*>(item);
            Wire* wire = dynamic_cast<Wire*>(item);
            BuiltInComponent* component = dynamic_cast<BuiltInComponent*>(item);

            if (pin) {
                continue;
            }
            else if (node) {
                QList<Node*> neighborsToUpdate;
                for (Wire* connectedWire : node->getWires()) {
                    if (connectedWire->getStartNode() && connectedWire->getStartNode() != node) {
                        neighborsToUpdate.append(connectedWire->getStartNode());
                    } else if (connectedWire->getEndNode() && connectedWire->getEndNode() != node) {
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
    else if (event->key() == Qt::Key_T) {
        if (currentMode == WireMode && tempWire != nullptr) {
            tempWire->toggleRoutingMode();
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
    QMenu* builtInComponentMenu = addMenu->addMenu("Built-in Component");
    QMenu* nodeMenu = addMenu->addMenu("Node");

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
    //...

    connect(actionAddResistor, &QAction::triggered, this, &CircuitView::addComponent);
    connect(actionAddConductance, &QAction::triggered, this, &CircuitView::addComponent);
    connect(actionAddCapacitor, &QAction::triggered, this, &CircuitView::addComponent);
    connect(actionAddCurrentSource, &QAction::triggered, this, &CircuitView::addComponent);
    connect(actionAddVoltageSource, &QAction::triggered, this, &CircuitView::addComponent);
    //...

    connect(actionAddNTypeNode, &QAction::triggered, this, &CircuitView::addNode);
    connect(actionAddBTypeNode, &QAction::triggered, this, &CircuitView::addNode);
    //...

    contextMenu.addSeparator();

    QAction* actionReassignIDs = contextMenu.addAction("Reassign IDs");

    connect(actionReassignIDs, &QAction::triggered, this, &CircuitView::reassignIDs);

    contextMenu.exec(event->globalPos());
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

// !!!
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

// !!!
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
    //...
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

// !!!
void CircuitView::reassignIDs() {
    if (!scene()) return;

    QList<BuiltInComponent*> allComps;
    for (QGraphicsItem* item : scene()->items()) {
        if (BuiltInComponent* comp = dynamic_cast<BuiltInComponent*>(item)) {
            allComps.append(comp);
        }
    }

    QList<Node*> allNodes;
    for (QGraphicsItem* item : scene()->items()) {
        if (Node* node = dynamic_cast<Node*>(item)) {
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

    // !!!
    reassignListIDs(resistors, "R", circuitModel->getIdManager());
    reassignListIDs(conductances, "G", circuitModel->getIdManager());
    reassignListIDs(capacitors, "C", circuitModel->getIdManager());
    reassignListIDs(currentSources, "I", circuitModel->getIdManager());
    reassignListIDs(voltageSources, "V", circuitModel->getIdManager());

    reassignListIDs(nNodes, "N", circuitModel->getIdManager());
    reassignListIDs(bNodes, "B", circuitModel->getIdManager());

    for (Node* node: std::as_const(allNodes)) {
        if (node) {
            NetworkTopology::updateNetworkNames(node);
        }
    }

    scene()->update();
}

/* void CircuitView::reassignCompIDs() {
    // Safety check
    if (!scene()) return;

    // Reset the internal counters
    idManager.resetAllComponents();

    // List all built-in components currently on the scene
    QList<BuiltInComponent*> componentsToRename;
    for (QGraphicsItem* item : scene()->items()) {
        if (BuiltInComponent* comp = dynamic_cast<BuiltInComponent*>(item)) {
            componentsToRename.append(comp);
        }
    }

    // Sort the listed components in ascending order
    std::sort(componentsToRename.begin(), componentsToRename.end(), compareComponents);

    // Iterate through the sorted list and assign new IDs
    for (BuiltInComponent* comp : componentsToRename) {
        QString prefix;

        switch (comp->getType()) {
        case ComponentType::Resistor:
            prefix = "R";
            break;
        case ComponentType::Conductance:
            prefix = "G";
            break;
        case ComponentType::Capacitor:
            prefix = "C";
            break;
        case ComponentType::CurrentSource:
            prefix = "I";
            break;
        case ComponentType::VoltageSource:
            prefix = "V";
            break;
        default:
            break;
        }

        QString newID = idManager.getNextID(prefix);
        comp->setID(newID);

        // repaint the component with its new text
        comp->update();
    }
} */
