#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
using namespace std;

class Bucket {
public:
    int localDepth;
    int size;
    vector<int> keys;
    vector<int> values;

    Bucket(int size, int localDepth) {
        this->localDepth = localDepth;
        this->size = 0;
        keys.resize(size);
        values.resize(size);
    }

    bool isFull() {
        return size == keys.size();
    }

    bool isEmpty() {
        return size == 0;
    }

    void insert(int key, int value) {
        for (int i = 0; i < size; i++) {
            if (keys[i] == key) {
                values[i] = value;
                return;
            }
        }
        if (isFull()) {
            throw runtime_error("Bucket is full");
        }
        keys[size] = key;
        values[size] = value;
        size++;
    }

    void deleteKey(int key) {
        if (size == 0) {
            return;
        }
        int index = -1;
        for (int i = 0; i < size; i++) {
            if (keys[i] == key) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            return;
        }

        for (int i = index; i < size - 1; i++) {
            keys[i] = keys[i + 1];
            values[i] = values[i + 1];
        }
        keys[size - 1] = 0;
        values[size - 1] = 0;
        size--;
    }

    void clear() {
        keys.clear();
        values.clear();
        size = 0;
    }

    int* getKeys() {
        return keys.data();
    }

    void increaseKeys(int* keys, int length) {
        for (int i = size; i < size + length; i++) {
            if (keys[i - size] == 0) {
                return;
            }
            this->keys[i] = keys[i - size];
        }
    }

    int get(int key) {
        for (int i = 0; i < size; i++) {
            if (keys[i] == key) {
                return values[i];
            }
        }
        return 0;
    }

    ~Bucket() {
        keys.clear();
        values.clear();
    }
};

class ExtendibleHash {
private:
    vector<Bucket*> directory;
    int global_depth;

    int get_index(int key) const {
        int index = 0;
        int mask = 1 << (global_depth - 1);
        for (int i = global_depth - 1; i >= 0; --i) {
            if (key & mask) {
                index |= (1 << i);
            }
            mask >>= 1;
        }
        return index;
    }

    void expand_directory() {
        vector<Bucket*> new_directory(directory.size() * 2);
        for (size_t i = 0; i < directory.size(); ++i) {
            new_directory[i] = directory[i];
            new_directory[i + directory.size()] = directory[i];
        }
        directory = new_directory;
    }

    void rehash(Bucket* bucket, int local_depth) {
        int new_local_depth = local_depth + 1;

        vector<Bucket*> new_buckets(2);
        for (size_t i = 0; i < 2; ++i) {
            new_buckets[i] = new Bucket(bucket->keys.size(), new_local_depth);
        }

        for (size_t i = 0; i < bucket->size; ++i) {
            int key = bucket->keys[i];
            int value = bucket->values[i];
            int index = get_index(key);
            new_buckets[(key >> local_depth) & 1]->insert(key, value);
        }

        int directory_size = 1 << global_depth;
        for (int i = 0; i < directory_size; ++i) {
            if (directory[i] == bucket) {
                directory[i] = new_buckets[(i >> local_depth) & 1];
            }
        }

        delete bucket;
    }

public:
    ExtendibleHash(int bucketSize, int global_depth) :global_depth(global_depth), directory(1 << global_depth) {
        for (size_t i = 0; i < directory.size(); ++i) {
            directory[i] = new Bucket(bucketSize, global_depth);
        }
    }

    void insert(int key, int value) {
        int index = get_index(key);
        Bucket* bucket = directory[index];

        if (!bucket->isFull()) {
            bucket->insert(key, value);
        }
        else {
            if (bucket->localDepth == global_depth) {
                expand_directory();
                global_depth++;
            }
            rehash(bucket, bucket->localDepth);
            insert(key, value);
        }
    }

    void print() const {
        for (Bucket* bucket : directory) {
            cout << "Bucket: " << endl;
            for (int i = 0; i < bucket->size; i++) {
                cout << bucket->keys[i] << " " << bucket->values[i] << endl;
            }
            cout << "Local depth: " << bucket->localDepth << std::endl;
        }
    }
};

int main() {
    srand(time(NULL));
    ExtendibleHash hash(2, 1);
    clock_t start = clock();
    for (int i = 0; i < 1000; i++)
    {
        hash.insert(i, i);
    }
    clock_t end = clock();
    double time = (double)(end - start);
    printf("The time: %f miliseconds\n", time);

    return 0;
}
/*
for (int i = 0; i < 1000000; i++)
    {
        int key;
        if (rand() % 2)
            key = rand() % 1000000;
        else key = rand() % 1000;
        hash.insert(key, i);
    }
*/
/*
for (int i = 0; i < 1000000; i++)
    {
        int key = rand() % 1000000;
        hash.insert(key, i);
    }
*/
