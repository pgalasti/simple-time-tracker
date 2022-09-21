#ifndef TIMETASK_H
#define TIMETASK_H

#include <QDialog>

#include "taskmodel.h"

namespace Ui {
class TimeTask;
}

class TimeTask : public QDialog
{
    Q_OBJECT

public:

    static const int TASK_SAVE_CODE = 1;

    explicit TimeTask(QWidget *parent = 0, TaskModel* pTaskModel = nullptr);
    ~TimeTask();

    TaskModel GetTaskModel() const;
    void SetTaskModel(const TaskModel& taskModel);

private slots:

    void on_startDateTime_dateTimeChanged(const QDateTime &dateTime);

    void on_taskEditText_textChanged(const QString &arg1);

    void on_okButton_clicked();

    void on_runningTaskCheckbox_clicked();

    void on_cancelButton_clicked();

private:
    Ui::TimeTask *ui;

    TaskModel m_TaskModel;
};

#endif // TIMETASK_H
