#include "timetask.h"
#include "ui_timetask.h"

TimeTask::TimeTask(QWidget *parent, TaskModel* pTaskModel/* = nullptr*/) :
    QDialog(parent),
    ui(new Ui::TimeTask)
{
    ui->setupUi(this);

    if(pTaskModel != nullptr)
    {
        m_TaskModel = *pTaskModel;

        ui->taskEditText->setText(m_TaskModel.TaskName.c_str());

        QDateTime startTime;
        startTime.setMSecsSinceEpoch(m_TaskModel.StartTime);
        ui->startDateTime->setDateTime(startTime);
        ui->commentText->setText(m_TaskModel.Comment.c_str());
        if(m_TaskModel.EndTime > 0)
        {
            QDateTime endTime;
            endTime.setMSecsSinceEpoch(m_TaskModel.EndTime);
            ui->endDateTime->setDateTime(endTime);
        }
        else
        {
            ui->endDateTime->setEnabled(false);
            ui->runningTaskCheckbox->setChecked(true);
        }
    }
    else
    {
        ui->startDateTime->setDateTime(QDateTime::currentDateTime());
        ui->endDateTime->setDateTime(QDateTime::currentDateTime());
        ui->okButton->setEnabled(false);

        ui->endDateTime->setEnabled(false);
        ui->runningTaskCheckbox->setChecked(true);
    }
}

TimeTask::~TimeTask()
{
    delete ui;
}

TaskModel TimeTask::GetTaskModel() const
{
    return m_TaskModel;
}

void TimeTask::SetTaskModel(const TaskModel& taskModel)
{
    m_TaskModel = taskModel;
}

void TimeTask::on_startDateTime_dateTimeChanged(const QDateTime &dateTime)
{
    QDateTime startDateTime = ui->startDateTime->dateTime();
    QDateTime endDateTime = ui->endDateTime->dateTime();

    if(startDateTime > endDateTime)
        ui->endDateTime->setDateTime(startDateTime);
}

void TimeTask::on_taskEditText_textChanged(const QString &arg1)
{
    bool enable = false;
    if(ui->taskEditText->text().toStdString().size())
        enable = true;

    ui->okButton->setEnabled(enable);
}

void TimeTask::on_okButton_clicked()
{
    QString taskText = ui->taskEditText->text();
    QString commentText = ui->commentText->toPlainText();
    m_TaskModel.TaskName = taskText.toStdString();
    m_TaskModel.Comment = commentText.toStdString();
    m_TaskModel.StartTime = ui->startDateTime->dateTime().toMSecsSinceEpoch();
    if(ui->runningTaskCheckbox->checkState() == Qt::CheckState::Unchecked)
        m_TaskModel.EndTime = ui->endDateTime->dateTime().toMSecsSinceEpoch();

    this->setResult(TASK_SAVE_CODE);
    this->close();
}

void TimeTask::on_runningTaskCheckbox_clicked()
{
    bool enableEndDate = true;
    if(ui->runningTaskCheckbox->checkState() == Qt::CheckState::Checked)
    {
        m_TaskModel.EndTime = 0;
        enableEndDate = false;
    }

    ui->endDateTime->setEnabled(enableEndDate);
}

void TimeTask::on_cancelButton_clicked()
{
    this->close();
}
