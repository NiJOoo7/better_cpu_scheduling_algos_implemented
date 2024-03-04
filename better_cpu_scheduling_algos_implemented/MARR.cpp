#include <iostream>
#include <vector>
#include <queue>
#include <numeric>
#include <cmath>

using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int remainingBurstTime;
};

int calculateInitialQuantum(const vector<Process>& processes) {
    int median = 0;
    int totalBurst = 0;

    int sortBT[processes.size()];
    int i = 0;
    for (const auto& process : processes) {
        totalBurst += process.burstTime;
        sortBT[i++] = process.burstTime;
    }

    for(int i=0; i<processes.size(); i++)
    {
        for(int j=i; j<processes.size(); j++)
        {
            if(sortBT[i] > sortBT[j])
            {
                swap(sortBT[i], sortBT[j]);
            }
        }
    }

    median = sortBT[int(processes.size()/2)];

    return round((median + round(totalBurst/processes.size())) / 2);
}

void findWaitingTime(vector<Process>& processes, int n, int bt[], int wt[], int& quantum) {
    int remainingTime[n];
    for (int i = 0; i < n; i++)
        remainingTime[i] = bt[i];

    int t = 0;

    queue<int> readyQueue;
    int idx = 0;

    while (true) {
        bool done = true;

        for (int i = 0; i < n; i++) {
            if (remainingTime[i] > 0) {
                done = false;

                if (remainingTime[i] > quantum) {
                    t += quantum;
                    remainingTime[i] -= quantum;
                } else {
                    t += remainingTime[i];
                    wt[i] = t - processes[i].burstTime - processes[i].arrivalTime;
                    remainingTime[i] = 0;
                }

                while (idx < n && processes[idx].arrivalTime <= t) {
                    readyQueue.push(idx);
                    idx++;
                }
            }
        }

        if (done)
            break;

        if (!readyQueue.empty()) {
            int frontIdx = readyQueue.front();
            readyQueue.pop();

            int median;
            if (n % 2 == 0) {
                median = (remainingTime[n / 2 - 1] + remainingTime[n / 2]) / 2;
            } else {
                median = remainingTime[n / 2];
            }


            quantum = round((median + round(accumulate(remainingTime, remainingTime + n, 0))) / 2);

            readyQueue.push(frontIdx);
            t = max(t, processes[frontIdx].arrivalTime);
        }
    }
}

void findTurnaroundTime(vector<Process>& processes, int n, int bt[], int wt[], int tat[]) {
    for (int i = 0; i < n; i++)
        tat[i] = bt[i] + wt[i];
}

void findAverageTimes(vector<Process>& processes, int n, int& quantum) {
    int bt[n];
    int wt[n];
    int tat[n];

    for (int i = 0; i < n; i++) {
        bt[i] = processes[i].burstTime;
        processes[i].remainingBurstTime = processes[i].burstTime;
    }

    findWaitingTime(processes, n, bt, wt, quantum);
    findTurnaroundTime(processes, n, bt, wt, tat);

    float total_wt = 0, total_tat = 0;
    cout << "Process\tWaiting Time\tTurnaround Time\n";
    for (int i = 0; i < n; i++) {
        cout << processes[i].id << "\t" << wt[i] << "\t\t" << tat[i] << endl;
        total_wt += wt[i];
        total_tat += tat[i];
    }

    cout << "\nAverage Waiting Time: " << (total_wt / n) << endl;
    cout << "Average Turnaround Time: " << (total_tat / n) << endl;
}

int main() {
    int n, quantum;

    cout << "Enter the number of processes: ";
    cin >> n;

    vector<Process> processes(n);

    cout << "Enter the arrival time and burst time for each process:\n";
    for (int i = 0; i < n; i++) {
        processes[i].id = i + 1;
        cout << "Process " << processes[i].id << " Arrival Time: ";
        cin >> processes[i].arrivalTime;
        cout << "Process " << processes[i].id << " Burst Time: ";
        cin >> processes[i].burstTime;
    }

    quantum = calculateInitialQuantum(processes);

    findAverageTimes(processes, n, quantum);

    return 0;
}
