#ifndef SUBCIRCUITCOMPONENT_H
#define SUBCIRCUITCOMPONENT_H

#include "component.h"
#include "snapping.h"

#include <QMap>

class Circuit;
class Node;

class SubcircuitComponent : public Component
{
    Q_OBJECT
public:
    explicit SubcircuitComponent(const QString& idParam,
                                 Circuit* rCircuit,
                                 QGraphicsItem* parent = nullptr);
    ~SubcircuitComponent() override = default;

    Circuit* getReferencedCircuit() const { return rCircuit; }

    QRectF boundingRect() const override;
    QRectF getObstacleRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    QJsonObject toJson() const override;

    void updatePinsAndGeometry();
    QList<Pin*> getObsoletePins() const;
    Node* getMappedNode(Pin* pinP) const {return pinToNodeMap.value(pinP, nullptr);}

private:
    Circuit* rCircuit;
    qreal boxWidth = 4 * Snapping::GRID_SIZE;
    qreal boxHeight = 3 * Snapping::GRID_SIZE;
    qreal leadLength = Snapping::GRID_SIZE;
    QList<Node*> leftNodes; // X, Y nodes on the left side of the box (normal external)
    QList<Node*> rightNodes; // A, O nodes on the right side (control external)
    QMap<Pin*, Node*> pinToNodeMap; // subcPin-extNode assignment for save/load/export
};

#endif // SUBCIRCUITCOMPONENT_H
