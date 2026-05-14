#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "addcircuitdialog.h"
#include "circuitview.h"

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

    ui->dockWidget->setVisible(false);

    // Safety lock: Force the QStackedWidget to the welcome screen on startup,
    // overriding the last saved state from Qt Designer.
    ui->stackedWidget->setCurrentIndex(0);

    // Safety lock 2: Ensure the workspace has exactly 0 tabs on startup
    ui->tabWidget->clear();

    setupConnections();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupConnections() {
    connect(ui->newProject, &QAction::triggered, this, &MainWindow::createNewProject);
    connect(ui->newCircuitOrSubcircuit, &QAction::triggered, this, &MainWindow::addCircuit);
    connect(ui->exitApplication, &QAction::triggered, this, &MainWindow::quitApp);
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &MainWindow::syncCompLib);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::syncCompLib);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::compLibItemDoubleClicked);
}

void MainWindow::createNewProject() {
    // Switch to the workspace view (index 1 contains the QTabWidget)
    ui->stackedWidget->setCurrentIndex(1);

    // Enable project-specific actions in the menu and toolbar
    ui->saveProject->setEnabled(true);
    ui->exportProject->setEnabled(true);
    ui->newCircuitOrSubcircuit->setEnabled(true);
    ui->newController->setEnabled(true);
}

void MainWindow::addCircuit() {
    AddCircuitDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        CircuitData data = dialog.getResult();
        if (data.circuitName.isEmpty()) {
            data.circuitName = "Unnamed Circuit";
        }

        CircuitView* circuitView = new CircuitView(ui->tabWidget);
        circuitView->setIDManager(&idManager);
        int newTabIndex = ui->tabWidget->addTab(circuitView, data.circuitName);
        ui->tabWidget->setCurrentIndex(newTabIndex);

        ui->newProject->setEnabled(false);
        ui->newCircuitOrSubcircuit->setEnabled(false);
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

    if (ui->tabWidget->count() == 0) {
        statusBar()->clearMessage();
    }
}

void MainWindow::updateCursorInfo(const QPointF& position) {
    const int gridSize = 20;
    int snappedX = qRound(position.x() / gridSize) * gridSize;
    int snappedY = qRound(position.y() / gridSize) * gridSize;

    QString text = QString("X: %1, Y: %2 | Snapped: [%3, %4]")
                    .arg(static_cast<int>(position.x()))
                    .arg(static_cast<int>(position.y()))
                    .arg(snappedX)
                    .arg(snappedY);

    statusBar()->showMessage(text);
}

void MainWindow::syncCompLib() {
    bool inWorkspaceView = (ui->stackedWidget->currentIndex() == 1);;
    bool hasActiveTabs = (ui->tabWidget->count() > 0);

    ui->dockWidget->setVisible(inWorkspaceView && hasActiveTabs);
}

void MainWindow::compLibItemDoubleClicked(QListWidgetItem* item) {
    CircuitView* currentCircuitView = qobject_cast<CircuitView*>(ui->tabWidget->currentWidget());

    if (!currentCircuitView || !currentCircuitView->scene()) return;

    QPoint centerOfView = currentCircuitView->viewport()->rect().center();
    QPointF scenePos = currentCircuitView->mapToScene(centerOfView);

    ComponentType type;
    if (item->text() == "Resistor") type = ComponentType::Resistor;
    else if (item->text() == "Capacitor") type = ComponentType::Capacitor;
    //...
    else return;
    currentCircuitView->addComponentAt(type, scenePos);
}
