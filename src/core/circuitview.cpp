#include "circuitview.h"

#include "resistor.h"

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
    QMenu* rMenu = builtInComponentMenu->addMenu("R");
    // QMenu* cMenu = builtInComponentMenu->addMenu("C");

    QAction* actionAddResistor = rMenu->addAction("Resistor");
    // QAction* actionAddCapacitor = cMenu->addAction("Capacitor");

    connect(actionAddResistor, &QAction::triggered, this, &CircuitView::addResistor);

    contextMenu.exec(event->globalPos());
}

void CircuitView::addResistorAt(const QPointF& position) {
    if (!scene()) return;
    Resistor* resistor = new Resistor("R?", "R");
    scene()->addItem(resistor);
    resistor->setPos(position);
}

void CircuitView::addResistor() {
    QPointF scenePos = mapToScene(contextMenuPosition);
    addResistorAt(scenePos);
}
