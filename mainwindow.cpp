
#include "timetask.h"
#include "configurationdialog.h"
#include "about.h"

#include "taskdate.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"



#include<fstream>
#include<sstream>

#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_bTrackingTask(false),
    m_bChangeMade(false)
{
    ui->setupUi(this);

    LoadSettings();

    // Context menu for task tree
    ui->taskTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->taskTree,&QTreeWidget::customContextMenuRequested,this,&MainWindow::prepareMenu);


    m_CurrentTask.Initialize();

    LoadModelState(m_CurrentSaveFile);

    ApplyModel();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::LoadSettings()
{
    const QString binaryPath = QDir::currentPath();
    const QString settingsPath = binaryPath + QDir::separator() + "TimeTracker.ini";

    QFile settingsPathCheck(QDir::cleanPath(settingsPath));


    if(!settingsPathCheck.exists())
    {
        if(!CreateDefaultSettings())
        {
            QMessageBox::information(this, "Failure", "The application is unable to create new default settings!");
            return;
        }
    }

    std::string seperator = ((QString)QDir::separator()).toStdString();

    QSettings settings(settingsPath, QSettings::IniFormat);
    m_Settings.SaveDirectory = QDir::cleanPath(settings.value("Settings/saveDir").toString()).toStdString();
    if(m_Settings.SaveDirectory.substr(m_Settings.SaveDirectory.length(), 1) != seperator)
        m_Settings.SaveDirectory += seperator;

    if(!CreateDefaultStructure())
    {
        QMessageBox::information(this, "Failure", "The application is unable to create the default file structure!");
        return;
    }

    m_CurrentSaveFile = m_Settings.SaveDirectory;
    m_CurrentSaveFile += ConvertDateToFileName(QDate::currentDate());
    m_TaskDate.setDate(QDate::currentDate());
}

bool MainWindow::CreateDefaultSettings()
{
    const QString binaryPath = QDir::currentPath();
    const QString settingsPath = binaryPath + QDir::separator() + "TimeTracker.ini";

    QSettings settings(settingsPath, QSettings::IniFormat);

    const QString defaultSavePath = binaryPath + QDir::separator() + "save";
    settings.setValue("Settings/saveDir", defaultSavePath);

    return true;
}

bool MainWindow::CreateDefaultStructure()
{
    QDir saveDirectory(m_Settings.SaveDirectory.c_str());
    if(!saveDirectory.exists())
    {
        saveDirectory.mkdir(m_Settings.SaveDirectory.c_str());
    }

    return true;
}

void MainWindow::prepareMenu( const QPoint & pos )
{
    QTreeWidget *pTree = ui->taskTree;
    QTreeWidgetItem *pItem = pTree->itemAt( pos );


    QMenu menu(this);

    if(pItem)
    {
        QAction *pEditAction = new QAction(tr("&Edit Task..."), this);
        pEditAction->setStatusTip(tr("Edit a task"));
        connect(pEditAction, SIGNAL(triggered()), this, SLOT(EditCurrentItem()));
        menu.addAction(pEditAction);

        QAction *pDeleteAction = new QAction(tr("&Delete Task"), this);
        pDeleteAction->setStatusTip(tr("Delete a task"));
        connect(pDeleteAction, SIGNAL(triggered()), this, SLOT(DeleteCurrentItem()));
        menu.addAction(pDeleteAction);


    }
    else
    {
        QAction *newAct = new QAction(tr("&Create Task..."), this);
        newAct->setStatusTip(tr("Create a new task"));
        connect(newAct, SIGNAL(triggered()), this, SLOT(on_addTaskButton_clicked()));
        menu.addAction(newAct);
    }

    QPoint pt(pos);
    menu.exec( pTree->mapToGlobal(pt) );
}

