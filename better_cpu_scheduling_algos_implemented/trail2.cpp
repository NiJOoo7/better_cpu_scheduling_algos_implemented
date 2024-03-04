#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;
const int NUM_CHAIRS = 5;
mutex mtx;
condition_variable barber_cv, customer_cv;
bool barber_sleeping = true;
int num_customers_waiting = 0;
bool continue_barber = true;
void barber()
{
    while (true)
    {
        unique_lock<mutex> lock(mtx);
        while (num_customers_waiting == 0 && continue_barber)
        {
            cout << "Barber is sleeping...\n";
            barber_cv.wait(lock);
        }
        if (!continue_barber)
        {
            cout << "Barber is going home.\n";
            break;
        }
        barber_sleeping = false;
        num_customers_waiting--;
        cout << "Barber is cutting hair.\n";
        this_thread::sleep_for(chrono::seconds(2));
        customer_cv.notify_one();
    }
}

void customer(int id)
{
    unique_lock<mutex> lock(mtx);
    if (barber_sleeping)
    {
        cout << "Customer " << id << " woke up the barber and is getting a haircut.\n";
        barber_sleeping = false;
        barber_cv.notify_one();
    }
    else
    {
        if (num_customers_waiting < NUM_CHAIRS)
        {
            num_customers_waiting++;
            cout << "Customer " << id << " is waiting in the waiting room.\n";
            customer_cv.wait(lock);
            cout << "Customer " << id << " is getting a haircut.\n";
        }
        else
        {
            cout << "Customer " << id << " couldn't find a free chair and is leaving.\n";
        }
    }
}

int main()
{
    thread barber_thread(barber);

    thread customer_threads[10];
    for (int i = 0; i < 10; ++i)
    {
        customer_threads[i] = thread(customer, i + 1);
        this_thread::sleep_for(chrono::seconds(1));
    }
    for (auto &thread : customer_threads)
    {
        thread.join();
    }
    {
        lock_guard<mutex> lock(mtx);
        continue_barber = false;
    }
    barber_cv.notify_one();
    barber_thread.join();
    return 0;
}