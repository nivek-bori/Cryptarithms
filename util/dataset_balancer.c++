#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

const int numBins = 3;

// Structure to hold each sample with its difficulty and bin label.
struct DataSample {
    json data;
    int difficulty;
};

// Utility to compute a quantile from a sorted vector.
double compute_quantile(const vector<double>& sorted, double quantile) {
    size_t index = quantile * (sorted.size() - 1);
    return sorted[index];
}

int main() {
    string input_filename = "outputs/data.jsonl";
    ifstream infile(input_filename);
    if (!infile.is_open()){
        cerr << "Error opening input file: " << input_filename << endl;
        return 1;
    }
    
    vector<DataSample> samples;
    string line;
    
    // Read all lines of json data
    while(getline(infile, line)) {
        if(line.empty()) continue;
        json j;
        try {
            j = json::parse(line);
        } catch(const std::exception& e) {
            cerr << "Error parsing line: " << e.what() << endl;
            continue;
        }
        
        if (j.contains("num_solutions") && j["num_solutions"].is_number()) {
                DataSample sample;
                sample.data = j;
                sample.difficulty = j["num_solutions"].get<int>();
                samples.push_back(sample);
        }
    }
    infile.close();
    
    if(samples.empty()){
        cerr << "No valid samples found." << endl;
        return 1;
    }
    
    // Sorting difficulty
    vector<double> difficulties;
    for (const auto& s : samples) {
        difficulties.push_back(s.difficulty);
    }
    sort(difficulties.begin(), difficulties.end());
    
    vector<double> quantiles(numBins - 1);
    for (int i = 0; i < numBins - 1; i++) {
        quantiles[i] = compute_quantile(difficulties, (i + 1) / static_cast<double>(numBins));
    }
    
    vector<vector<DataSample>> categorized_samples(numBins);
    
    for(auto &s : samples) {
        int bin = 0;  // Default to lowest bin
        for (int i = 0; i < numBins - 1; i++) {
            if (s.difficulty > quantiles[i]) {
                bin = i + 1;
            }
        }
        categorized_samples[bin].push_back(s);
    }
    
    double test_ratio = 0.2;
    random_device rd;
    mt19937 rng(rd());
    
    auto stratified_split = [&](const vector<DataSample>& bin_samples,
                                vector<DataSample>& train, vector<DataSample>& test) {
        vector<DataSample> shuffled = bin_samples;
        shuffle(shuffled.begin(), shuffled.end(), rng);
        size_t test_count = round(shuffled.size() * test_ratio);
        test.insert(test.end(), shuffled.begin(), shuffled.begin() + test_count);
        train.insert(train.end(), shuffled.begin() + test_count, shuffled.end());
    };
    
    vector<DataSample> train_samples, test_samples;
    for (int i = 0; i < numBins; i++) {
        stratified_split(categorized_samples[i], train_samples, test_samples);
    }

    ofstream train_file("outputs/train.jsonl", ios::trunc);
    ofstream test_file("outputs/test.jsonl", ios::trunc);
    if(!train_file.is_open() || !test_file.is_open()){
        cerr << "Error opening output file(s)." << endl;
        return 1;
    }
    
    for(const auto &s : train_samples) {
        train_file << s.data.dump() << "\n";
    }
    for(const auto &s : test_samples) {
        test_file << s.data.dump() << "\n";
    }
    
    train_file.close();
    test_file.close();
    
    clog << "Train samples: " << train_samples.size() << ", Test samples: " << test_samples.size() << endl;
    clog << "TASK END" << endl;
    return 0;
}
