#include "addcircuitdialog.h"
#include "ui_addcircuitdialog.h"

AddCircuitDialog::AddCircuitDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddCircuitDialog)
{
    ui->setupUi(this);
    ui->xSpinBox->setValue(0);
    ui->xSpinBox->setEnabled(false);
    ui->ySpinBox->setValue(0);
    ui->ySpinBox->setEnabled(false);
    ui->aSpinBox->setValue(0);
    ui->aSpinBox->setEnabled(false);
    ui->oSpinBox->setValue(0);
    ui->oSpinBox->setEnabled(false);
}

AddCircuitDialog::~AddCircuitDialog()
{
    delete ui;
}

CircuitData AddCircuitDialog::getResult() const {
    CircuitData data;
    data.circuitName = ui->circuitNameEdit->text();
    data.xNodesCount = ui->xSpinBox->value();
    data.yNodesCount = ui->ySpinBox->value();
    data.aNodesCount = ui->aSpinBox->value();
    data.oNodesCount = ui->oSpinBox->value();
    return data;
}

