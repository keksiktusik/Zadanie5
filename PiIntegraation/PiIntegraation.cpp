#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <chrono>
#include <cmath>

using namespace std;

// Funkcja do obliczania wartości funkcji f(x) = 4 / (1 + x^2)
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

// Funkcja obliczająca całkę na zadanym przedziale
void calculatePartialIntegral(double start, double end, int steps, double stepSize, double& result) {
    double sum = 0.0;
    for (int i = 0; i < steps; ++i) {
        double x = start + i * stepSize + stepSize / 2.0; // Środek prostokąta
        sum += f(x) * stepSize;
    }
    result = sum;
}
int main()
{
    std::cout << "Hello World!\n";
}

