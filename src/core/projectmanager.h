#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QList>
#include <QString>

class Circuit;

namespace ProjectManager {
    bool saveProject(const QList<Circuit*>& circuits, const QString& filePath);
    QList<Circuit*> loadProject(const QString& filePath);
}

#endif // PROJECTMANAGER_H
