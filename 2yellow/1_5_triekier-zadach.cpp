#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <tuple>
using namespace std;

// enum class TaskStatus {
//   NEW,          // новая
//   IN_PROGRESS,  // в разработке
//   TESTING,      // на тестировании
//   DONE          // завершена
// };
// using TasksInfo = map<TaskStatus, int>;

TaskStatus next(const TaskStatus& s) {
    switch (s) {
    case TaskStatus::NEW:
        return TaskStatus::IN_PROGRESS;
    case TaskStatus::IN_PROGRESS:
        return TaskStatus::TESTING;
    case TaskStatus::TESTING:
        return TaskStatus::DONE;
    case TaskStatus::DONE:
        return TaskStatus::DONE;
    default:
        return TaskStatus::DONE;
    }
}

TasksInfo& clear(TasksInfo& t) {
    vector<TaskStatus> to_remove;
    for (auto& [s, c] : t) {
        if (c == 0) {
            to_remove.push_back(s);
        }
    }
    for (auto s : to_remove) {
        t.erase(s);
    }
    return t;
}

class TeamTasks {
public:
    const TasksInfo& GetPersonTasksInfo(const string& person) const {
        return db_.at(person);
    }

    void AddNewTask(const string& person) {
        db_[person][TaskStatus::NEW] += 1;
    }

    tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person, int task_count) {
        auto it = db_.find(person);
        if (it == end(db_)) {
            return {{}, {}};
        }
        TasksInfo old = it->second;
        TasksInfo touched;
        for (auto& [s, c] : old) {
            if (s == TaskStatus::DONE) {
                continue;
            }
            if (task_count == 0) {
                break;
            }
            int touched_c = min(task_count, c);
            task_count -= touched_c;
            c -= touched_c;
            it->second[s] -= touched_c;
            it->second[next(s)] += touched_c;
            touched[next(s)] = touched_c;
        }
        old.erase(TaskStatus::DONE);
        clear(it->second);
        return {clear(touched), clear(old)};
    }

private:
    map<string, TasksInfo> db_;
};

// void PrintTasksInfo(TasksInfo tasks_info) {
//   cout << tasks_info[TaskStatus::NEW] << " new tasks" <<
//       ", " << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress" <<
//       ", " << tasks_info[TaskStatus::TESTING] << " tasks are being tested" <<
//       ", " << tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
// }

// void ShowTasksInfo(const TasksInfo& t) {
//     cout << "[";
//     for (const auto& [s, c] : t) {
//         switch (s) {
//         case TaskStatus::NEW:
//             cout << "NEW";
//             break;
//         case TaskStatus::IN_PROGRESS:
//             cout << "IN_PROGRESS";
//             break;
//         case TaskStatus::TESTING:
//             cout << "TESTING";
//             break;
//         case TaskStatus::DONE:
//             cout << "DONE";
//             break;
//         }
//         cout << ": " << c << ", "; 
//     }
//     cout << "]";
// }

// void PerformAndShow(TeamTasks& db, const string& name, int count) {
//   auto [updated_tasks, untouched_tasks] = db.PerformPersonTasks(name, count);
//   ShowTasksInfo(updated_tasks);
//   cout << " <---> ";
//   ShowTasksInfo(untouched_tasks);
//   cout << endl;
// }

// int main() {
//   TeamTasks tasks;
//   TasksInfo updated_tasks, untouched_tasks;
//   for (int i = 0; i < 5; ++i) {
//     tasks.AddNewTask("Alice");
//   }
//   ShowTasksInfo(tasks.GetPersonTasksInfo("Alice"));
//   cout << endl;
//   PerformAndShow(tasks, "Alice", 5);
//   PerformAndShow(tasks, "Alice", 5);
//   PerformAndShow(tasks, "Alice", 1);
//   for (int i = 0; i < 5; ++i) {
//     tasks.AddNewTask("Alice");
//   }
//   PerformAndShow(tasks, "Alice", 2);
//   ShowTasksInfo(tasks.GetPersonTasksInfo("Alice"));
//   cout << endl;
//   PerformAndShow(tasks, "Alice", 4);
//   ShowTasksInfo(tasks.GetPersonTasksInfo("Alice"));
//   cout << endl;

//   return 0;
// }
