#include <iostream>
#include <vector>

using namespace std;

void Worry(vector<bool>& queue) {
    int num;
    cin >> num;
    queue[num] = true;
}

void Quiet(vector<bool>& queue) {
    int num;
    cin >> num;
    queue[num] = false;
}

void Come(vector<bool>& queue) {
    int num;
    cin >> num;
    queue.resize(queue.size() + num, false);
}

void WorryCount(const vector<bool>& queue) {
    int sum = 0;
    for (bool w : queue) {
        if (w) {
            sum++;
        }
    }
    cout << sum << endl;
}

int main()
{
    vector<bool> queue;

    int commands_num;    
    cin >> commands_num;

    while (commands_num > 0) {
        commands_num--;
        string command;
        cin >> command;
        if (command == "WORRY") {
            Worry(queue);
        } else if (command == "QUIET") {
            Quiet(queue);
        } else if (command == "COME") {
            Come(queue);
        } else if (command == "WORRY_COUNT") {
            WorryCount(queue);
        }
    }
    return 0;
}