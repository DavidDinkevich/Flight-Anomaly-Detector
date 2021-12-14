

#ifndef CLI_H_
#define CLI_H_

#include <string.h>
#include "commands.h"

using namespace std;

class CLI {
private: // variables
	DefaultIO* dio;
    float currCorrelationThreshold;
    TimeSeries ts;
    HybridAnomalyDetector hybridAnomalyDetector;
	// you can add data members
private: // methods
    void printMenu() const;

public:
	CLI(DefaultIO* dio);
	void start();
	virtual ~CLI();
};

#endif /* CLI_H_ */
