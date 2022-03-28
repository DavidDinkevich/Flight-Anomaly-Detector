# Flight Anomaly Detector
Interactive client-server platform that detects anomalies in flight data. <br>
Clients can upload the data (e.g. altitude, speed, etc.) of a proper flight, and the server will analyze and learn patterns in the data. The client can then upload the data of another flight (in real time), and the server will notify the client of any anomalies in the new flight.
<br><br>
<b>Design Goals and Paradigms:</b>
* Maximizing code reuse and modularity by employing principles/design patterns such as:
  * The Command design pattern
  * The open/closed principle
* Seamlessly handle multiple clients with multithreading
* Leveraging the memory semantics of C++ to minimize latency 
***
