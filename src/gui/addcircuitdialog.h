#ifndef ADDCIRCUITDIALOG_H
#define ADDCIRCUITDIALOG_H

#include <QDialog>

#include <QString>

namespace Ui {
class AddCircuitDialog;
}

// Container for circuit configuration data collected from the dialog
struct CircuitData {
    QString circuitName;
    int xNodesCount;
    int yNodesCount;
    int aNodesCount;
    int oNodesCount;
};

class AddCircuitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddCircuitDialog(QWidget *parent = nullptr);
    ~AddCircuitDialog();
    // Returns the user's input as a CircuitData object
    CircuitData getResult() const;

private:
    Ui::AddCircuitDialog *ui;
};

#endif // ADDCIRCUITDIALOG_H
