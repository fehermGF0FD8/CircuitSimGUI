#ifndef IDMANAGER_H
#define IDMANAGER_H

#include <QMap>
#include <QString>

class IDmanager
{
public:
    explicit IDmanager() = default;
    QString getNextID(const QString& prefix);
    void reset() {idCounters.clear();}

private:
    QMap<QString, int> idCounters;
};

#endif // IDMANAGER_H
