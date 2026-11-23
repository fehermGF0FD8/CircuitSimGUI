#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "circuit.h"
#include "circuitview.h"
#include "projectmanager.h"
#include "snapping.h"
#include "subcircuitcomponent.h"

#include <QActionGroup>
#include <QCloseEvent>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QInputDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPointF>
#include <QSet>
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

QList<Circuit*> MainWindow::getAllOpenCircuits() const {
    QList<Circuit*> circuits;
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        if (auto* view = qobject_cast<CircuitView*>(ui->tabWidget->widget(i))) {
            if (view->getCircuitModel()) {
                circuits.append(view->getCircuitModel());
            }
        }
    }
    return circuits;
}

void MainWindow::clearWorkspace() {
    for (Circuit* c : getAllOpenCircuits()) {
        c->disconnect();
    }
    while (ui->tabWidget->count() > 0) {
        QWidget* w = ui->tabWidget->widget(0);
        ui->tabWidget->removeTab(0);
        delete w;
    }
    syncUiState();
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
        MainWindow::addCircuit();
    }
}

/*
 * TODO:
 * this action saves a single circuit document currently;
 * the user must manually save all related circuits into the same directory
 */
void MainWindow::saveProject() {
    const QList<Circuit*> circuits = getAllOpenCircuits();
    if (circuits.isEmpty()) return;

    QString savePath = QFileDialog::getSaveFileName(
        this, tr("Save Project"), "project.json", tr("JSON files (*.json)"));
    if (savePath.isEmpty()) return;

    ProjectManager::saveProject(circuits, savePath);
}

void MainWindow::setupNewTab(CircuitView* view, const QString& tabName) {
    int index = ui->tabWidget->addTab(view, tabName);
    ui->tabWidget->setCurrentIndex(index);

    syncUiState();
    ui->placeMode->setChecked(true);
    syncUiState();

    connect(view, &CircuitView::mouseMoved, this, &MainWindow::updateCursorInfo);
    connect(view->getCircuitModel(), &Circuit::extNodesChanged,
            this, &MainWindow::refreshSubcircuitLists);
    refreshSubcircuitLists();
}

/*
 * TODO:
 * currently loading a single .json file into a new tab;
 * the action does not handle automatic workspace cleanup or multi-file project loading;
 * cross-directory files can be opened side-by-side
 */
void MainWindow::loadProject() {
    QString loadPath = QFileDialog::getOpenFileName(
        this, tr("Load Project"), "", tr("JSON files (*.json)"));
    if (loadPath.isEmpty()) return;

    const QList<Circuit*> loadedCircuits = ProjectManager::loadProject(loadPath);
    if (loadedCircuits.isEmpty()) return;

    clearWorkspace();
    ui->stackedWidget->setCurrentIndex(1);

    for (Circuit* c : loadedCircuits) {
        CircuitView* circuitView = new CircuitView(c, ui->tabWidget);
        setupNewTab(circuitView, c->getName());
    }
    refreshSubcircuitLists();
}

void MainWindow::addCircuit() {
    bool ok = false;
    QString circuitName = QInputDialog::getText(
        this, tr("New Circuit/Subcircuit"), tr("Circuit/Subcircuit Name:"), QLineEdit::Normal, QString(), &ok);

    if (!ok) return;

    circuitName = circuitName.trimmed();
    const QList<Circuit*> existing = getAllOpenCircuits();

    if (circuitName.isEmpty()) {
        int idx = 1;
        do {
            circuitName = QString("Unnamed_%1").arg(idx++);
        } while (std::any_of(existing.begin(), existing.end(),
                             [&](Circuit* c) { return c->getName() == circuitName; }));
    }
    else {
        static const QSet<QString> reservedPrefixes = {
            "R", "G", "C", "I", "V", "N", "B", "X", "Y", "A", "O"
        };
        if (reservedPrefixes.contains(circuitName.toUpper())) {
            QMessageBox::warning(this, tr("Reserved Name"),
                                 tr("'%1' is a reserved component/node prefix. Choose another name.").arg(circuitName));
            return;
        }
        for (Circuit* c : existing) {
            if (c->getName() == circuitName) {
                QMessageBox::warning(this, tr("Duplicate Name"),
                                     tr("A circuit named '%1' already exists in the project.").arg(circuitName));
                return;
            }
        }
    }

    Circuit* circuit = new Circuit(circuitName);
    CircuitView* circuitView = new CircuitView(circuit, ui->tabWidget);
    setupNewTab(circuitView, circuitName);
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
    if (auto* viewToClose = qobject_cast<CircuitView*>(ui->tabWidget->widget(index))) {
        Circuit* target = viewToClose->getCircuitModel();
        for (Circuit* other : getAllOpenCircuits()) {
            if (other != target && other->dependsOn(target)) {
                QMessageBox::warning(this, tr("Subcircuit in Use"),
                                     tr("Cannot close '%1' because it is instantiated inside '%2'.\n"
                                        "Delete its instances first!").arg(target->getName(), other->getName()));
                return;
            }
        }
    }

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
    refreshSubcircuitLists();
}

