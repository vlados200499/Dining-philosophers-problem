#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#define NUM_PHILOSOPHERS 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (phNum + 4) % NUM_PHILOSOPHERS
#define RIGHT (phNum + 1) % NUM_PHILOSOPHERS

int state[NUM_PHILOSOPHERS];
int philosophers[NUM_PHILOSOPHERS] = {0, 1, 2, 3, 4};
std::priority_queue<std::pair<int, int>> priorityQueue; // Para (priorytet, indeks filozofa)


std::mutex mutex;
std::condition_variable forkCV[NUM_PHILOSOPHERS];

void test(int phNum) {
    if (state[phNum] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        // state that eating
        state[phNum] = EATING;

        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::cout << "Philosopher " << phNum + 1 << " takes fork " << LEFT + 1 << " and " << phNum + 1 << std::endl;
        std::cout << "Philosopher " << phNum + 1 << " has priority " << priorityQueue.top().first << std::endl;
        std::cout << "Philosopher " << phNum + 1 << " is Eating" << std::endl;

        int meal_value = 10;//(rand() % 10)+2;
        auto a = priorityQueue.top();
        priorityQueue.pop();
        priorityQueue.emplace(a.first - meal_value, phNum);
        forkCV[phNum].notify_all();
    }
}

// take up chopsticks
void takeFork(int phNum) {

    std::unique_lock<std::mutex> lock(mutex);

    // state that hungry
    state[phNum] = HUNGRY;

    std::cout << "Philosopher " << phNum + 1 << " is Hungry" << std::endl;

    if (priorityQueue.top().second == phNum) {
        // eat if neighbors are not eating
        test(phNum);
    }
    if (state[phNum] != EATING) {
        forkCV[phNum].wait(lock);
    }
}

// put down chopsticks
void putFork(int phNum) {

    std::unique_lock<std::mutex> lock(mutex);

    // state that thinking
    state[phNum] = THINKING;

    std::cout << "Philosopher " << phNum + 1 << " putting fork " << LEFT + 1 << " and " << phNum + 1 << " down"
              << std::endl;
    std::cout << "Philosopher " << phNum + 1 << " is thinking" << std::endl;

    if (priorityQueue.top().second == LEFT) {
        test(LEFT);
    }
    if (priorityQueue.top().second == RIGHT) {
        test(RIGHT);
    }
}

void philosopher(int num) {
    while (true) {
        takeFork(num);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        putFork(num);
    }
}

int main() {
    std::thread threadId[NUM_PHILOSOPHERS];

    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        // create philosopher processes
        priorityQueue.emplace(9999999, i);
        threadId[i] = std::thread(philosopher, philosophers[i]);

        std::cout << "Philosopher " << i + 1 << " is thinking" << std::endl;
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; ++i)
        threadId[i].join();

    return 0;
}