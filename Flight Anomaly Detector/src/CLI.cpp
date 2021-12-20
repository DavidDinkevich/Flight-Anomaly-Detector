#include "CLI.h"

CLI::CLI(DefaultIO* dio) : dio(dio), currCorrelationThreshold(0.9f){
}

CLI::~CLI() = default;

void CLI::printMenu() const {
    dio->write("Welcome to the Anomaly Detection Server.\n"
               "Please choose an option:\n"
               "1.upload a time series csv file\n"
               "2.algorithm settings\n"
               "3.detect anomalies\n"
               "4.display results\n"
               "5.upload anomalies and analyze results\n"
               "6.exit\n");
}

void CLI::start() {
    printMenu();

    ReportInfo repInfo;

    Command *options[] = {
            new Option1Command(dio),
            new Option2Command(dio, hybridAnomalyDetector),
            new Option3Command(dio, repInfo),
            new Option4Command(dio, repInfo),
            new Option5Command(dio, repInfo)
    };
    string inp;
    while (!(inp = dio->read()).empty() && inp != "6") {
        options[stoi(inp) - 1]->execute();
        printMenu();
    }
    remove("traincsvfile.csv");
    remove("testcsvfile.csv");

    for (auto& op : options)
        delete op;
}

