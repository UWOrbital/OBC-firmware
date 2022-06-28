#ifndef ADCS_BDOT_H
#define ADCS_BDOT_H

typedef struct{
    /**
     * a 3 by 3 matrix
     * 
     */
    float matrix[3][3];

} matrix_t;

typedef struct{
    /**
     * magnetic field value expressed in the Body Frame
     */
    float magneticFieldBody[3];

    float bdotVector[3];

    /**
     * proportional positive scalar gain
     */
    matrix_t Kp;

    /**
     * Maximum magnetic dipole of magnetorquers (in Am^2)
     */
    float maxMagneticDipole[3];

    /**
     * derivation value equal to the time-period elapsed between two measurements of the magnetic field (in sec)
     */
    float timeStep;
    
} bdot_t;

/**
 * Acts as the constructor
 * @param magneticFieldBody magnetic field value expressed in the Body Frame
 * @param Kp proportional positive scalar gain
 * @param maxMagneticDipole magnetorquers maximum magnetic dipole (in Am^2)
 * @param timeStep derivation value (in sec)
 */
void bdot_init(float magneticFieldBody[3], float Kp[3][3], float maxMagneticDipole[3], float timeStep);

/**
 * @brief bdot controller is used in Detumbling mode, during which actuation is performed by the magnetorquers, and
 *        utilizes only magnetometer data.
 *        It calculates a magnetic dipole opposite to the derivative of the magnetic field.
 * 
 * @param magneticFieldBody magnetic field value expressed in the Body Frame
 * @param magneticDipole magnetic dipole array to be outputed (in Am^2)
 */
void bdot_controller(float magneticFieldBody[3], float magneticDipole[3]);

/**
 * @brief Scaling of the desired magnetic dipole in case it exceeds the maximum dipole provided by each magnetorquer
 * 
 * @param magneticDipole desired magnetic dipole (in Am^2)
 * @return scaled magnetic dipole (in Am^2)
 */
float *bdot_magnetorquer_scaling(float magneticDipole[3]);

/**
 * @brief magnetic field value expressed in the Body Frame getter
 * 
 * @return magnetic field value expressed in the Body Frame
 */
float *bdot_get_magnetic_field_body(void);

/**
 * @brief magnetic field value expressed in the Body Frame setter
 * 
 * @param magneticFieldBody magnetic field value expressed in the Body Frame
 */
void bdot_set_magnetic_field_body(float magneticFieldBody[3]);

/**
 * @brief bdot vector getter
 * 
 * @return bdot vector
 */
float *bdot_get_bdot_vector(void);

/**
 * @brief bdot vector setter
 * 
 * @param bdotVector bdot vector
 */
void bdot_set_bdot_vector(float bdotVector[3]);

/**
 * @brief proportional positive scalar gain getter
 * 
 * @return proportional positive scalar gain
 */
matrix_t bdot_get_kp(void);

/**
 * @brief proportional positive scalar gain setter
 * 
 * @param Kp proportional positive scalar gain
 */
void bdot_set_kp(float Kp[3][3]);

/**
 * @brief Maximum magnetic dipole of magnetorquers (in Am^2) getter
 * 
 * @return Maximum magnetic dipole of magnetorquers (in Am^2)
 */
float *bdot_get_max_magnetic_dipole(void);

/**
 * @brief Maximum magnetic dipole of magnetorquers (in Am^2) setter
 * 
 * @param maxMagneticDipole Maximum magnetic dipole of magnetorquers (in Am^2)
 */
void bdot_set_max_magnetic_dipole(float maxMagneticDipole[3]);

/**
 * @brief derivation value getter
 * 
 * @return derivation value
 */
float bdot_get_time_step(void);

/**
 * @brief derivation value setter
 * 
 * @param timeStep derivation value
 */
void bdot_set_time_step(float timeStep);

#endif //ADCS_BDOT_H