void MainWindow::compLibItemDoubleClicked(QListWidgetItem* item) {
    CircuitView* currentCircuitView = qobject_cast<CircuitView*>(ui->tabWidget->currentWidget());

    if (!currentCircuitView || !currentCircuitView->scene()) return;

    QPoint centerOfView = currentCircuitView->viewport()->rect().center();
    QPointF scenePos = currentCircuitView->mapToScene(centerOfView);

    QVariant userData = item->data(Qt::UserRole);
    if (Circuit* subCircuit = userData.value<Circuit*>()) {
        currentCircuitView->addSubcircuitAt(subCircuit, scenePos);
        return;
    }

    QString text = item->text();

    NodeType nType;
    bool isNode = true;
    if (text == "N-type Node") nType = NodeType::N;
    else if (text == "B-type Node") nType = NodeType::B;
    else if (text == "X-type Node") nType = NodeType::X;
    else if (text == "Y-type Node") nType = NodeType::Y;
    else if (text == "A-type Node") nType = NodeType::A;
    else if (text == "O-type Node") nType = NodeType::O;
    else isNode = false;

    if (isNode) {
        currentCircuitView->addNodeAt(nType, scenePos);
        return;
    }

    ComponentType cType;
    if (text == "Resistor") cType = ComponentType::Resistor;
    else if (text == "Conductance") cType = ComponentType::Conductance;
    else if (text == "Capacitor") cType = ComponentType::Capacitor;
    else if (text == "Current Source") cType = ComponentType::CurrentSource;
    else if (text == "Voltage Source") cType = ComponentType::VoltageSource;
    //...
    else return;
    currentCircuitView->addComponentAt(cType, scenePos);
}

void MainWindow::refreshSubcircuitLists() {
    const QList<Circuit*> allCircuits = getAllOpenCircuits();

    // contextMenu
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        auto* view = qobject_cast<CircuitView*>(ui->tabWidget->widget(i));
        if (!view || !view->getCircuitModel()) continue;

        Circuit* targetCircuit = view->getCircuitModel();
        QList<Circuit*> validSubs;
        for (Circuit* candidate : allCircuits) {
            if (candidate && candidate->isSubcircuit() && !candidate->dependsOn(targetCircuit)) {
                validSubs.append(candidate);
            }
        }
        view->setAvailableSubcircuits(validSubs);
    }

    // listWidget
    for (int i = ui->listWidget->count() - 1; i >= 0; --i) {
        if (ui->listWidget->item(i)->data(Qt::UserRole).isValid()) {
            delete ui->listWidget->takeItem(i);
        }
    }

    auto* activeView = qobject_cast<CircuitView*>(ui->tabWidget->currentWidget());
    if (!activeView || !activeView->getCircuitModel()) return;

    Circuit* activeCircuit = activeView->getCircuitModel();
    QList<Circuit*> activeValidSubs;
    for (Circuit* candidate : allCircuits) {
        if (candidate && candidate->isSubcircuit() && !candidate->dependsOn(activeCircuit)) {
            activeValidSubs.append(candidate);
        }
    }

    if (!activeValidSubs.isEmpty()) {
        auto* headerItem = new QListWidgetItem("   --- Subcircuit ---");
        headerItem->setData(Qt::UserRole, "header");
        headerItem->setFlags(Qt::ItemIsEnabled);
        ui->listWidget->addItem(headerItem);

        for (Circuit* sub : activeValidSubs) {
            auto* subItem = new QListWidgetItem(sub->getName());
            subItem->setData(Qt::UserRole, QVariant::fromValue(sub));
            ui->listWidget->addItem(subItem);
        }
    }
}
