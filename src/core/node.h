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
class Wire;

class Node: public QGraphicsObject
{
    Q_OBJECT

public:
    explicit Node(const QString& idParam,
                  NodeType typeParam,
                  QGraphicsItem* parent = nullptr);
    virtual ~Node();

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem*,
               QWidget*) override;

    void setID(const QString& nodeIDParam) {id = nodeIDParam;}
    QString getID() const {return id;}

    void setNetName(const QString& netNameParam) {
        if (netName != netNameParam) {
            netName = netNameParam;
            update();
        }
    }

    QString getNetName() const {
        return netName;
    }

    NodeType getType() const {return type;}

    void addWire(Wire* wireParam) {
        if (!connectedWires.contains(wireParam)) {
            connectedWires.append(wireParam);
        }
    }

    void removeWire(Wire* wireParam) {
        connectedWires.removeAll(wireParam);
    }

    QList<Wire*> getWires() const {return connectedWires;}

private:
    QString id;
    QString netName;
    NodeType type;
    QList<Wire*> connectedWires;
    bool isHovered;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
};

#endif // NODE_H
