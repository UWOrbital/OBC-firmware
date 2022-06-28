#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

/**
 * @brief Subtracts two vectors
 * 
 * @param vector1 First vector
 * @param vector2 Second vector
 * @param outputVector The vector storing the output
 */
void vector_subtraction(float vector1[3], float vector2[3], float outputVector[3]);

/**
 * @brief Multiplication of a vector with a scalar value
 * 
 * @param vector The vector being multiplied
 * @param scalar The scalar value being multiplied
 * @param outputVector The vector storing the output
 */
void scalar_vector_multiply(float vector[3], float scalar, float outputVector[3]);

/**
 * @brief Multiplication of a matrix with a scalar
 * 
 * @param matrix The matrix being multiplied
 * @param scalar The scalar value being multiplied
 * @param outputMatrix The matrix storing the output
 */
void scalar_multiply(float matrix[3][3], float scalar, float outputMatrix[3][3]);

/**
 * @brief Multiplication of a vector of size 1x3 and a matrix of size 3x3
 * 
 * @param matrix The matrix being multiplied
 * @param vector The vector being multiplied
 * @param outputVector The vector storing the output
 */
void vector_multiply(float matrix[3][3], float vector[3], float outputVector[3]);

/**
 * @brief Calculates the "Frobenius Norm" for a 1x3 vector, where its the square root
 * of the sum of all the elements in a matrix squared
 *  * 
 * @param vector The vector for which the Norm value is found
 * @return The Frobenius Norm of the input vector
 */
float vector_norm(float vector[3]);

#endif //MATRIX_OPERATIONS_H