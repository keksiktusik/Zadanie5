/**
 * @file pi_calculation.cpp
 * @brief Program do obliczania przybliżonej wartości liczby PI metodą całkowania numerycznego.
 *
 * Program wykorzystuje zrównoleglenie obliczeń za pomocą biblioteki <thread>.
 * Użytkownik podaje liczbę kroków oraz liczbę wątków. Program oblicza wartość liczby PI
 * oraz mierzy czas wykonania obliczeń.
 */

#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <chrono>
#include <cmath>

using namespace std;

/**
 * @brief Funkcja matematyczna obliczająca wartość 4 / (1 + x^2).
 *
 * Jest to funkcja podcałkowa używana w metodzie całkowania numerycznego
 * do przybliżania wartości liczby PI.
 *
 * @param x Wartość zmiennej niezależnej.
 * @return Wynik funkcji f(x) = 4 / (1 + x^2).
 */
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

/**
 * @brief Funkcja obliczająca całkę oznaczoną na zadanym przedziale.
 *
 * Funkcja dzieli przedział na równe części (kroki) i oblicza sumę
 * pól prostokątów o wysokościach wyznaczonych przez funkcję f(x).
 *
 * @param start Początek przedziału całkowania.
 * @param end Koniec przedziału całkowania.
 * @param steps Liczba kroków na przedziale.
 * @param stepSize Rozmiar pojedynczego kroku.
 * @param result Referencja do zmiennej, w której zapisany zostanie wynik częściowy.
 *
 * @details Dla każdego kroku obliczana jest wartość funkcji w środku przedziału
 * oraz pole prostokąta, które jest sumowane w zmiennej \p result.
 */
void calculatePartialIntegral(double start, double end, int steps, double stepSize, double& result) {
    double sum = 0.0; /**< Zmienna przechowująca sumę pól prostokątów na przedziale. */
    for (int i = 0; i < steps; ++i) {
        double x = start + i * stepSize + stepSize / 2.0; /**< Środek prostokąta. */
        sum += f(x) * stepSize; /**< Dodanie pola prostokąta do sumy. */
    }
    result = sum; /**< Zapisanie wyniku częściowego w zmiennej referencyjnej. */
}

/**
 * @brief Funkcja główna programu.
 *
 * Użytkownik podaje liczbę wątków i liczbę kroków. Program dzieli obliczenia
 * na wiele wątków, wykonuje je równolegle, sumuje wyniki częściowe i mierzy czas.
 *
 * @return Zwraca 0, jeśli program zakończy się poprawnie.
 *
 * @details Algorytm opiera się na metodzie prostokątów do obliczania całki oznaczonej
 * z funkcji \p f(x). Program dzieli przedział na mniejsze fragmenty i równolegle oblicza
 * częściowe wartości, które są sumowane do wyniku końcowego.
 */
int main()
{
    int numThreads; /**< Liczba wątków podana przez użytkownika. */
    int numSteps;   /**< Liczba kroków podana przez użytkownika. */

    cout << "Podaj liczbe wątków: ";
    cin >> numThreads;
    cout << "Podaj liczbe kroków (np. 100000000): ";
    cin >> numSteps;

    double stepSize = 1.0 / numSteps; /**< Rozmiar pojedynczego kroku. */
    vector<thread> threads; /**< Wektor przechowujący wątki. */
    vector<double> partialResults(numThreads, 0.0); /**< Wektor wyników częściowych. */

    auto startTime = chrono::high_resolution_clock::now(); /**< Start pomiaru czasu. */

    /**
     * @brief Podział pracy na wątki.
     *
     * Każdy wątek oblicza częściową wartość całki na przypisanym mu przedziale.
     */
    int stepsPerThread = numSteps / numThreads; /**< Liczba kroków na wątek. */
    for (int i = 0; i < numThreads; ++i) {
        double start = i * stepsPerThread * stepSize; /**< Początek przedziału dla wątku. */
        double end = (i + 1) * stepsPerThread * stepSize; /**< Koniec przedziału dla wątku. */
        threads.emplace_back(calculatePartialIntegral, start, end, stepsPerThread, stepSize, ref(partialResults[i]));
    }

    /**
     * @brief Czekanie na zakończenie wszystkich wątków.
     */
    for (auto& t : threads) {
        t.join();
    }

    /**
     * @brief Sumowanie wyników częściowych.
     *
     * Wyniki częściowe obliczone przez każdy wątek są sumowane do zmiennej \p pi.
     */
    double pi = 0.0; /**< Przybliżona wartość liczby PI. */
    for (double result : partialResults) {
        pi += result;
    }

    auto endTime = chrono::high_resolution_clock::now(); /**< Koniec pomiaru czasu. */
    chrono::duration<double> duration = endTime - startTime; /**< Czas wykonania obliczeń. */

    /**
     * @brief Wyświetlanie wyniku oraz czasu obliczeń.
     */
    cout << "Przyblizona liczba PI: " << pi << endl;
    cout << "Czas obliczen: " << duration.count() << " sekund" << endl;

    return 0;
}
