#include "map.h"

float mapRange(char input, char inputMin, char inputMax, char outputMin, float outputMax) {

    // Calculamos el factor de escala
    char inputRange = inputMax - inputMin;
//    char outputRange = outputMax - outputMin;
    float scaleFactor = (outputMax - outputMin) / inputRange;

    // Realizamos el mapeo y devolvemos el resultado
    float result = outputMin + (input - inputMin) * scaleFactor;
    return result;
}