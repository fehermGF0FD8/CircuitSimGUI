#include "circuitview.h"

#include "idmanager.h"

#include "resistor.h"
#include "conductance.h"
#include "capacitor.h"
#include "currentsource.h"
#include "voltagesource.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QVector>
#include <QWheelEvent>
#include <QtMath>

CircuitView::CircuitView(QWidget* parent): QGraphicsView(parent) {
    QGraphicsScene* tempScene = new QGraphicsScene(this);
    tempScene->setSceneRect(-2000, -2000, 4000, 4000);
    this->setScene(tempScene);

    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    this->viewport()->setMouseTracking(true);
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
    emit mouseMoved(scenePos);
    QGraphicsView::mouseMoveEvent(event);
}

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
        for (QGraphicsItem* item : this->scene()->selectedItems()) {
            delete item;
        }
    }
    else {
        QGraphicsView::keyPressEvent(event);
    }
}

void CircuitView::contextMenuEvent(QContextMenuEvent* event) {
    contextMenuPosition = event->pos();

    QMenu contextMenu(this);
    QMenu* addMenu = contextMenu.addMenu("Add");
    QMenu* builtInComponentMenu = addMenu->addMenu("Built-in Component");

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

    connect(actionAddResistor, &QAction::triggered, this, &CircuitView::addComponent);
    connect(actionAddConductance, &QAction::triggered, this, &CircuitView::addComponent);
    connect(actionAddCapacitor, &QAction::triggered, this, &CircuitView::addComponent);
    connect(actionAddCurrentSource, &QAction::triggered, this, &CircuitView::addComponent);
    connect(actionAddVoltageSource, &QAction::triggered, this, &CircuitView::addComponent);
    //...

    contextMenu.exec(event->globalPos());
}

void CircuitView::addComponent() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) return;

    ComponentType type = static_cast<ComponentType>(action->data().toInt());

    QPointF scenePos = mapToScene(contextMenuPosition);
    addComponentAt(type, scenePos);
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
        if (idManager) {
            QString newCompID = idManager->getNextID(prefix);
            newComponent->setID(newCompID);
        }

        scene()->addItem(newComponent);
        newComponent->setPos(position);
    }
}
