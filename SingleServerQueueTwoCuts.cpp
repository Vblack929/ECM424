#include <iostream>
#include <random>
#include <vector>

#define Q_limit 2
#define BUSY 1
#define IDLE 0

int next_event_type, num_customer_delayed, num_customer, num_in_queue,
    server_status, num_events, num_customer_required, num_delays_required, num_balked;
double area_num_in_queue, area_server_status, mean_interarrival, mean_service,
    sim_time, time_last_event, time_next_event[3], total_of_delays,
    time_arrival[Q_limit + 1], closing_time = 480.0;


double expon(double mean) {
    return -mean * log((double)rand() / (double)RAND_MAX);
}

void initialize() {
    // initialize the simulation
    sim_time = 0.0;

    // initialize the state variables
    server_status = IDLE;
    num_in_queue = 0;
    time_last_event = 0.0;

    // initialize the statistical counters
    num_customer_delayed = 0;
    total_of_delays = 0.0;
    area_num_in_queue = 0.0;
    area_server_status = 0.0;

    // initialize the event list
    // simulate a poisson process with mean mean_interarrival
    mean_interarrival = 1.0;
    mean_service = 0.5;
    time_next_event[1] = sim_time + expon(mean_interarrival);
    time_next_event[2] = 1.0e+30;
}

void timing() {
    // deternmine the type of next event
    double min_time_next_event = 1.0e+29;
    next_event_type = 0;

    for (int i = 1; i <= num_events; i++) {
        if (time_next_event[i] < min_time_next_event) {
            min_time_next_event = time_next_event[i];
            next_event_type = i;
        }
    }

    if (next_event_type == 0) {
        std::cout << "Event list empty at time " << sim_time << std::endl;
        exit(1);
    }

    sim_time = min_time_next_event;
}

void arrive() {
    if (sim_time > closing_time) {
        return;
    }
    double delay;
    // schedule next arrival
    time_next_event[1] = sim_time + expon(mean_interarrival);
    if (server_status == IDLE) {
        delay = 0.0;
        total_of_delays += delay;
        server_status = BUSY;
        num_customer_delayed++;
        time_next_event[2] = sim_time + expon(mean_service);
    } else if (num_in_queue < Q_limit) {  // server is busy
        num_in_queue++;
        time_arrival[num_in_queue] = sim_time;  // record the arrival time
    } else {
        num_balked++;
    }
}

void depart() {
    double delay = 0.0;
    double service_time = expon(mean_service);
    // if queue is empty
    if (num_in_queue == 0) {
        server_status = IDLE;
        time_next_event[2] = 1.0e+30;
    } else {
        num_in_queue--;
        delay = sim_time - time_arrival[1];
        total_of_delays += delay;

        num_customer_delayed++;
        time_next_event[2] = sim_time + service_time;

        for (int i = 1; i <= num_in_queue; i++) {
            time_arrival[i] = time_arrival[i + 1];
        }
    }
    double time_in_system = delay + service_time;
}

void update_time_avg_stats() {
    double time_since_last_event;

    // compute time since last event, and update last event time marker
    time_since_last_event = sim_time - time_last_event;
    time_last_event = sim_time;

    area_num_in_queue += num_in_queue * time_since_last_event;
    area_server_status += server_status * time_since_last_event;
}

void report() {
    std::cout << "Mean interarrival time: " << mean_interarrival << std::endl;
    std::cout << "Mean service time: " << mean_service << std::endl;
    std::cout << "Number of customers: " << num_customer_delayed << std::endl;
    std::cout << "Average delay in queue: "
              << total_of_delays / num_customer_delayed << std::endl;
    std::cout << "Average number in queue: " << area_num_in_queue / sim_time
              << std::endl;
    std::cout << "Server utilization: " << area_server_status / sim_time
              << std::endl;
    std::cout << "Time simulation ended: " << sim_time << std::endl;
}

int main() {
    num_events = 2;
    num_delays_required = 1000;

    // initialize the parameters
    initialize();
    std::cout << "Simulation Begins" << std::endl;
    while (sim_time < closing_time || num_in_queue > 0) {
        // determine the next event
        timing();
        // update time-average statistical accumulators
        update_time_avg_stats();
        // invoke the appropriate event function
        switch (next_event_type) {
            case 1:
                arrive();
                break;
            case 2:
                depart();
                break;
        }
    }
    report();
}