#ifndef NODE_H
#define NODE_H

#include "nodetype.h"

#include <QGraphicsObject>
#include <QList>
#include <QRectF>
#include <QString>
#include <QVariant>

class Pin;
class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
// class Wire;

class Node: public QGraphicsObject
{
    Q_OBJECT

public:
    explicit Node(const QString& idParam,
                  NodeType typeParam,
                  QGraphicsItem* parent = nullptr);
    virtual ~Node() = default;

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem*,
               QWidget*) override;

    void setID(const QString& nodeIDParam) {id = nodeIDParam;}
    QString getID() const {return id;}

    NodeType getType() const {return type;}

    /*
    void addPin(Pin* pinParam);
    void removePin(Pin* pinParam);
    QList<Pin*> getPins() const {return connectedPins;}
    */

    /*
    void addWire(Wire* wireParam);
    void removeWire(Wire* wireParam);
    QList<Wire*> getWires() const {return connectedWires;}
    */

    // void mergeWith(Node* nodeParam);

private:
    QString id;
    NodeType type;
    QList<Pin*> connectedPins;
    // QList<Wire*> connectedWires;
    bool isHovered;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

signals:
    void nodeClicked(Node* nodeParam);
};

#endif // NODE_H