void MainWindow::ApplyModel()
{
    CheckIfTracking();

    // Set the date
    QDate date = m_TaskDate.GetDate();
    ui->dateLabel->setText(date.toString());

    // Enable/disable stop button
    ui->stopTaskButton->setEnabled(false);
    if(m_bTrackingTask) {
        ui->stopTaskButton->setEnabled(true);
    }

    // Set the tree
    ui->taskTree->clear();

    unsigned int totalMinutes = 0;

    for(TaskModel task : m_TaskDate.GetTasks())
    {
        QDateTime startDateTime;
        startDateTime.setMSecsSinceEpoch(task.StartTime);

        QDateTime endDateTime;
        endDateTime.setMSecsSinceEpoch(task.EndTime);

        // Parent handles allocated memory
        QTreeWidgetItem* pItem = new QTreeWidgetItem(ui->taskTree);
        pItem->setText(0, task.TaskName.c_str());
        pItem->setText(1, startDateTime.toString());
        pItem->setText(4, task.Comment.c_str());

        if(task.EndTime > 1)
        {
            pItem->setText(2, endDateTime.toString());

            unsigned int minutes = task.CalculateMinuteDifference();
            std::string minuteDuration = std::to_string(minutes);

            totalMinutes += minutes;
            pItem->setText(3, minuteDuration.c_str());
        }
    }

    // Set total minutes
    ui->totalMinutesLabel->setText(std::to_string(totalMinutes).c_str());

    // Save model state
    if(!SaveModelState())
    {
//        QMessageBox::information(this, "Failure", "There was a failure to save task data to file!");
//        return;
    }

}

void MainWindow::CloseModel()
{
    m_CurrentSaveFile = "";

    // Update UI
    ui->taskTree->clear();
    ui->totalMinutesLabel->setText("0");
    ui->addTaskButton->setEnabled(false);
    ui->stopTaskButton->setEnabled(false);
    ui->dateLabel->setText("");

    m_bChangeMade = false;
    m_bTrackingTask = false;
}

bool MainWindow::SaveModelState()
{
    std::ofstream outputFile;
    outputFile.open(m_CurrentSaveFile);

    if(!outputFile.good())
        return false;

    // Header
    outputFile << m_TaskDate.GetDate().toString("MM/dd/yyyy").toStdString() << std::endl;// << ","
//               << "Start Time (in milliseconds)" << ","
//               << "End Time (in milliseconds)" << ","
//               << "Notes" << ","
//               << "Duration (in minutes)" << std::endl;

    // Entries
    for(TaskModel task : m_TaskDate.GetTasks())
        outputFile << task << std::endl;

    outputFile.close();
    return true;
}

bool MainWindow::LoadModelState(const std::string& path)
{
    std::ifstream savedFile(path);
    m_TaskDate.ClearTasks();

    bool firstLine = true;
    while (savedFile)
    {
        std::string line;
        if (!std::getline(savedFile, line))
            break;

        TaskModel task;
        task.Initialize();

        int step = 0;
        std::istringstream strStream(line);
        while (strStream)
        {
            if(firstLine)
            {
                if(line.size() != 10)
                    return false;

                int month = atoi(line.substr(0, 2).c_str());
                int day = atoi(line.substr(3, 2).c_str());
                int year = atoi(line.substr(6, 4).c_str());

                QDate date(year, month, day);
                if(!date.isValid())
                    return false;

                m_TaskDate.setDate(date);
                firstLine = false;
                break;
            }

            std::string lineString;
            if (!getline(strStream, lineString, ','))
                break;

            std::stringstream sConvert(lineString);

            switch(++step)
            {
            case 1:
                task.TaskName = lineString;
                break;
            case 2:
                sConvert >> task.StartTime;
                break;
            case 3:
                sConvert >> task.EndTime;
                break;
            case 4:
                task.Comment = lineString;
                break;
            }
        }

        if(!task.TaskName.empty())
        {
            if(task.EndTime == 0)
                m_bTrackingTask = true;

            m_TaskDate.AddTask(task);
        }
    }

    if (!savedFile.eof())
        return false;

    return true;
}

bool MainWindow::DoOpenFile()
{
    QFileDialog fileDialog;
    QString fileName = fileDialog.getOpenFileName(this, "Open task file", m_Settings.SaveDirectory.c_str(), "*.csv");


    if(fileName.isEmpty())
        return true;

    m_CurrentSaveFile = fileName.toStdString();
    LoadModelState(m_CurrentSaveFile);
    ApplyModel();

    ui->addTaskButton->setEnabled(true);
    return true;
}

