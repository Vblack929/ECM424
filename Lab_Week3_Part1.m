% Part 1
clear all;
clc;
%% 1
lamda = 0.3;                            % Arrival Rate
u = 0.5;                                % Service Rate
p = lamda / u;                          % Traffic Intensity (utilization)
N = lamda / (u - lamda);                % Mean Number of Customers in the system
T = 1 / (u *(1 - p));                   % Mean Response Time
Wq = (p/u) / (1 - p);                   % Mean Waiting Time
Lq = p^2 / (1 - p);                     % Mean Queue Length

%% 2
k = 8;
s = 8;
for z=1 : k
    u(z)=s+z;
    for i = 1 : k
        lamda(i) = u(z)/k*i;
        p(i,z) = lamda(i) / u(z);
        Wq(i,z) = (p(i,z)/u(z)) / (1 - p(i,z));                % Mean Waiting Time
        Lq(i,z) = p(i,z)^2 / (1 - p(i,z));                     % Mean Queue Lengt
    end
end

%% 3
% Draw the graph for Average Waiting time and Average Queue Length of the system
figure(1)
plot(Wq, Lq, '-');
xlabel('Mean Waiting Time');
ylabel('Average Queue Length');

%% 4
% Draw a 3 dimensional graph showing the arrival rate, service time and average waiting time of the system
figure(2)
surf(lamda,u,Wq);
xlabel('Arrival time');
ylabel('Service time');
zlabel('Average Waiting Time');
colorbar;

