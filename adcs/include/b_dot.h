#ifndef ADCS_ONBOARD_SOFTWARE_BDOT_H
#define ADCS_ONBOARD_SOFTWARE_BDOT_H


typedef struct BdotStruct{
    /**
     * magnetic field value expressed in the Body Frame
     */
    float magneticFieldBody[3];

    float bDotVector[3];

    /**
     * proportional positive scalar gain
     */
    float Kp[3][3];

    /**
     * Maximum magnetic dipole of magnetorquers (in Am^2)
     */
    float maxMagneticDipole[3];

    /**
     * derivation value equal to the time-period elapsed between two measurements of the magnetic field (in sec)
     */
    float timeStep;
    
} BdotStruct;

BdotStruct Bdot;

/**
 * Acts as the constructor
 * @param magneticFieldBody magnetic field value expressed in the Body Frame
 * @param Kp proportional positive scalar gain
 * @param maxMagneticDipole magnetorquers maximum magnetic dipole (in Am^2)
 * @param timeStep derivation value (in sec)
 */
void Bdot_init(float magneticFieldBody[3], float Kp[3][3], float maxMagneticDipole[3], float timeStep);

/**
 * @brief Bdot controller is used in Detumbling mode, during which actuation is performed by the magnetorquers, and
 *        utilizes only magnetometer data.
 *        It calculates a magnetic dipole opposite to the derivative of the magnetic field.
 * 
 * @param magneticFieldBody magnetic field value expressed in the Body Frame
 */
void Bdot_controller(float magneticFieldBody[3], float magneticDipole);

/**
 * @brief Scaling of the desired magnetic dipole in case it exceeds the maximum dipole provided by each magnetorquer
 * 
 * @param magneticDipole desired magnetic dipole (in Am^2)
 * @return scaled magnetic dipole (in Am^2)
 */
float *Bdot_magnetorquer_scaling(float magneticDipole[3]);

/**
 * @brief magnetic field value expressed in the Body Frame getter
 * 
 * @return magnetic field value expressed in the Body Frame
 */
float *Bdot_get_magnetic_field_body(void){
    return Bdot.magneticFieldBody;
}

/**
 * @brief magnetic field value expressed in the Body Frame setter
 * 
 * @param magneticFieldBody magnetic field value expressed in the Body Frame
 */
void Bdot_set_magnetic_field_body(float magneticFieldBody[3]){
    for (int i = 0; i < 3; i++){
        Bdot.magneticFieldBody[i] = magneticFieldBody[i];
    }
}

/**
 * @brief Bdot vector getter
 * 
 * @return Bdot vector
 */
float *Bdot_get_Bdot_vector(void){
    return Bdot.bDotVector;
}

/**
 * @brief Bdot vector setter
 * 
 * @param bDotVector Bdot vector
 */
void Bdot_set_Bdot_vector(float bDotVector[3]){
    for (int i = 0; i < 3; i++){
        Bdot.bDotVector[i] = bDotVector[i];
    }
}

/**
 * @brief proportional positive scalar gain getter
 * 
 * @return proportional positive scalar gain
 */
float **Bdot_get_kp(void){
    return Bdot.Kp;
}

/**
 * @brief proportional positive scalar gain setter
 * 
 * @param Kp proportional positive scalar gain
 */
void Bdot_set_kp(float Kp[3][3]){
    for (int i = 0; i < 3;i ++){
        for (int j = 0; j < 3; j++){
            Bdot.Kp[i][j] = Kp[i][j];
        }
    }
    
}

/**
 * @brief Maximum magnetic dipole of magnetorquers (in Am^2) getter
 * 
 * @return Maximum magnetic dipole of magnetorquers (in Am^2)
 */
float *Bdot_get_max_magnetic_dipole(void){
    return Bdot.maxMagneticDipole;
}

/**
 * @brief Maximum magnetic dipole of magnetorquers (in Am^2) setter
 * 
 * @param maxMagneticDipole Maximum magnetic dipole of magnetorquers (in Am^2)
 */
void Bdot_set_max_magnetic_dipole(float maxMagneticDipole[3]){
    for (int i = 0; i < 3; i++){
        Bdot.maxMagneticDipole[i] = maxMagneticDipole[i];
    }
}

/**
 * @brief derivation value getter
 * 
 * @return derivation value
 */
float Bdot_get_time_step(void){
    return Bdot.timeStep;
}

/**
 * @brief derivation value setter
 * 
 * @param timeStep derivation value
 */
void Bdot_set_time_step(float timeStep){
    Bdot.timeStep = timeStep;
}

#endif //ADCS_ONBOARD_SOFTWARE_BDOT_H