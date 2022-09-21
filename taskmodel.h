#ifndef TASK_H
#define TASK_H

#include<string>
#include<iostream>
#include<QtGlobal>


struct TaskModel
{
    std::string TaskName;
    qint64 StartTime;
    qint64 EndTime;
    std::string Comment;

    TaskModel();
    ~TaskModel();

    inline void Initialize();
    unsigned int CalculateMinuteDifference() const;

    friend std::ostream& operator<<(std::ostream& os, const TaskModel& task);

};
#endif // TASK_H
