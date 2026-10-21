#ifndef CIRCUITVIEW_H
#define CIRCUITVIEW_H

#include "componenttype.h"
#include "nodetype.h"

#include <QGraphicsView>

#include <QPoint>
#include <QPointF>
#include <QRectF>

class Circuit;
class IDmanager;
class QContextMenuEvent;
class QKeyEvent;
class QMouseEvent;
class QPainter;
class QWheelEvent;
class Wire;

class CircuitView : public QGraphicsView
{
    Q_OBJECT;

public:
    // !!!
    explicit CircuitView(Circuit* circuit, QWidget* parent = nullptr);
    virtual ~CircuitView();

    Circuit* getCircuitModel() const {return circuitModel;}

    enum ViewMode {PlaceMode, WireMode};
    void setViewMode(ViewMode mode) {currentMode = mode;}

    void addComponentAt(ComponentType type, const QPointF& position);
    void addNodeAt(NodeType type, const QPointF& position);

/*
public slots:
    void reassignIDs(); => TODO: let it be available from the Tools menu
*/

protected:
    // Handles zooming in and out using the mouse wheel and Ctrl modifier
    void wheelEvent(QWheelEvent* event) override;
    // Renders the background grid points for alignment
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    // Starts panning when the middle mouse button is pressed
    void mousePressEvent(QMouseEvent* event) override;
    // Stops panning on release and restores the standard cursor
    void mouseReleaseEvent(QMouseEvent* event) override;
    // Updates and emits the current scene coordinates of the cursor
    void mouseMoveEvent(QMouseEvent* event) override;

    void mouseDoubleClickEvent(QMouseEvent* event) override;
    // Handles component rotation (R) and deletion (Delete) via keyboard
    void keyPressEvent(QKeyEvent* event) override;
    // Manages and displays the context menu at the cursor position
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    Circuit* circuitModel = nullptr;
    QPoint contextMenuPosition;
    ViewMode currentMode = PlaceMode;
    Wire* tempWire = nullptr;

private slots:
    void addComponent();
    void addNode();
    // only available via context menu currently
    void reassignIDs();

signals:
    void mouseMoved(const QPointF& position);
};

#endif // CIRCUITVIEW_H
