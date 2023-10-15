% Part 2
clear all;
clc;
%% a
RAND = unifrnd(0,1);                % standard (0,1) distribution uniform random number
t = 1;
x = -t * log(RAND);                 % exponentially distributed random number generator

%% b
t = 1;
for i = 1 : 1000
    RAND(i) = unifrnd(0,1);                % standard (0,1) distribution uniform random number
    xx(i) = -t * log(RAND(i));             % exponentially distributed 1000 random number generator
end

%% c
X = sum(xx)/1000;                          % average    
Varx = sum((xx - X).^2) / 1000;            % variance

