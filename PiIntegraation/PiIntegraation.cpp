/**
 * @file pi_integral.cpp
 * @brief Obliczanie liczby PI metodą całkowania numerycznego przy użyciu wielowątkowości.
 *
 * Program ten implementuje metodę prostokątów do przybliżonego obliczenia wartości liczby PI.
 * Dzięki zastosowaniu wielowątkowości, obliczenia są podzielone między wiele wątków, co
 * znacząco przyspiesza działanie programu na procesorach wielordzeniowych. Wyniki są
 * zapisywane do pliku w formacie CSV, co pozwala na późniejszą analizę wydajności
 * dla różnych konfiguracji liczby wątków i kroków.
 */

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cmath>
#include <fstream>

using namespace std;

/**
 * @brief Funkcja obliczająca wartość funkcji \( f(x) = \frac{4}{1 + x^2} \).
 *
 * Funkcja ta jest kluczowa w obliczaniu liczby PI metodą całkowania numerycznego.
 * Reprezentuje funkcję, której całka w granicach [0, 1] daje wartość liczby PI.
 *
 * @param x Punkt, w którym funkcja ma zostać obliczona.
 * @return Wartość funkcji \( f(x) \) w punkcie \p x.
 *
 * ### Wyjaśnienie:
 * Funkcja \( f(x) = \frac{4}{1 + x^2} \) pochodzi z przekształcenia równania
 * koła \( x^2 + y^2 = 1 \). Wynik integracji tej funkcji w zakresie od 0 do 1
 * równa się liczbie PI.
 */
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

/**
 * @brief Funkcja obliczająca wartość całki w zadanym przedziale metodą prostokątów.
 *
 * Funkcja wykorzystuje metodę prostokątów, aby przybliżyć wartość całki funkcji \( f(x) \)
 * na zadanym przedziale \p [start, end]. Całkowity przedział całkowania jest dzielony
 * na \p steps równych części, a wynik z każdej części jest sumowany.
 *
 * @param start Początek przedziału całkowania.
 * @param end Koniec przedziału całkowania.
 * @param steps Liczba kroków podziału w przedziale. Większa liczba kroków zwiększa dokładność.
 * @param stepSize Rozmiar jednego kroku (delta x). Określa szerokość prostokąta.
 * @param result Zmienna wynikowa, do której zapisany zostanie wynik obliczeń.
 *
 * ### Wyjaśnienie działania:
 * - Każdy krok reprezentuje prostokąt, którego szerokość to \p stepSize, a wysokość
 *   to wartość funkcji \( f(x) \) w środku tego prostokąta.
 * - Wynik dla danego prostokąta jest obliczany jako \( f(x) \times \text{stepSize} \),
 *   a wszystkie wyniki są sumowane w zmiennej \p result.
 */
void calculatePartialIntegral(double start, double end, long long steps, double stepSize, double& result) {
    double sum = 0.0; ///< Suma wartości prostokątów w przedziale.
    for (long long i = 0; i < steps; ++i) {
        double x = start + i * stepSize + stepSize / 2.0; ///< Środek prostokąta w bieżącym kroku.
        sum += f(x) * stepSize; ///< Dodanie pola prostokąta do sumy.
    }
    result = sum; ///< Zapisanie wyniku całkowania w przedziale do zmiennej \p result.
}

/**
 * @brief Funkcja główna programu.
 *
 * Funkcja sterująca, która wykonuje obliczenia liczby PI metodą całkowania numerycznego
 * dla różnych konfiguracji liczby wątków i kroków. Program obsługuje wielowątkowość,
 * dzieląc obliczenia na równoległe fragmenty, aby przyspieszyć działanie. Wyniki są
 * zapisywane w pliku CSV, co pozwala na analizę wpływu liczby wątków i kroków na wydajność.
 *
 * @return Zwraca 0, jeśli program zakończył się poprawnie.
 *
 * ### Wyjaśnienie:
 * - Funkcja iteruje przez różne liczby kroków (dokładności obliczeń) oraz różne
 *   liczby wątków (poziomy równoległości).
 * - Dla każdej kombinacji liczby kroków i wątków, funkcja:
 *   - Dzieli zakres obliczeń między wątki.
 *   - Uruchamia wątki do równoległego obliczania wartości całki.
 *   - Sumuje wyniki z poszczególnych wątków.
 *   - Zapisuje wyniki do pliku CSV.
 */
