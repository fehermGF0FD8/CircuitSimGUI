#ifndef PIN_H
#define PIN_H

#include <QGraphicsObject>

#include <QRectF>

class BuiltInComponent;
//class Node
class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
// class Wire

class Pin: public QGraphicsObject
{
    Q_OBJECT

public:
    explicit Pin(QPointF relativePosParam,
                 BuiltInComponent* parentCompParam);
    ~Pin() = default;

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem*,
               QWidget*) override;

    /*
    void setNode(Node* nodeParam) {connectedNode = nodeParam;}
    void removeNode() {connectedNode = nullptr;}
    Node* getNode() const {return connectedNode;}
    */

    /*
    void setWire(Wire* wireParam) {connectedWire = wireParam;}
    void removeWire() {connectedWire = nullptr;}
    Wire* getWire() const {return connectedWire;}
    */

private:
    BuiltInComponent* parentComp;
    // Node* connectedNode = nullptr;
    // Wire* connectedWire = nullptr;
    bool isHovered;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

signals:
    void pinClicked(Pin* pinParam);
};

#endif // PIN_H
