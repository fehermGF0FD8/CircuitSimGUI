#include "autorouter.h"

#include "builtincomponent.h"
#include "node.h"
#include "pin.h"
#include "wire.h"

#include <QGraphicsItem>
#include <QSet>
#include <cmath>
#include <queue>

AutoRouter::AutoRouter(QGraphicsScene* sceneP, int gridSizeP):
    scene(sceneP),
    gridSize(gridSizeP)
{}

void AutoRouter::initRouting() {
    if (!scene) return;

    sceneBounds = scene->sceneRect();

    gridWidth = std::ceil(sceneBounds.width() / gridSize);
    gridHeight = std::ceil(sceneBounds.height() / gridSize);

    updateObstacleMap();
}

// Executes the A* algorithm
QList<QPointF> AutoRouter::findPath(const QPointF& startPos,
                                    const QPointF& currentMousePos,
                                    Direction startPED,
                                    Direction endPED)
{
    QPoint startGrid = mapToGrid(startPos);
    QPoint endGrid = mapToGrid(currentMousePos);

    if (startGrid == endGrid) {
        return QList<QPointF>{ startPos };
    }

    // Min-heap for frontier nodes prioritized by f-cost, and a set for closed/visited cells
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openList;
    QSet<QPoint> closedList;

    openList.push(AStarNode(startGrid, 0, manhattanDistance(startGrid, endGrid), nullptr, Direction::None));

    std::shared_ptr<AStarNode> current = nullptr;
    bool pathFound = false;

    const QPoint neighbors[4] = { QPoint(0, -1), QPoint(0, 1), QPoint(-1, 0), QPoint(1, 0) };
    const Direction dirs[4] = { Direction::Up, Direction::Down, Direction::Left, Direction::Right };
    const int turnPenalty = 5;

    // A*
    while (!openList.empty()) {
        // pop the lowest f-cost
        current = std::make_shared<AStarNode>(openList.top());
        openList.pop();

        if (current->pos == endGrid) {
            pathFound = true;
            break;
        }

        // Check if current node is already evaluated
        if (closedList.contains(current->pos)) continue;
        closedList.insert(current->pos);

        // Expand all orthogonal neighbours
        for (int i = 0; i < 4; ++i) {
            if (current->pos == startGrid && startPED != Direction::None) {
                if (dirs[i] == getOppositeDirection(startPED)) {
                    continue;
                }
            }

            QPoint nextPos = current->pos + neighbors[i];

            // Checking if neighbour is inside the grid boundaries
            if (nextPos.x() < 0 || nextPos.x() >= gridWidth ||
                nextPos.y() < 0 || nextPos.y() >= gridHeight) {
                continue;
            }

            if (nextPos == endGrid && endPED != Direction::None) {
                if (dirs[i] == endPED) {
                    continue;
                }
            }

            // Leaving obstacles out, but allowing landing on end grid
            if (obstacleMap[nextPos.x()][nextPos.y()] && nextPos != endGrid) {
                continue;
            }

            // Skipping closed nodes
            if (closedList.contains(nextPos)) continue;

            // Calculate g-cost
            int newG = current->g + 1;
            if (current->dir != Direction::None && current->dir != dirs[i]) {
                newG += turnPenalty;
            }

            // Estimation for the remaining distance
            int h = manhattanDistance(nextPos, endGrid);
            openList.push(AStarNode(nextPos, newG, h, current, dirs[i]));
        }
    }

    if (!pathFound) {
        return QList<QPointF>();
    }

    // Backtracking: target -> origin
    QList<QPoint> rawGridPath;
    while (current != nullptr) {
        rawGridPath.prepend(current->pos);
        current = current->parent;
    }

    return simplifyPath(rawGridPath);
}

// Builds 2D obstacle grid from scene items
void AutoRouter::updateObstacleMap() {
    obstacleMap.assign(gridWidth, QVector<bool>(gridHeight, false));

    // Marking obstacles on the scene
    for (QGraphicsItem* item : scene->items()) {

        // Filtering pins, nodes, already placed wires and startcomp. from obstacleMap
        if (item->parentItem() != nullptr) continue;
        if (dynamic_cast<Node*>(item) != nullptr) continue;
        if (dynamic_cast<Wire*>(item) != nullptr) continue;

        QRectF rect;
        if (BuiltInComponent* comp = dynamic_cast<BuiltInComponent*>(item)) {
            rect = comp->mapToScene(comp->getObstacleRect()).boundingRect();
        } else {
            rect = item->sceneBoundingRect();
        }

        int minX = std::max(0, mapToGrid(rect.topLeft()).x());
        int minY = std::max(0, mapToGrid(rect.topLeft()).y());
        int maxX = std::min(gridWidth - 1, mapToGrid(rect.bottomRight()).x());
        int maxY = std::min(gridHeight - 1, mapToGrid(rect.bottomRight()).y());

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                obstacleMap[x][y] = true;
            }
        }
    }
}

Direction AutoRouter::getPinExitDirection(Pin* pin) {
    if (!pin || !pin->parentItem()) {
        return Direction::None;
    }

    QPointF localOffset = pin->pos();

    QPointF sceneVector = pin->parentItem()->mapToScene(localOffset) - pin->parentItem()->mapToScene(QPointF(0, 0));

    if (std::abs(sceneVector.x()) > std::abs(sceneVector.y())) {
        return (sceneVector.x() > 0) ? Direction::Right : Direction::Left;
    } else {
        return (sceneVector.y() > 0) ? Direction::Down : Direction::Up;
    }
}

Direction AutoRouter::getOppositeDirection(Direction dirP) {
    switch (dirP) {
    case Direction::Up:    return Direction::Down;
    case Direction::Down:  return Direction::Up;
    case Direction::Left:  return Direction::Right;
    case Direction::Right: return Direction::Left;
    default:               return Direction::None;
    }
}

QList<QPointF> AutoRouter::simplifyPath(const QList<QPoint>& rawGridPath) const {
    QList<QPointF> simplified;
    if (rawGridPath.isEmpty()) return simplified;

    simplified.append(mapToScene(rawGridPath.first()));

    for (int i = 1; i < rawGridPath.size() - 1; ++i) {
        QPoint prev = rawGridPath[i - 1];
        QPoint curr = rawGridPath[i];
        QPoint next = rawGridPath[i + 1];

        if ((curr.x() - prev.x()) != (next.x() - curr.x()) ||
            (curr.y() - prev.y()) != (next.y() - curr.y())) {
            simplified.append(mapToScene(curr));
        }
    }

    simplified.append(mapToScene(rawGridPath.last()));
    return simplified;
}

// Converts to discrete grid coordinates
QPoint AutoRouter::mapToGrid(const QPointF& scenePos) const {
    QPointF snapped = Snapping::snapToGrid(scenePos);

    int x = qRound((snapped.x()-sceneBounds.left())/gridSize);
    int y = qRound((snapped.y()-sceneBounds.top())/gridSize);

    x = std::clamp(x, 0, gridWidth-1);
    y = std::clamp(y, 0, gridHeight-1);

    return QPoint(x, y);
}

// Converts to scene pixel coordinates
QPointF AutoRouter::mapToScene(const QPoint& gridPos) const {
    qreal x = sceneBounds.left() + (gridPos.x() * gridSize);
    qreal y = sceneBounds.top() + (gridPos.y() * gridSize);
    return QPointF(x, y);
}

// Calculates the heuristic Manhattan distance between two grid points
int AutoRouter::manhattanDistance(const QPoint& a, const QPoint& b) const {
    return std::abs(a.x() - b.x()) + std::abs(a.y() - b.y());
}
