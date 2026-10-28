#ifndef WIRE_H
#define WIRE_H

#include <QGraphicsObject>

#include <QColor>
#include <QJsonObject>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>

class Node;
class Pin;
class QGraphicsItem;
class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QPainter;
class QPainterPath;
class QStyleOptionGraphicsItem;
class QWidget;

class Wire: public QGraphicsObject
{
    Q_OBJECT

public:
    explicit Wire(QGraphicsItem* parent = nullptr);
    virtual ~Wire();

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem*,
               QWidget*) override;

    void setStartPos(const QPointF& startPosParam);
    QPointF getStartPos() const {return startPos;}
    void setEndPos(const QPointF& endPosParam);
    QPointF getEndPos() const {return endPos;}
    void setPathPoints(const QList<QPointF>& pathPointsP);

    void setStartItem(Node* startNodeParam);
    Node* getStartNode() const {return startNode;}
    void setStartItem(Pin* startPinParam);
    Pin* getStartPin() const {return startPin;}
    void setEndItem(Node* endNodeParam);
    Node* getEndNode() const {return endNode;}
    void setEndItem(Pin* endPinParam);
    Pin* getEndPin() const {return endPin;}

    // !!!
    void setIsHorizontalFirst(bool isHorizontalFirstParam) {
        isHorizontalFirst = isHorizontalFirstParam;
    }
    bool getIsHorizontalFirst() const {
        return isHorizontalFirst;
    }

    void toggleRoutingMode();
    void updatePosition();

    // !!!
    QJsonObject toJson() const;

private:
    QPointF startPos;
    QPointF endPos;
    QList<QPointF> pathPoints;

    Node* startNode = nullptr;
    Pin* startPin = nullptr;
    Node* endNode = nullptr;
    Pin* endPin = nullptr;

    bool isHovered;
    bool isHorizontalFirst = true;

    QColor getBaseColor() const;
    QPainterPath getPath() const;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
};

#endif // WIRE_H
