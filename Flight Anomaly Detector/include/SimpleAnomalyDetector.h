

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include "minCircle.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct CorrelatedPair {
    CorrelatedPair(const string& f1, const string& f2, float c)
        : feature1(f1), feature2(f2), corrlation(c) {}
    string feature1, feature2;
    float corrlation;
};

struct correlatedFeatures : public CorrelatedPair {
    correlatedFeatures(const string& f1, const string& f2, float c, Line lin_reg, float thresh, Circle mc)
        : CorrelatedPair(f1, f2, c), lin_reg(lin_reg), threshold(thresh), minCircle(mc) {}
    Line lin_reg;
    float threshold;
    Circle minCircle;
};

class SimpleAnomalyDetector : public TimeSeriesAnomalyDetector {
private:
    vector<CorrelatedPair> allCorrs;
protected:
    float minCorrelation = 0.9f;
    vector<correlatedFeatures> cf;
    void calculateAllCorrelations(const TimeSeries& ts);
public:
    SimpleAnomalyDetector();
    virtual ~SimpleAnomalyDetector();


    virtual void learnNormal(const TimeSeries& ts);
    virtual vector<AnomalyReport> detect(const TimeSeries& ts);

    void setMinCorrelation(float f) {
        minCorrelation = f;
    }

    vector<correlatedFeatures> getNormalModel(){
        return cf;
    }

    const vector<CorrelatedPair>& getAllCorrelations() const {
        return allCorrs;
    }

};



#endif /* SIMPLEANOMALYDETECTOR_H_ */