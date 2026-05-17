#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QListWidgetItem;
class QPointF;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // Initializes all signal-slot connections for the UI elements
    void setupConnections();

private slots:
    // Sets up the UI for a new project workspace
    void createNewProject();
    // Opens a dialog to add a new circuit or subcircuit tab
    void addCircuit();
    // Handles the application exit logic
    void quitApp();
    // Closes a specific tab and manages memory cleanup
    void closeTab(int index);
    // Updates the status bar with current mouse coordinates and grid snapping
    void updateCursorInfo(const QPointF& position);
    // Toggles visibility of the component library based on active tabs
    void syncCompLib();
    // Adds the selected component to the active scene upon double-clicking
    void compLibItemDoubleClicked(QListWidgetItem* item);


};
#endif // MAINWINDOW_H