bool MainWindow::DoCloseFile()
{

    CloseModel();

    if(m_bChangeMade)
    {
        QMessageBox saveConfirmationDialog;
        saveConfirmationDialog.setText("Task information is pending.");
        saveConfirmationDialog.setInformativeText("Do you want to save your changes?");
        saveConfirmationDialog.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        saveConfirmationDialog.setDefaultButton(QMessageBox::Save);

        int returnValue = saveConfirmationDialog.exec();

        if(returnValue == QMessageBox::Cancel)
            return false;

        if(returnValue == QMessageBox::Save)
        {
            // Save logic
        }

        m_bChangeMade = false;

    }

    return true;
}

void MainWindow::ShutdownWindow()
{
    if(!DoCloseFile())
        return;

    this->close();
}

void MainWindow::on_menuActionOpen_triggered()
{
    if(!DoOpenFile())
    {
        QMessageBox::information(this, "Failure", "There was a failure to open the time task file!");
        return;
    }
}

void MainWindow::on_actionExit_triggered()
{
    ShutdownWindow();
}

void MainWindow::on_addTaskButton_clicked()
{
    TimeTask taskDlg(this);
    taskDlg.exec();

    m_CurrentTask = taskDlg.GetTaskModel();

    if(m_CurrentTask.TaskName.empty())
        return;

    CapRunningTask();

    m_TaskDate.AddTask(m_CurrentTask);

    ApplyModel();

}

void MainWindow::CapRunningTask()
{

    TaskDateModel::TaskList_t newTaskList = m_TaskDate.GetTasks();
    m_TaskDate.ClearTasks();

    for(TaskModel task : newTaskList)
    {
        if(task.EndTime < 1 && m_bTrackingTask)
            task.EndTime = QDateTime::currentMSecsSinceEpoch();

        m_TaskDate.AddTask(task);
    }

    m_bTrackingTask = true;
}

std::string MainWindow::ConvertDateToFileName(const QDate& date) const
{
    int year, month, day;
    date.getDate(&year, &month, &day);

    std::string result;
    result += std::to_string(month) + std::string("-");
    result += std::to_string(day) + std::string("-");
    result += std::to_string(year);
    result += ".csv";

    return result;
}

void MainWindow::on_actionClose_triggered()
{
    DoCloseFile();
}

void MainWindow::on_stopTaskButton_clicked()
{
    CapRunningTask();
    ui->stopTaskButton->setEnabled(false);

    m_bTrackingTask = false;
    ApplyModel();
}

void MainWindow::EditCurrentItem()
{
    const unsigned int row = ui->taskTree->currentIndex().row();
    TaskModel task = m_TaskDate.GetTask(row);

    TimeTask taskDlg(this, &task);
    auto result = taskDlg.exec();

    m_CurrentTask = taskDlg.GetTaskModel();

    m_TaskDate.UpdateTask(row, m_CurrentTask);

    ApplyModel();
}

void MainWindow::DeleteCurrentItem()
{
    const unsigned int row = ui->taskTree->currentIndex().row();
    TaskModel task = m_TaskDate.GetTask(row);

    m_TaskDate.ClearTask(row);
    ApplyModel();
}

void MainWindow::on_taskTree_doubleClicked(const QModelIndex &index)
{
    EditCurrentItem();
}

void MainWindow::on_taskTree_customContextMenuRequested(const QPoint &pos)
{

}

void MainWindow::on_actionSave_triggered()
{
    QFileDialog saveFileDialog;
    QString saveFilePath = saveFileDialog.getSaveFileName(this, "Save task file", "", ".csv");
    if(saveFilePath.isEmpty())
        return;

    m_CurrentSaveFile = saveFilePath.toStdString();
    SaveModelState();
}

void MainWindow::CheckIfTracking()
{
    m_bTrackingTask = false;
    for(TaskModel task : m_TaskDate.GetTasks())
    {
        if(task.EndTime < 1)
        {
            m_bTrackingTask = true;
            return;
        }
    }
}

void MainWindow::on_actionConfiguration_triggered()
{
    ConfigurationDialog configDialog(this);
    int result = configDialog.exec();

    if(result)
    {
        LoadSettings();

        if(!SaveModelState())
        {
            QMessageBox::information(this, "Failure", "There was a failure to save to new file!");
            return;
        }
    }
}

void MainWindow::on_action_About_triggered()
{
    About aboutDialog(this);
    aboutDialog.exec();

}
