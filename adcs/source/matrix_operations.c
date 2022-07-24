#include "matrix_operations.h"

#include <math.h>

void vector_subtraction(float *vector1, float *vector2, float *outputVector, uint8_t length){
    for ( int i = 0; i < length; i ++ ) {
        outputVector[i] = vector1[i] - vector2[i];
    }
}

void scalar_vector_multiply(float *vector, float scalar, float *outputVector, uint8_t length){
    for ( int i = 0; i < length; i++ ) {
        outputVector[i] = vector[i]*scalar;
    }
}

void scalar_matrix_3x3_multiply(float matrix[3][3], float scalar, float outputMatrix[3][3]){
    for ( int i = 0; i < 3; i++ ) {
        for ( int j = 0; j < 3; j++ ) {
            outputMatrix[i][j] = matrix[i][j]*scalar;
        }
    }
}

void vector_matrix_3x3_multiply(float matrix[3][3], float vector[3], float outputVector[3]){
    for ( int i = 0; i < 3; i ++ ) {
        float currentSum = 0;
        for ( int j = 0; j < 3; j++ ) {
            currentSum += vector[i]*matrix[i][j];
        }
        outputVector[i] = currentSum;
    }
}

float vector_norm(float *vector, uint8_t length){
    float output = 0;
    for ( int i = 0; i < length; i ++ ){
        output += pow(vector[i], 2);
    }

    return sqrt(output);
}