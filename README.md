# Human Activity Recognition (HAR) using Embedded Devices

This project explores **Human Activity Recognition (HAR)**, a technology designed to identify basic movements such as walking, running, and standing still. HAR has applications in areas like sports monitoring, healthcare, and the supervision of elderly people and children. Traditionally, HAR data processing is done in cloud servers, which come with drawbacks such as high energy consumption, high costs, and reliance on stable internet connections.

This study investigates the feasibility of implementing HAR directly on embedded devices, focusing on three specific activities: walking, jumping, and standing still. The approach leverages machine learning models implemented with **LiteRT (TensorFlow Lite)**, enabling efficient execution on hardware with limited resources. The proof of concept demonstrates the potential for real-time activity recognition on embedded systems, emphasizing the efficiency of edge AI for local inferences without the need for cloud processing.

## Data Collection

Movement data was collected using four wearable devices equipped with the **BNO080 inertial sensor**, which reads accelerometer, gyroscope, and magnetometer data with 9 degrees of freedom. Each device was connected to a **NodeMCU ESP-32 microcontroller** powered by a lithium-ion battery. Devices were placed in various positions on the legs to capture comprehensive movement data. Data transmission was performed via Bluetooth at 20 Hz, i.e., one sample every 50 ms.

### Recorded Activities
The collected data included the following activities:

| Label      | Description                            |
|------------|----------------------------------------|
| Walking    | Data collected during walking indoors |
| Jumping    | Data collected during jumping indoors |
| Standing   | Data collected while standing still   |

## Data Processing

Each device recorded approximately 1,000 samples per activity, sampled at a frequency of 20 Hz (1 sample every 50 ms). The data were processed, including noise removal and normalization, to make them compatible with the machine learning model.

Graphs of the collected data show how the activities vary. For example, walking shows significant variations across all three axes, with typical cyclical oscillations. Standing still shows stable readings near zero, and jumping exhibits peak data, especially on the y-axis, with more abrupt variations.

## Model Training and Conversion

A neural network model was implemented using **TensorFlow/Keras** to classify the activities of walking, standing still, and jumping. The model uses six features per sample (accelerometer and gyroscope data), and its structure consists of dense layers with ReLU activation and an output layer with Softmax activation.

The model was compiled using the **Adam optimizer** and the **categorical crossentropy** loss function. Training involved up to 500 epochs with a batch size of 32, monitoring accuracy and mean absolute error (MAE). After evaluating the model with test data, it was converted to **TensorFlow Lite (TFLite)** format for embedded devices. The model was then transformed into a byte array in C format and stored in a header file (`model.h`) for integration into the firmware of embedded systems.

## Deployment on the Device

The model was deployed on an **Arduino Nano 33 BLE** board, which features an **Arm® Cortex®-M4F** processor running at 64 MHz with 1 MB of Flash memory and 256 kB of RAM. This board is ideal for AIoT (Artificial Intelligence and Internet of Things) applications due to its low power consumption and Bluetooth Low Energy (BLE) connectivity.
