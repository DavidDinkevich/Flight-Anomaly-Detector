

#ifndef TIMESERIES_H_
#define TIMESERIES_H_

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class TimeSeries{
private:
    std::map<std::string, std::vector<float>> table;
    std::vector<std::string> featureNames;
private: // HELPER FUNCTIONS
    static std::vector<std::string> split(const std::string& str, const char *delim);
    void loadFile(const char *CSVfileName);
public:

	explicit TimeSeries(const char *CSVfileName);

    std::vector<std::string> getFeatureNames() const;

    std::vector<float> getFeatureValues(const char *name) const;

    int getNumFeatures() const;

    int getDataLength() const;


};



#endif /* TIMESERIES_H_ */
