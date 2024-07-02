#include <iostream>
#include <fstream>
#include <gmpxx.h>
using namespace std;

bool isPrime(const mpz_class& num) {
    if (num <= 1) return false;
    if (num == 2) return true;
    if (num % 2 == 0 || num % 3 == 0) return false;

    mpz_class i = 5;
    while (i * i <= num) {
        if (num % i == 0 || num % (i + 2) == 0) return false;
        i+=6;
    }
    return true;
}

int main() {
    ofstream PrimeNumbers("numbers.txt", ios::out | ios::app);
    if (!PrimeNumbers) {
        cerr << "Unable to open file numbers.txt\n";
        return 1;
    }

    string range;
    string start;
    cout << "Pick amount of iterations: ";
    cin >> range;
    cout << "Pick starting number: ";
    cin >> start;

    int iterations;
    mpz_class startingNum;

    try {
        iterations = stoi(range);
        startingNum = mpz_class(start);
    } catch (const invalid_argument& e) {
        cerr << "Invalid input: " << e.what() << '\n';
        return 1;
    } catch (const out_of_range& e) {
        cerr << "Input out of range: " << e.what() << '\n';
        return 1;
    }

    mpz_class num = startingNum;

    PrimeNumbers << "\n";
    for (int i = 0; i < iterations; i++) {
        if (isPrime(num)) {
            PrimeNumbers << num << "\n";
        }
        num++;
    }

    PrimeNumbers.close();
    return 0;
}