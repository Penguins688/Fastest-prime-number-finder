#include <iostream>
#include <fstream>
#include <gmpxx.h>
#include <vector>
#include <pthread.h>
#include <queue>

using namespace std;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
priority_queue<mpz_class, vector<mpz_class>, greater<mpz_class> > pq;
mpz_class current_number;

bool isPrime(const mpz_class& num) {
    if (num <= 1) return false;
    if (num == 2) return true;
    if (num % 2 == 0 || num % 3 == 0) return false;

    mpz_class i = 5;
    while (i * i <= num) {
        if (num % i == 0 || num % (i + 2) == 0) return false;
        i += 6;
    }
    return true;
}

struct ThreadArgs {
    mpz_class start;
    int count;
    ofstream* PrimeNumbers;
};

void* findPrimes(void* args) {
    ThreadArgs* tArgs = static_cast<ThreadArgs*>(args);
    mpz_class num = tArgs->start;
    for (int i = 0; i < tArgs->count; i++) {
        if (isPrime(num)) {
            pthread_mutex_lock(&mtx);
            pq.push(num);
            while (!pq.empty() && pq.top() == current_number) {
                *(tArgs->PrimeNumbers) << pq.top() << "\n";
                pq.pop();
                current_number++;
            }
            pthread_mutex_unlock(&mtx);
        }
        num++;
    }
    return NULL;
}

int main() {
    ofstream PrimeNumbers("numbers.txt", ios::out | ios::app);
    if (!PrimeNumbers) {
        cerr << "Unable to open file numbers.txt\n";
        return 1;
    }

    string range;
    string start;
    int numThreads;
    
    cout << "Pick amount of iterations: ";
    cin >> range;
    cout << "Pick starting number: ";
    cin >> start;
    cout << "Enter number of threads: ";
    cin >> numThreads;

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

    PrimeNumbers << "\n";

    current_number = startingNum;

    vector<pthread_t> threads(numThreads);
    vector<ThreadArgs> threadArgs(numThreads);
    int iterationsPerThread = iterations / numThreads;
    int remainingIterations = iterations % numThreads;

    for (int i = 0; i < numThreads; i++) {
        threadArgs[i].count = iterationsPerThread + (i < remainingIterations ? 1 : 0);
        threadArgs[i].start = startingNum + i * iterationsPerThread + (i < remainingIterations ? i : remainingIterations);
        threadArgs[i].PrimeNumbers = &PrimeNumbers;
        pthread_create(&threads[i], NULL, findPrimes, &threadArgs[i]);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    while (!pq.empty()) {
        PrimeNumbers << pq.top() << "\n";
        pq.pop();
    }

    PrimeNumbers.close();
    pthread_mutex_destroy(&mtx);
    return 0;
}
