﻿#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>

using namespace std;

struct Event {
    int data;
};

class Monitor {
public:
    void addEvent(const Event& event) {
        unique_lock<mutex> lock(mutex_);
        queue_.push(event);
        cv_.notify_one();
    }

    Event getEvent() {
        unique_lock<mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        Event event = queue_.front();
        queue_.pop();
        return event;
    }

private:
    queue<Event> queue_;
    mutex mutex_;
    condition_variable cv_;
};

Event generate_event_data() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 100);
    Event event;
    event.data = dist(gen);

    return event;
}

void producer(Monitor& monitor) {
    while (true) {
        Event event = generate_event_data();

        cout << "Поставщик: Сгенерировано событие с данными " << event.data << endl;
        monitor.addEvent(event);

        cout << "Поставщик: Обработка события..." << endl;
        _sleep(1);
    }
}

void consumer(Monitor& monitor) {
    while (true) {
        Event event = monitor.getEvent();

        cout << "Потребитель: Получено событие с данными " << event.data << endl;
        cout << "Потребитель: Обработка события..." << endl;
        _sleep(1);
    }
}

int main() {
    Monitor monitor;
    setlocale(LC_ALL, "russian");
    thread supplierThread(producer, ref(monitor));
    thread consumerThread(consumer, ref(monitor));

    supplierThread.join();
    consumerThread.join();

    return 0;
}