#include <TensorFlowLite.h>
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

// Aqui vai o modelo treinado em formato .h
#include "model.h"
// O arquivo que contém o dummyData
#include "dummy.h"  

const int numSamples = 20;  // O número de amostras por gesto (ajuste conforme necessário)


// Definições para o TensorFlow Lite (Micro)
const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;

TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));

const char* GESTURES[] = {"caminhando", "pulando", "parado"};
#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Inicialização do TensorFlow Lite (Micro)
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1);
  }

  static tflite::MicroMutableOpResolver<4> micro_op_resolver;
  micro_op_resolver.AddFullyConnected();  
  micro_op_resolver.AddRelu();           
  micro_op_resolver.AddSoftmax();         

  static tflite::MicroInterpreter static_interpreter(tflModel, micro_op_resolver, tensorArena, tensorArenaSize);
  interpreter = &static_interpreter;
  interpreter->AllocateTensors();

  tflInputTensor = interpreter->input(0);
  tflOutputTensor = interpreter->output(0);
}

void loop() {
  // Preenche os dados de entrada com as amostras simuladas
  for (int blockIndex = 0; blockIndex < 12; blockIndex++) {  // 12 blocos de 20 amostras
    int baseIndex = blockIndex * numSamples;  // Base index para cada bloco de 20 amostras

    // Preenche as 20 amostras para este bloco
    for (int sampleIndex = 0; sampleIndex < numSamples; sampleIndex++) {
      int dataIndex = baseIndex + sampleIndex;  // Índice global para os dados simulados

      // Preenche o tensor de entrada com os dados de cada amostra
      tflInputTensor->data.f[sampleIndex * 6 + 0] = (dummyData[dataIndex][0] + 4.0) / 8.0;  // Aceleração X
      tflInputTensor->data.f[sampleIndex * 6 + 1] = (dummyData[dataIndex][1] + 4.0) / 8.0;  // Aceleração Y
      tflInputTensor->data.f[sampleIndex * 6 + 2] = (dummyData[dataIndex][2] + 4.0) / 8.0;  // Aceleração Z
      tflInputTensor->data.f[sampleIndex * 6 + 3] = (dummyData[dataIndex][3] + 2000.0) / 4000.0;  // Giroscópio X
      tflInputTensor->data.f[sampleIndex * 6 + 4] = (dummyData[dataIndex][4] + 2000.0) / 4000.0;  // Giroscópio Y
      tflInputTensor->data.f[sampleIndex * 6 + 5] = (dummyData[dataIndex][5] + 2000.0) / 4000.0;  // Giroscópio Z
    }

    // Realiza a inferência para o bloco atual de 20 amostras
    TfLiteStatus invokeStatus = interpreter->Invoke();
    if (invokeStatus != kTfLiteOk) {
      Serial.println("Invoke failed!");
      while (1);
      return;
    }

    // Processa os resultados da inferência para este bloco
    float maxScore = 0;
    int maxIndex = -1;
    for (int i = 0; i < NUM_GESTURES; i++) {
      float score = tflOutputTensor->data.f[i];
      Serial.print(GESTURES[i]);
      Serial.print(": ");
      Serial.println(score, 6);

      if (score > maxScore) {
        maxScore = score;
        maxIndex = i;
      }
    }

    // Exibe o gesto detectado para o bloco
    if (maxIndex != -1) {
      Serial.print("Gesto detectado: ");
      Serial.println(GESTURES[maxIndex]);
    }

    delay(1000);  // Aguarda 1 segundo antes de executar novamente
  }
}
