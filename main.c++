#include <iostream>
#include <fstream>
#include <pthread.h>
#include <sstream>
#include <queue>
#include <algorithm>
#include <vector>
#include <string>
#include <cctype>
#include <set>
#include <map>

using namespace std;


struct ThreadData {
    vector<string> *files;
    map<string, set<int>> *WordMap;
    vector<map<string, set<int>>> *WordMaps;
    queue<int> *fileQueue;
    queue<char> *letterQueue;
    pthread_mutex_t *Mutex;
    pthread_barrier_t *barrier;  
};

void *process_files(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    while (!(data->fileQueue->empty())) {
        pthread_mutex_lock(data->Mutex);


        int IndexFile = data->fileQueue->front();
        data->fileQueue->pop();

        pthread_mutex_unlock(data->Mutex);

        ifstream file("../checker/" + (*data->files)[IndexFile]);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                istringstream f1(line);
                string word;
                while (f1 >> word) {
                    string word1;
                    for(char l : word){
                        if (isalpha(l)) {
                            word1 += tolower(l);
                        }
                    }

                    (*data->WordMap)[word1].insert(IndexFile + 1);
                }
            }
            file.close();
        }
    }

    pthread_barrier_wait(data->barrier);
    return NULL;
}

void *sort_files(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    pthread_barrier_wait(data->barrier);

    while (!(data->letterQueue->empty())) {
        pthread_mutex_lock(data->Mutex);


        char litera = data->letterQueue->front();
        data->letterQueue->pop();

        pthread_mutex_unlock(data->Mutex);

        map<string, set<int>> GlobalMap;
        for (const auto &localMap : *data->WordMaps) {
            for (const auto &entry : localMap) {
                if (entry.first[0] == litera) {
                    GlobalMap[entry.first].insert(entry.second.begin(), entry.second.end());
                }
            }
        }

        vector<pair<string, set<int>>> sortedList(GlobalMap.begin(), GlobalMap.end());
        sort(sortedList.begin(), sortedList.end(), [](const auto &a, const auto &b) {
            return (a.second.size() != b.second.size()) ? a.second.size() > b.second.size()
                                                        : a.first < b.first;
        });

        ofstream outf(string(1, litera) + ".txt");
        if (outf.is_open()) {
            for (const auto &entry : sortedList) {
                outf << entry.first << ":[";

                int nr = 0;
                for (int i : entry.second) {
                    if (nr >= 1) {
                        outf << " ";
                    }
                    outf << i;
                    nr++;
                }
                outf << "]" << endl;
            }
            outf.close();
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
  
    int numProcessThreads = atoi(argv[1]);
    int numSortThreads = atoi(argv[2]);
    vector<string> files;

    ifstream test_file(argv[3]);
    string line;
    int count = 0;
    while (getline(test_file, line)) {
        if (count != 0) {
            files.push_back(line);
        }
        count++;
    }

    queue<int> coada_de_fisiere;
    for (int i = 0; i < files.size(); i++) {
        coada_de_fisiere.push(i);
    }

    queue<char> coada_de_litere;
    for (char ch = 'a'; ch <= 'z'; ch++) {
        coada_de_litere.push(ch);
    }

    vector<map<string, set<int>>> WordMaps(numProcessThreads);
    pthread_t threads[numProcessThreads + numSortThreads];
    pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_barrier_t barrier;

    pthread_barrier_init(&barrier, NULL, numProcessThreads + numSortThreads);

    ThreadData threadData[numProcessThreads + numSortThreads];



    for (int i = 0; i < max(numProcessThreads,numSortThreads); i++) {
        if(i < numProcessThreads){
        threadData[i] = {&files, &WordMaps[i], &WordMaps, &coada_de_fisiere, &coada_de_litere,
                         &queueMutex, &barrier};
        pthread_create(&threads[i], NULL, process_files, &threadData[i]);
        }
        if(i < numSortThreads){
            threadData[i + numProcessThreads] = {&files, nullptr, &WordMaps, &coada_de_fisiere, &coada_de_litere,
                         &queueMutex, &barrier};
            pthread_create(&threads[i + numProcessThreads], NULL, sort_files, &threadData[i + numProcessThreads]);
        }
    }

    for (int i = 0; i < numProcessThreads + numSortThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    return 0;
}