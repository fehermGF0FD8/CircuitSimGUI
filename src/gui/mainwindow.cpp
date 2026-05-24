#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "addcircuitdialog.h"
#include "circuit.h"
#include "circuitview.h"
#include "snapping.h"

#include <QActionGroup>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPointF>
#include <QStatusBar>
#include <QString>
#include <QWidget>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("CircuitSimGUI");

    QActionGroup *modeGroup = new QActionGroup(this);
    modeGroup->addAction(ui->placeMode);
    modeGroup->addAction(ui->wireMode);
    modeGroup->setExclusive(true);

    ui->placeMode->setEnabled(false);
    ui->wireMode->setEnabled(false);

    ui->placeMode->setChecked(false);
    ui->wireMode->setChecked(false);

    // Safety lock: Force the QStackedWidget to the welcome screen on startup,
    // overriding the last saved state from Qt Designer.
    ui->stackedWidget->setCurrentIndex(0);

    // Safety lock 2: Ensure the workspace has exactly 0 tabs on startup
    ui->tabWidget->clear();

    ui->dockWidget->setVisible(false);

    setupConnections();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupConnections() {
    connect(ui->newProject, &QAction::triggered, this, &MainWindow::createNewProject);
    connect(ui->newCircuitOrSubcircuit, &QAction::triggered, this, &MainWindow::addCircuit);
    connect(ui->exitApplication, &QAction::triggered, this, &MainWindow::quitApp);
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &MainWindow::syncUiState);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::syncUiState);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::compLibItemDoubleClicked);
    connect(ui->placeMode, &QAction::toggled, this, &MainWindow::syncUiState);
    connect(ui->wireMode, &QAction::toggled, this, &MainWindow::syncUiState);
}

void MainWindow::createNewProject() {
    // Switch to the workspace view (index 1 contains the QTabWidget)
    ui->stackedWidget->setCurrentIndex(1);

    syncUiState();
}

void MainWindow::addCircuit() {
    AddCircuitDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        CircuitData data = dialog.getResult();
        if (data.circuitName.isEmpty()) {
            data.circuitName = "Unnamed Circuit";
        }

        Circuit* circuit = new Circuit(data.circuitName);
        CircuitView* circuitView = new CircuitView(circuit, ui->tabWidget);
        int newTabIndex = ui->tabWidget->addTab(circuitView, data.circuitName);
        ui->tabWidget->setCurrentIndex(newTabIndex);

        syncUiState();
        ui->placeMode->setChecked(true);
        syncUiState();

        connect(circuitView, &CircuitView::mouseMoved, this, &MainWindow::updateCursorInfo);
    }
}

void MainWindow::quitApp() {
    // TODO: Implement "Unsaved Changes" check here!
    // Need to add a condition to check if the current project (new or loaded)
    // has been modified. If yes, show a warning dialog (Save/Discard/Cancel)
    // before allowing the window to close.

    // Close the main window (triggers application exit)
    this->close();
}

void MainWindow::closeTab(int index) {
    // Get the pointer to the widget (the workspace) at the clicked tab's index
    QWidget *tabToClose = ui->tabWidget->widget(index);

    // Remove the tab visually from the QTabWidget bar
    ui->tabWidget->removeTab(index);

    // Delete the actual widget from memory to prevent memory leaks
    delete tabToClose;

    syncUiState();
}

void MainWindow::updateCursorInfo(const QPointF& position) {
    QPointF snappedPos = Snapping::snapToGrid(position);

    QString text = QString("X: %1, Y: %2 | Snapped: [%3, %4]")
                    .arg(static_cast<int>(position.x()))
                    .arg(static_cast<int>(position.y()))
                    .arg(static_cast<int>(snappedPos.x()))
                    .arg(static_cast<int>(snappedPos.y()));

    statusBar()->showMessage(text);
}

void MainWindow::syncUiState() {
    bool inWorkspaceView = (ui->stackedWidget->currentIndex() == 1);
    bool hasActiveTabs = (ui->tabWidget->count() > 0);
    bool isPlaceModeActive = ui->placeMode->isChecked();

    ui->dockWidget->setVisible(inWorkspaceView && hasActiveTabs && isPlaceModeActive);

    ui->saveProject->setEnabled(inWorkspaceView);
    ui->exportProject->setEnabled(inWorkspaceView);
    ui->newCircuitOrSubcircuit->setEnabled(inWorkspaceView);
    ui->newController->setEnabled(inWorkspaceView);

    ui->placeMode->setEnabled(hasActiveTabs);
    ui->wireMode->setEnabled(hasActiveTabs);

    if (!hasActiveTabs) {
        ui->placeMode->setChecked(false);
        ui->wireMode->setChecked(false);

        statusBar()->clearMessage();
    }

    if (hasActiveTabs) {
        CircuitView* currentView = qobject_cast<CircuitView*>(ui->tabWidget->currentWidget());

        if (currentView) {
            if (isPlaceModeActive) {
                currentView->setViewMode(CircuitView::PlaceMode);
            } else {
                currentView->setViewMode(CircuitView::WireMode);
            }
        }
    }
}

void MainWindow::compLibItemDoubleClicked(QListWidgetItem* item) {
    CircuitView* currentCircuitView = qobject_cast<CircuitView*>(ui->tabWidget->currentWidget());

    if (!currentCircuitView || !currentCircuitView->scene()) return;

    QPoint centerOfView = currentCircuitView->viewport()->rect().center();
    QPointF scenePos = currentCircuitView->mapToScene(centerOfView);

    QString text = item->text();

    if (text == "N-type Node") {
        currentCircuitView->addNodeAt(NodeType::N, scenePos);
        return;
    }
    else if (text == "B-type Node") {
        currentCircuitView->addNodeAt(NodeType::B, scenePos);
        return;
    }
    else if (text == "--- Nodes ---") {
        return;
    }

    ComponentType type;
    if (item->text() == "Resistor") type = ComponentType::Resistor;
    else if (item->text() == "Conductance") type = ComponentType::Conductance;
    else if (item->text() == "Capacitor") type = ComponentType::Capacitor;
    else if (item->text() == "Current Source") type = ComponentType::CurrentSource;
    else if (item->text() == "Voltage Source") type = ComponentType::VoltageSource;
    //...
    else return;
    currentCircuitView->addComponentAt(type, scenePos);
}

