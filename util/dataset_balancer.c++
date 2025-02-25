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
    double difficulty;
    int bin;
};

// Utility to compute a quantile from a sorted vector.
double compute_quantile(const vector<double>& sorted, double quantile) {
    size_t index = quantile * (sorted.size() - 1);
    return sorted[index];
}

int main() {
    string input_filename = "data.jsonl";
    ifstream infile(input_filename);
    if (!infile.is_open()){
        cerr << "Error opening input file: " << input_filename << endl;
        return 1;
    }
    
    vector<DataSample> samples;
    string line;
    getline(infile, line); // Skip metadata line
    
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
        
        if(j.contains("num_solutions") && j["num_solutions"].is_number()){
            DataSample sample;
            sample.data = j;
            sample.difficulty = j["num_solutions"].get<double>();
            samples.push_back(sample);
        }
    }
    infile.close();
    
    if(samples.empty()){
        cerr << "No valid samples found." << endl;
        return 1;
    }
    
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
        for (int i = numBins - 2; i >= 0; i--) {
            if (s.difficulty <= quantiles[i]) {
                categorized_samples[i].push_back(s);
                break;
            }
        }
    }
    
    double test_ratio = 0.2;
    random_device rd;
    mt19937 rng(rd());
    
    auto stratified_split = [&](const vector<DataSample>& bin_samples,
                                vector<DataSample>& train, vector<DataSample>& test) {
        vector<DataSample> shuffled = bin_samples;
        shuffle(shuffled.begin(), shuffled.end(), rng);
        size_t test_count = static_cast<size_t>(shuffled.size() * test_ratio);
        test.insert(test.end(), shuffled.begin(), shuffled.begin() + test_count);
        train.insert(train.end(), shuffled.begin() + test_count, shuffled.end());
    };
    
    vector<DataSample> train_samples, test_samples;
    stratified_split(easy_samples, train_samples, test_samples);
    stratified_split(medium_samples, train_samples, test_samples);
    stratified_split(hard_samples, train_samples, test_samples);
    
    // Write training samples to a JSONL file.
    ofstream train_file("train.jsonl");
    // Write testing samples to a JSONL file.
    ofstream test_file("test.jsonl");
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
    
    cout << "Stratified sampling complete." << endl;
    cout << "Train samples: " << train_samples.size() 
         << ", Test samples: " << test_samples.size() << endl;
    
    return 0;
}
