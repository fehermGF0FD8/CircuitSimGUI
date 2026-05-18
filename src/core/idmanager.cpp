#include "idmanager.h"

QString IDmanager::getNextID(const QString& prefix) {
    if (!idCounters.contains(prefix)) {
        idCounters[prefix] = 1;
    }

    QString newID = prefix + "_" + QString::number(idCounters[prefix]);
    idCounters[prefix]++;
    return newID;
}

void IDmanager::resetPrefix(const QString& prefix) {
    idCounters.remove(prefix);
}

void IDmanager::resetAllComponents() {
    resetPrefix("R");
    resetPrefix("G");
    resetPrefix("C");
    resetPrefix("I");
    resetPrefix("V");
    //...
}

void IDmanager::resetAllInternalNodes() {
    resetPrefix("N");
    resetPrefix("B");
}
