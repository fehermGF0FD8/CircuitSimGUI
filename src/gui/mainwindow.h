#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QString>

class CircuitView;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Circuit;
class QCloseEvent;
class QListWidgetItem;
class QPointF;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::MainWindow *ui;

    // Initializes all signal-slot connections for the UI elements
    void setupConnections();

    QList<Circuit*> getAllOpenCircuits() const;
    void clearWorkspace();

private slots:
    // Sets up the UI for a new project workspace
    void createNewProject();
    // !!!
    void saveProject();

    void setupNewTab(CircuitView* view, const QString& tabName);
    // !!!
    void loadProject();
    // Opens a dialog to add a new circuit or subcircuit tab
    void addCircuit();
    // Handles the application exit logic
    void quitApp();
    // Closes a specific tab and manages memory cleanup
    void closeTab(int index);
    // Updates the status bar with current mouse coordinates and grid snapping
    void updateCursorInfo(const QPointF& position);
    // Toggles visibility of the component library based on active tabs
    void syncUiState();
    // Adds the selected component to the active scene upon double-clicking
    void compLibItemDoubleClicked(QListWidgetItem* item);

    void refreshSubcircuitLists();
};
#endif // MAINWINDOW_H
