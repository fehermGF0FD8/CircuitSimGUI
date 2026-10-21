#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "addcircuitdialog.h"
#include "circuit.h"
#include "circuitview.h"
#include "projectmanager.h"
#include "snapping.h"

#include <QActionGroup>
#include <QCloseEvent>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
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

void MainWindow::closeEvent(QCloseEvent* event) {
    if (ui->tabWidget->count() == 0) {
        event->accept();
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "Exit Application",
                                 "Are you sure you want to exit?\nPlease make sure you have saved all open circuits!",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

// !!!
void MainWindow::setupConnections() {
    connect(ui->newProject, &QAction::triggered, this, &MainWindow::createNewProject);
    connect(ui->saveProject, &QAction::triggered, this, &MainWindow::saveProject);
    connect(ui->loadProject, &QAction::triggered, this, &MainWindow::loadProject);


    connect(ui->newCircuitOrSubcircuit,
            &QAction::triggered,
            this,
            &MainWindow::addCircuit);


    connect(ui->exitApplication, &QAction::triggered, this, &MainWindow::quitApp);
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &MainWindow::syncUiState);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::syncUiState);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::compLibItemDoubleClicked);
    connect(ui->placeMode, &QAction::toggled, this, &MainWindow::syncUiState);
    connect(ui->wireMode, &QAction::toggled, this, &MainWindow::syncUiState);
}

/*
 * TODO:
 * createNewProject currently just opens the workspace with a blank tab;
 * folder management is not implemented yet
 */
void MainWindow::createNewProject() {
    // Switch to the workspace view (index 1 contains the QTabWidget)
    ui->stackedWidget->setCurrentIndex(1);

    syncUiState();

    if (ui->tabWidget->count() == 0) {
        AddCircuitDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            CircuitData data = dialog.getResult();
            if (data.circuitName.isEmpty()) {
                data.circuitName = "Unnamed Circuit";
            }

            Circuit* circuit = new Circuit(data.circuitName);
            CircuitView* circuitView = new CircuitView(circuit, ui->tabWidget);
            setupNewTab(circuitView, data.circuitName);
        }
    }
}

/*
 * TODO:
 * this action saves a single circuit document currently;
 * the user must manually save all related circuits into the same directory
 */
void MainWindow::saveProject() {
    CircuitView* currentView = dynamic_cast<CircuitView*>(ui->tabWidget->currentWidget());
    if (!currentView) return;

    Circuit* currentCircuit = currentView->getCircuitModel();

    QString defaultName = currentCircuit->getName() + ".json";

    QString savePath = QFileDialog::getSaveFileName(this,
                                                    "Save Project",
                                                    defaultName,
                                                    "JSON files (*.json)");

    if (savePath.isEmpty()) return;

    ProjectManager::saveProject(currentCircuit, savePath);
}

void MainWindow::setupNewTab(CircuitView* view, const QString& tabName) {
    int index = ui->tabWidget->addTab(view, tabName);
    ui->tabWidget->setCurrentIndex(index);

    syncUiState();
    ui->placeMode->setChecked(true);
    syncUiState();

    connect(view, &CircuitView::mouseMoved, this, &MainWindow::updateCursorInfo);
}

/*
 * TODO:
 * currently loading a single .json file into a new tab;
 * the action does not handle automatic workspace cleanup or multi-file project loading;
 * cross-directory files can be opened side-by-side
 */
void MainWindow::loadProject() {
    QString loadPath = QFileDialog::getOpenFileName(this,
                                                    "Load Project",
                                                    "",
                                                    "JSON files (*.json)");
    if (loadPath.isEmpty()) return;

    Circuit* loadedCircuit = ProjectManager::loadProject(loadPath);

    if (loadedCircuit) {
        ui->stackedWidget->setCurrentIndex(1);
        CircuitView* circuitView = new CircuitView(loadedCircuit, ui->tabWidget);
        setupNewTab(circuitView, loadedCircuit->getName());
    }
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
        setupNewTab(circuitView, data.circuitName);
    }
}

/*
 * TODO:
 * implement "Unsaved Changes" check here;
 * need to add a condition to check if the current project has been modified;
 * in case of being modified, show a warning dialog before allowing the window to close
 */
void MainWindow::quitApp() {
    // Close the main window (triggers application exit)
    this->close();
}

/*
 * TODO:
 * implement "Unsaved Changes" check here;
 * need to add a condition to check if the current circuit has been modified;
 * in case of being modified, show a warning dialog before allowing the tab to close
 */
void MainWindow::closeTab(int index) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Close Tab",
                                  "Are you sure you want to close this circuit?\nIf you haven't saved, all changes will be lost!",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    // Get the pointer to the widget (the workspace) at the clicked tab's index
    QWidget *tabToClose = ui->tabWidget->widget(index);

    // Remove the tab visually from the QTabWidget bar
    ui->tabWidget->removeTab(index);

    // Delete the actual widget from memory to prevent memory leaks
    if (tabToClose) {
        tabToClose->deleteLater();
    }

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
