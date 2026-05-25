#include "projectmanager.h"

#include "circuit.h"

#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>

bool ProjectManager::saveProject(const Circuit* circuit, const QString& filePath) {
    if (!circuit) {
        qWarning() << "Cannot save a null pointer!";
        return false;
    }

    QJsonObject jsonCircuit = circuit->toJson();

    QJsonDocument doc(jsonCircuit);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "Successfully saved to:" << filePath;
    return true;
}

Circuit* ProjectManager::loadProject(const QString& filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return nullptr;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);

    QJsonObject jsonCircuit = doc.object();

    QString circuitName = jsonCircuit["circuitName"].toString();

    Circuit* loadedCircuit = new Circuit(circuitName);

    loadedCircuit->fromJson(jsonCircuit);

    qDebug() << "Successfully loaded project from:" << filePath;

    return loadedCircuit;
}