int main() {
    // Parametry testowe
    vector<long long> stepCounts = { 100000000, 1000000000, 3000000000 }; ///< Liczby podziałów (ilość kroków dla całkowania).
    int maxThreads = 50; ///< Maksymalna liczba wątków do testowania równoległych obliczeń.

    // Otwórz plik do zapisu wyników
    ofstream outputFile("results.csv"); ///< Strumień do zapisu wyników w pliku CSV.
    if (!outputFile.is_open()) {
        cerr << "Nie można otworzyć pliku results.csv do zapisu." << endl;
        return 1; ///< Kod błędu w przypadku niepowodzenia otwarcia pliku.
    }
    outputFile << "Liczba krokow,Liczba watków,Czas (s),Przyblizona liczba PI\n"; ///< Nagłówek pliku CSV.

    // Iteracja przez różne liczby kroków
    for (long long steps : stepCounts) {
        /**
         * @brief Długość jednego kroku (delta x).
         *
         * Dla danego kroku całkowania obliczana jest szerokość prostokąta
         * jako \( \text{stepSize} = \frac{1}{\text{steps}} \).
         */
        double stepSize = 1.0 / static_cast<double>(steps);

        // Iteracja przez liczbę wątków
        for (int numThreads = 1; numThreads <= maxThreads; ++numThreads) {
            vector<thread> threads; ///< Wektor przechowujący obiekty wątków.
            vector<double> partialResults(numThreads, 0.0); ///< Wyniki obliczeń dla poszczególnych wątków.

            /**
             * @brief Rejestracja czasu rozpoczęcia obliczeń.
             *
             * Używane do pomiaru wydajności dla każdej konfiguracji liczby kroków i wątków.
             */
            auto startTime = chrono::high_resolution_clock::now();

            // Podział pracy na wątki
            /**
             * @brief Liczba kroków przypadających na każdy wątek.
             *
             * Całkowity zakres obliczeń \p steps jest dzielony równomiernie na wątki.
             */
            long long stepsPerThread = steps / numThreads;
            for (int i = 0; i < numThreads; ++i) {
                /**
                 * @brief Początek i koniec zakresu dla bieżącego wątku.
                 *
                 * Każdy wątek obsługuje inny podprzedział całkowania,
                 * obliczany na podstawie numeru wątku i \p stepsPerThread.
                 */
                double start = i * stepsPerThread * stepSize;
                double end = (i + 1) * stepsPerThread * stepSize;

                // Tworzenie i uruchamianie wątku
                threads.emplace_back(calculatePartialIntegral, start, end, stepsPerThread, stepSize, ref(partialResults[i]));
            }

            // Czekanie na zakończenie wszystkich wątków
            /**
             * @brief Synchronizacja wątków.
             *
             * Program czeka, aż wszystkie wątki zakończą swoje obliczenia,
             * zanim przejdzie do kolejnych kroków.
             */
            for (auto& t : threads) {
                t.join();
            }

            // Sumowanie wyników częściowych
            /**
             * @brief Przybliżona wartość liczby PI.
             *
             * Wynik obliczeń dla danego zestawu parametrów (liczby kroków i wątków).
             * Sumowane są wyniki częściowe z każdego wątku.
             */
            double pi = 0.0;
            for (double result : partialResults) {
                pi += result;
            }

            // Rejestracja czasu zakończenia obliczeń
            auto endTime = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = endTime - startTime; ///< Czas trwania obliczeń.

            // Zapis wyników do pliku CSV
            /**
             * @brief Zapis wyników do pliku.
             *
             * Dla każdej kombinacji liczby kroków i wątków zapisane są:
             * - Liczba kroków całkowania.
             * - Liczba wątków użytych w obliczeniach.
             * - Czas trwania obliczeń w sekundach.
             * - Przybliżona wartość liczby PI.
             */
            outputFile << steps << "," << numThreads << "," << duration.count() << "," << pi << "\n";

            // Wyświetlanie wyników na konsoli
            /**
             * @brief Wyświetlanie wyników na konsoli.
             *
             * Wyniki obliczeń są również wyświetlane w konsoli, co pozwala
             * użytkownikowi śledzić postęp działania programu.
             */
            cout << "Liczba kroków: " << steps << ", Wątki: " << numThreads
                << ", Czas: " << duration.count() << "s, PI: " << pi << endl;
        }
    }

    // Zamknięcie pliku wyników
    /**
     * @brief Zamykanie pliku CSV.
     *
     * Plik wyników jest zamykany po zapisaniu wszystkich danych, aby upewnić się,
     * że dane zostały prawidłowo zapisane i zwolnić zasoby.
     */
    outputFile.close();
    cout << "Wyniki zapisane do pliku results.csv" << endl;

    return 0; ///< Zwraca 0, jeśli program zakończył się poprawnie.
}
