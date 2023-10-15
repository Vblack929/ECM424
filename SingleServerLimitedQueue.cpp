#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <matplotlibcpp.h>
#include <fstream>
namespace plt = matplotlibcpp;


#define Q_limit 1000000
#define BUSY 1
#define IDLE 0
#define MAX_QUEUE_SIZE 10

int next_event_type, num_customer_delayed, num_customer, num_in_queue,
    server_status, num_events, num_delays_required, num_cutsomer_rejected;
double area_num_in_queue, area_server_status, mean_interarrival, arrival_rate, mean_service, service_rate,
    sim_time, time_last_event, time_next_event[3], total_of_delays,
    time_arrival[Q_limit + 1];

std::vector<double> arrival_rates;
std::vector<double> avg_waiting_times;
std::vector<double> avg_queue_lengths;

std::vector<double> analytical_waiting_times;
std::vector<double> analytical_queue_lengths;
std::vector<double> traffic_intensities;


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
    double delay;
    // schedule next arrival
    time_next_event[1] = sim_time + expon(mean_interarrival);
    if (server_status == IDLE) {
        delay = 0.0;
        total_of_delays += delay;
        server_status = BUSY;
        num_customer_delayed++;
        time_next_event[2] = sim_time + expon(mean_service);
    } else {  // server is busy
       if (num_in_queue < MAX_QUEUE_SIZE) {
            num_in_queue++;
            if (num_in_queue > Q_limit) {
                std::cout << "Overflow of the array time_arrival at";
                std::cout << " time " << sim_time << std::endl;
                exit(2);
            }
            time_arrival[num_in_queue] = sim_time;
       } else {
            num_cutsomer_rejected++;
       }
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
    std::ofstream outfile("results/output_mm1k.txt");

    outfile << "Simulation results:\n" << std::endl;
    outfile << "Arrival Rate\tAverage Waiting Time\tAverage Queue Length\n";
    for (size_t i = 0; i < arrival_rates.size(); ++i) {
        outfile << arrival_rates[i] << "\t" 
                << avg_waiting_times[i] << "\t" 
                << avg_queue_lengths[i] << "\n";
    }

    outfile << "\nAnalytical Results:\n";
    outfile << "Arrival Rate\tAnalytical Waiting Time\tAnalytical Queue Length\tTraffic Intensity\n";
    for (size_t i = 0; i < arrival_rates.size(); ++i) {
        outfile << arrival_rates[i] << "\t" 
                << analytical_queue_lengths[i] << "\t"
                << traffic_intensities[i] << "\n";
    }
    outfile.close();
}

int main() {
    num_events = 2;
    num_delays_required = 100000;

    // initialize the parameters
    std::cout << "Simulation Begins" << std::endl;
    for (double arrival_rate = 1; arrival_rate < 100; arrival_rate += 1) {
        initialize();
        mean_interarrival = 1.0 / arrival_rate;
        mean_service = 0.01;
        while (num_customer_delayed < num_delays_required) {
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
        arrival_rates.push_back(arrival_rate);
        avg_waiting_times.push_back(total_of_delays / num_customer_delayed);
        avg_queue_lengths.push_back(area_num_in_queue / sim_time);
        // compute analytical result
        double traffic_intensity = arrival_rate * mean_service;
        double mean_queue_length = traffic_intensity / (1 - traffic_intensity) - (MAX_QUEUE_SIZE + 1) * pow(traffic_intensity, MAX_QUEUE_SIZE + 1) / (1 - pow(traffic_intensity, MAX_QUEUE_SIZE + 1));
        analytical_queue_lengths.push_back(mean_queue_length);
        traffic_intensities.push_back(traffic_intensity);
    }
    report();
    // plot
    std::string title2 = "Average Queue Length vs Arrival Rate (" + std::to_string(num_delays_required) + " customers)";
    plt::named_plot("Simulation", arrival_rates, avg_queue_lengths);
    plt::named_plot("Analytical", arrival_rates, analytical_queue_lengths);
    plt::xlabel("Arrival Rate");
    plt::ylabel("Average Queue Length");    
    plt::title(title2);
    plt::annotate("Number of customers simulated: " + std::to_string(num_delays_required), 0.05, 0.05);
    plt::legend();
    std::string filename2 = "results/mm1k_avg_queue_length_" + std::to_string(num_delays_required) + ".png";
    plt::save(filename2);
    plt::show();

    return 0;
}