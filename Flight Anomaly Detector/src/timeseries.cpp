#include "timeseries.h"

using namespace std;

TimeSeries::TimeSeries() = default;

TimeSeries::TimeSeries(const char *CSVfileName) {
    loadFile(CSVfileName);
}


vector<string> TimeSeries::split(const std::string& str, const char * delim) {
    vector<string> chunks;
    string line = str;
    int end;
    do {
        end = (int) line.find(',');
        chunks.emplace_back(line.substr(0, end));
        line = line.substr(end + 1, line.size());
    } while (end != -1);

    return chunks;
}

std::vector<std::string> TimeSeries::getFeatureNames() const {
    return { featureNames };
}

void TimeSeries::loadFile(const char *CSVfileName) {
    std::ifstream is(CSVfileName);

    bool firstLine = true;

    for (std::string line; getline(is, line);) {
        // Split line by commas
        std::vector<std::string> chunks = split(line, ",");
        // Special process for first line
        if (firstLine) {
            firstLine = false;
            // Create column vector
            for (auto& chunk : chunks) {
                featureNames.emplace_back(chunk); // Add as feature name
                // Initialize vector
                table[chunk] = std::vector<float>();
            }
        } else {
            // Populate table
            for (int i = 0; i < chunks.size(); i++) {
                table[featureNames[i]].push_back(std::stof(chunks[i]));
            }
        }
    }

//    for (auto& name : featureNames) {
//        std::cout << name << ": ";
//        for (auto& field : table[name]) {
//            std::cout << field << ", ";
//        }
//        std::cout << std::endl;
//    }

    is.close();
}

std::vector<float> TimeSeries::getFeatureValues(const char *name) const {
    return { table.at(std::string(name)) };
}

int TimeSeries::getNumFeatures() const {
    return (int) table.size();
}

int TimeSeries::getDataLength() const {
    if (getNumFeatures() == 0)
        return 0;
    return (int) table.at(featureNames[0]).size();
}

