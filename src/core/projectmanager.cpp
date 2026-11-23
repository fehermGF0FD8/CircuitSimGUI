#include "projectmanager.h"

#include "circuit.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QDebug>

bool ProjectManager::saveProject(const QList<Circuit*>& circuits, const QString& filePath) {
    QJsonArray circuitsArray;
    for (const Circuit* circuit : circuits) {
        if (circuit) {
            circuitsArray.append(circuit->toJson());
        }
    }

    QJsonObject rootObj;
    rootObj["circuits"] = circuitsArray;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }
    file.write(QJsonDocument(rootObj).toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "Successfully saved to:" << filePath;
    return true;
}

QList<Circuit*> ProjectManager::loadProject(const QString& filePath) {
    QList<Circuit*> loadedCircuits;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return loadedCircuits;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject rootObj = doc.object();

    if (!rootObj.contains("circuits") || !rootObj["circuits"].isArray()) {
        qWarning() << "Invalid project file format:" << filePath;
        return loadedCircuits;
    }

    QJsonArray circuitsArray = rootObj["circuits"].toArray();

    // loading circuits and nodes
    QMap<QString, Circuit*> circuitReg;
    QList<QJsonObject> circuitJsons;

    for (const QJsonValue& val : std::as_const(circuitsArray)) {
        QJsonObject cObj = val.toObject();
        QString name = cObj["circuitName"].toString();
        Circuit* c = new Circuit(name);
        c->loadNodesFromJson(cObj);

        loadedCircuits.append(c);
        circuitReg.insert(name, c);
        circuitJsons.append(cObj);
    }

    // loading components and wires
    for (int i = 0; i < loadedCircuits.size(); ++i) {
        loadedCircuits[i]->loadCompsAndWiresFromJson(circuitJsons[i], circuitReg);
    }

    qDebug() << "Successfully loaded project from:" << filePath;
    return loadedCircuits;
}
