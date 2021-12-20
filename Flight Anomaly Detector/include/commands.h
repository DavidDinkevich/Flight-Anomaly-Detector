

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>

#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO{
public:
	virtual string read()=0;
	virtual void write(string text)=0;
	virtual void write(float f)=0;
	virtual void read(float* f)=0;
	virtual ~DefaultIO(){}

    void writeLine(const string& text) {
        write(text + "\n");
    }

    string readFile() {
        string res, inp;
        while (!(inp = read()).empty() && inp.find("done") == string::npos)
            res += inp + "\n";
        return res;
    }

	// you may add additional methods here
};

struct ReportInfo {
    vector<AnomalyReport> anoms;
    HybridAnomalyDetector had;
};

// you may edit this class
class Command {
protected:
    DefaultIO* dio;
public:
	Command(DefaultIO* dio):dio(dio){}
	virtual void execute()=0;
	virtual ~Command(){}
    static string formatFloat(float f) {
        string s = to_string(f).substr(0, 5);
        while (s[s.size() - 1] == '0' || s[s.size() - 1] == '.')
            s = s.substr(0, s.size() - 1);
        return f == 0.f ? "0" : s;
    }

};

// implement here your command classes

class UploadFileCommand : public Command {
private:
    const char *fileName;
public:
    explicit UploadFileCommand(DefaultIO *dio, const char *fileName)
                            : Command(dio), fileName(fileName) {}
    void execute() override {
        ofstream os(fileName);
        string fileData = dio->readFile();
        os << fileData;
        os.close();
    }
};

class Option1Command : public Command {
public:
    explicit Option1Command(DefaultIO *dio) : Command(dio) {}
    void execute() override {
        dio->write("Please upload your local train CSV file.\n");
        UploadFileCommand trainUploader(dio, "traincsvfile.csv");
        trainUploader.execute();
        dio->writeLine("Upload complete.");
        dio->writeLine("Please upload your local test CSV file.");
        UploadFileCommand testUploader(dio, "testcsvfile.csv");
        testUploader.execute();
        dio->writeLine("Upload complete.");
    }
};

class Option2Command : public Command {
private:
    HybridAnomalyDetector& hyb;
public:
    Option2Command(DefaultIO *dio, HybridAnomalyDetector& hyb) : Command(dio), hyb(hyb) {}
    void execute() override {
        dio->writeLine("The current correlation threshold is " + formatFloat(hyb.getMinCorrelation()));
        dio->writeLine("Type a new threshold");
        string newThresh = dio->read();
        float thr = stof(newThresh);
        while (thr > 1 || thr < 0) {
            dio->writeLine("please choose a value between 0 and 1.");
            newThresh = dio->read();
            thr = stof(newThresh);
        }
        hyb.setMinCorrelation(thr);



//        do {
//            string newCorr = dio->read();
//            char *ending;
//            float corrAsFloat = strtof(newCorr.c_str(), &ending);
//            if (*ending != 0)
//                dio->writeLine("please choose a value between 0 and 1.");
//            else if (corrAsFloat >= 0.f && corrAsFloat <= 1.f) {
//                hyb.setMinCorrelation(corrAsFloat);
//                break;
//            }
//        } while (true);
    }
};

class Option3Command : public Command {
private:
    ReportInfo& repInfo;
public:
    Option3Command(DefaultIO *dio, ReportInfo& repInfo)
                    : Command(dio), repInfo(repInfo) {}
    void execute() override {
        TimeSeries tsTrain("traincsvfile.csv");
        repInfo.had.learnNormal(tsTrain);
        TimeSeries tsTest("testcsvfile.csv");
        repInfo.anoms = repInfo.had.detect(tsTest);
        dio->writeLine("anomaly detection complete.");
    }
};

class Option4Command : public Command {
private:
    ReportInfo& repInfo;
public:
    Option4Command(DefaultIO *dio, ReportInfo& repInfo)
                    : Command(dio), repInfo(repInfo) {}
    void execute() override {
        for (const AnomalyReport& ar : repInfo.anoms) {
            dio->writeLine(to_string(ar.timeStep) + "\t" + ar.description);
        }
        dio->writeLine("Done.");
    }
};

class Option5Command : public Command {
private:
    ReportInfo& repInfo;
private: // methods
    vector<pair<int, int>> getAnomalyIntervals() {
        vector<pair<int, int>> intervals;
        if (repInfo.anoms.empty())
            return intervals;
        const AnomalyReport *prevRep = &repInfo.anoms[0];
        long intervalStart = repInfo.anoms[0].timeStep;
        for (auto it = ++repInfo.anoms.begin(); it != repInfo.anoms.end(); ++it) {
            const AnomalyReport& rep = *it;
            if (rep.timeStep - 1 != prevRep->timeStep || rep.description != prevRep->description) {
                intervals.emplace_back(make_pair(intervalStart, prevRep->timeStep));
                intervalStart = rep.timeStep;
            }
            prevRep = &rep;
        }
        intervals.emplace_back(make_pair(intervalStart, prevRep->timeStep));
        return intervals;
    }
public:
    Option5Command(DefaultIO *dio, ReportInfo& repInfo)
                    : Command(dio), repInfo(repInfo) {}
    void execute() override {
        dio->writeLine("Please upload your local anomalies file.");
        vector<pair<int, int>> myIntervals = getAnomalyIntervals();
        vector<pair<int, int>> userIntervals;
        string inp;
        while (!(inp = dio->read()).empty() && inp.find("done") == string::npos) {
            int first = stoi(inp.substr(0, inp.find(',')));
            int second = stoi(inp.substr(inp.find(',') + 1, inp.find('\n')));
            userIntervals.emplace_back(make_pair(first, second));
        }
        dio->writeLine("Upload complete.");

        int sumUserIntervals = 0;
        for (const auto& userInt : userIntervals)
            sumUserIntervals += userInt.second - userInt.first + 1;

        int numTruePositives = 0, numFalsePositives = 0;
        for (const auto& myInt : myIntervals) {
            int prevNumTruePositives = numTruePositives;
            for (const auto& userInt : userIntervals) {
                if ((myInt.first >= userInt.first && myInt.first <= userInt.second)
                || (myInt.second >= userInt.first && myInt.second <= userInt.second)
                || (myInt.first <= userInt.first && myInt.second >= userInt.second)) {
                    numTruePositives++;
                }
            }
            if (prevNumTruePositives == numTruePositives)
                numFalsePositives++;
        }

        dio->writeLine("True Positive Rate: " + formatFloat((float)numTruePositives / userIntervals.size()));

        TimeSeries ts("testcsvfile.csv");
        dio->writeLine("False Positive Rate: " + formatFloat(numFalsePositives / ((float) ts.getDataLength() - sumUserIntervals)));
    }
};

#endif /* COMMANDS_H_ */
