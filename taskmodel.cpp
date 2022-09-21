#include "taskmodel.h"

TaskModel::TaskModel()
{
    this->Initialize();
}

TaskModel::~TaskModel()
{

}

void TaskModel::Initialize()
{
    this->TaskName = "";
    this->StartTime = 0;
    this->EndTime = 0;
    this->Comment = "";
}

unsigned int TaskModel::CalculateMinuteDifference() const
{
    const qint64 millisecondDifference = EndTime - StartTime;

    return (unsigned int) (millisecondDifference/60000);
}

std::ostream& operator<<(std::ostream& os, const TaskModel& task)
{
    os << task.TaskName << ','
       << task.StartTime << ','
       << task.EndTime << ','
       << task.Comment << ',';

    if(task.EndTime > 0)
        os << task.CalculateMinuteDifference();
    else
        os << 0;

    return os;
}
