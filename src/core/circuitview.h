#ifndef CIRCUITVIEW_H
#define CIRCUITVIEW_H

#include <QGraphicsView>

#include <QPoint>
#include <QPointF>
#include <QRectF>

class QContextMenuEvent;
class QKeyEvent;
class QMouseEvent;
class QPainter;
class QWheelEvent;

class CircuitView : public QGraphicsView
{
    Q_OBJECT;

public:
    explicit CircuitView(QWidget* parent = nullptr);
    // Places a new resistor (specification: const. reg)
    // component at the specified scene position
    void addResistorAt(const QPointF& position);

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
    // Handles component rotation (R) and deletion (Delete) via keyboard
    void keyPressEvent(QKeyEvent* event) override;
    // Manages and displays the context menu at the cursor position
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    QPoint contextMenuPosition;

private slots:
    // Add a resistor from the context menu action
    void addResistor();
    // void addCapacitor();

signals:
    void mouseMoved(const QPointF& position);
};

#endif // CIRCUITVIEW_H
