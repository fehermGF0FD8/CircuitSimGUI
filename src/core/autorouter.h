#ifndef AUTOROUTER_H
#define AUTOROUTER_H

#include "snapping.h"

#include <QList>
#include <QPointF>
#include <QPoint>
#include <QGraphicsScene>
#include <QVector>
#include <memory>

class Pin;

// Direction for moving across grid points
// Crucial for determining turn penalty
enum class Direction { None, Up, Down, Left, Right };

// Specifies the state of a grid point
struct AStarNode {
    QPoint pos;
    int g; // Cost of route from the start "node"
    int h; // Heuristic approximation till the end "node", aka. Manhattan distance
    int f() const { return g + h; }


    std::shared_ptr<AStarNode> parent; // ptr to the previous "node" from which we came
    Direction dir; // direction from which we came

    AStarNode(QPoint p, int gP, int hP, std::shared_ptr<AStarNode> parentP, Direction dirP):
        pos(p),
        g(gP),
        h(hP),
        parent(parentP),
        dir(dirP)
    {}

    // Overloading the ">" operator to put the node with the lowest f-cost at the front of the priority queue
    bool operator>(const AStarNode& other) const {
        return f() > other.f();
    }
};

class AutoRouter {
public:
    explicit AutoRouter(QGraphicsScene* sceneP, int gridSizeP = Snapping::GRID_SIZE);

    void initRouting(const QPointF& startPos);

    QList<QPointF> findPath(const QPointF& startPos,
                            const QPointF& currentMousePos,
                            Direction startPED = Direction::None,
                            Direction endPED = Direction::None);

    Direction getPinExitDirection(Pin* pinP);
    Direction getOppositeDirection(Direction dirP);

private:
    QGraphicsScene* scene;
    int gridSize;
    QRectF sceneBounds;
    int gridWidth;
    int gridHeight;

    QVector<QVector<bool>> obstacleMap; // 2D grid representing obstacles

    void updateObstacleMap(const QPointF& startPos); // gathers bounding rects of components on the scene

    QPoint mapToGrid(const QPointF& scenePos) const;
    QPointF mapToScene(const QPoint& gridPos) const;

    int manhattanDistance(const QPoint& a, const QPoint& b) const;
    QList<QPointF> simplifyPath(const QList<QPoint>& rawGridPath) const;
};

#endif // AUTOROUTER_H
