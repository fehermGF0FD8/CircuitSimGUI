#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QString>

class Circuit;

namespace ProjectManager {
    bool saveProject(const Circuit* circuit, const QString& filePath);
    Circuit* loadProject(const QString& filePath);
}

#endif // PROJECTMANAGER_H
