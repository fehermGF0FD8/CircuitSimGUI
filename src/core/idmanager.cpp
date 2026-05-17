#include "idmanager.h"

QString IDmanager::getNextID(const QString& prefix) {
    if (!idCounters.contains(prefix)) {
        idCounters[prefix] = 1;
    }

    QString newID = prefix + "_" + QString::number(idCounters[prefix]);
    idCounters[prefix]++;
    return newID;
}
