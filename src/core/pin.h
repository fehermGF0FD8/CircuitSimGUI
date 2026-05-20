#ifndef PIN_H
#define PIN_H

#include <QGraphicsObject>

#include <QRectF>

class BuiltInComponent;
class Node;
class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class Wire;

class Pin: public QGraphicsObject
{
    Q_OBJECT

public:
    explicit Pin(QPointF relativePosParam,
                 BuiltInComponent* parentCompParam);
    virtual ~Pin();

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem*,
               QWidget*) override;

    void setWire(Wire* wireParam) {connectedWire = wireParam;}
    void removeWire() {connectedWire = nullptr;}
    Wire* getWire() const {return connectedWire;}

    bool isConnected() const {
        return connectedWire != nullptr;
    }


private:
    BuiltInComponent* parentComp;
    Wire* connectedWire = nullptr;
    bool isHovered;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
};

#endif // PIN_H
