cmake_minimum_required(VERSION 3.20)
project(Flight_Anomaly_Detector)
include_directories(include)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(
        Flight_Anomaly_Detector
        src/MainTrain.cpp
        src/anomaly_detection_util.cpp include/anomaly_detection_util.h
        include/SimpleAnomalyDetector.h src/SimpleAnomalyDetector.cpp
        include/timeseries.h src/timeseries.cpp
        include/AnomalyDetector.h
        include/minCircle.h src/minCircle.cpp
        include/HybridAnomalyDetector.h src/HybridAnomalyDetector.cpp
        include/CLI.h src/CLI.cpp
        include/commands.h
        include/Server.h src/Server.cpp
)
