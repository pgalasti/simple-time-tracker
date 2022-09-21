#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "taskdate.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void LoadSettings();

    bool CreateDefaultSettings();

    bool CreateDefaultStructure();

    // Applies the model onto the UI
    void ApplyModel();

    // Closes current model
    void CloseModel();

    // Saves the model state
    bool SaveModelState();

    // Loads the model state from file
    bool LoadModelState(const std::string& path);

    // Opens time task file into UI
    bool DoOpenFile();

    // Closes task file
    bool DoCloseFile();

    // Shuts down window.
    void ShutdownWindow();

    void CheckIfTracking();

private slots:
    void on_menuActionOpen_triggered();

    void on_actionExit_triggered();

    void on_addTaskButton_clicked();

    void on_actionClose_triggered();

    void on_stopTaskButton_clicked();

    void on_taskTree_doubleClicked(const QModelIndex &index);

    void on_taskTree_customContextMenuRequested(const QPoint &pos);

    void prepareMenu( const QPoint & pos );

    void EditCurrentItem();

    void DeleteCurrentItem();

    void on_actionSave_triggered();

    void on_actionConfiguration_triggered();

    void on_action_About_triggered();

private:

    struct Settings
    {

        std::string SaveDirectory;

        Settings()
        {
            this->Initialize();
        }

        void Initialize()
        {
            this->SaveDirectory = "";
        }
    };


    Ui::MainWindow *ui;

    void CapRunningTask();

    std::string ConvertDateToFileName(const QDate& date) const;
    TaskDateModel m_TaskDate;
    TaskModel m_CurrentTask;
    bool m_bTrackingTask;
    bool m_bChangeMade;
    std::string m_CurrentSaveFile;


    Settings m_Settings;
};

#endif // MAINWINDOW_H
