#include "idmanager.h"

#include <QStringList>

QString IDmanager::getNextID(const QString& prefix) {
    if (!idCounters.contains(prefix)) {
        idCounters[prefix] = 1;
    }

    QString newID = prefix + "_" + QString::number(idCounters[prefix]);
    idCounters[prefix]++;
    return newID;
}

void IDmanager::registerID(const QString& idP) {
    int sepIdx = idP.lastIndexOf('_');
    if (sepIdx > 0 && sepIdx < idP.length() - 1) {
        QString prefix = idP.left(sepIdx);
        bool ok = false;
        int num = idP.mid(sepIdx + 1).toInt(&ok);
        if (ok && num >= idCounters.value(prefix, 1)) {
            idCounters[prefix] = num + 1;
        }
    }
}

void IDmanager::resetPrefix(const QString& prefix) {
    idCounters.remove(prefix);
}
