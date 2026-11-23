#ifndef IDMANAGER_H
#define IDMANAGER_H

#include <QMap>
#include <QString>

class IDmanager
{
public:
    explicit IDmanager() = default;
    QString getNextID(const QString& prefix);
    // registering already existing ids after loading a circuit in order to avoid name collision
    void registerID (const QString& idP);
    void resetPrefix(const QString& prefix);

private:
    QMap<QString, int> idCounters;
};

#endif // IDMANAGER_H
