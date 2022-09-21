#ifndef TASKDATE_H
#define TASKDATE_H

#include "taskmodel.h"
#include <QDate>
#include <vector>

class TaskDateModel
{

public:

    typedef std::vector<TaskModel> TaskList_t;

    TaskDateModel() {}
    ~TaskDateModel() {}

    void AddTask(const TaskModel& task) { tasks.push_back(task); }
    void ClearTasks() { tasks.clear(); }

    void ClearTask(unsigned int index)
    {
        auto iter = tasks.begin() + index;
        tasks.erase(iter);
    }

    void setDate(const QDate& date) { this->date = date; }

    QDate GetDate() { return date; }
    TaskList_t GetTasks() { return tasks; }
    TaskModel GetTask(unsigned int index)
    {
        return tasks[index];
    }

    void UpdateTask(unsigned int index, const TaskModel& task)
    {
        tasks[index] = task;
    }

protected:
    QDate date;
    TaskList_t tasks;
};

#endif // TASKDATE_H
