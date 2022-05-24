#ifndef ADCS_ONBOARD_SOFTWARE_BDOT_H
#define ADCS_ONBIARD_SOFTWARE_BDOT_H


typedef struct Bdot_Struct{
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
    float timestep;
    
} Bdot_Struct;

Bdot_Struct Bdot;

/**
 * Acts as the constructor
 * @param magneticFieldBody magnetic field value expressed in the Body Frame
 * @param Kp proportional positive scalar gain
 * @param maxMagneticDipole magnetorquers maximum magnetic dipole (in Am^2)
 * @param timestep derivation value (in sec)
 */
void Bdot_init(float magneticFieldBody[3], float Kp[3][3], float maxMagneticDipole[3], float timestep);

/**
 * Bdot controller is used in Detumbling mode, during which actuation is performed by the magnetorquers, and
 * utilizes only magnetometer data.
 * It calculates a magnetic dipole opposite to the derivative of the magnetic field.
 * @param magneticFieldBody magnetic field value expressed in the Body Frame
 * @return magnetic dipole value
 */
float *Bdot_controller(float magneticFieldBody[3]);

/**
 * Scaling of the desired magnetic dipole in case it exceeds the maximum dipole provided by each magnetorquer
 * @param magneticDipole desired magnetic dipole (in Am^2)
 * @return scaled magnetic dipole (in Am^2)
 */
float *Bdot_magnetorquerScaling(float magneticDipole[3]);

/**
 * magnetic field value expressed in the Body Frame getter
 * @return magnetic field value expressed in the Body Frame
 */
float *Bdot_getMagneticFieldBody(){
    return Bdot.magneticFieldBody;
}

/**
 * magnetic field value expressed in the Body Frame setter
 * @param magneticFieldBody magnetic field value expressed in the Body Frame
 */
void Bdot_setMagneticFieldBody(float magneticFieldBody[3]){
    for (int i = 0; i < 3; i++){
        Bdot.magneticFieldBody[i] = magneticFieldBody[i];
    }
}

/**
 * Bdot vector getter
 * @return Bdot vector
 */
float *Bdot_getBDotVector(){
    return Bdot.bDotVector;
}

/**
 * Bdot vector setter
 * @param bDotVector Bdot vector
 */
void Bdot_setBDotVector(float bDotVector[3]){
    for (int i = 0; i < 3; i++){
        Bdot.bDotVector[i] = bDotVector[i];
    }
}

/**
 * proportional positive scalar gain getter
 * @return proportional positive scalar gain
 */
float (*Bdot_getKp(void))[3]{
    return Bdot.Kp;
}

/**
 * proportional positive scalar gain setter
 * @param Kp proportional positive scalar gain
 */
void Bdot_setKp(float Kp[3][3]){
    for (int i = 0; i < 3;i ++){
        for (int j = 0; j < 3; j++){
            Bdot.Kp[i][j] = Kp[i][j];
        }
    }
    
}

/**
 * Maximum magnetic dipole of magnetorquers (in Am^2) getter
 * @return Maximum magnetic dipole of magnetorquers (in Am^2)
 */
float *Bdot_getMaxMagneticDipole(){
    return Bdot.maxMagneticDipole;
}

/**
 * Maximum magnetic dipole of magnetorquers (in Am^2) setter
 * @param maxMagneticDipole Maximum magnetic dipole of magnetorquers (in Am^2)
 */
void Bdot_setMaxMagneticDipole(float maxMagneticDipole[3]){
    for (int i = 0; i < 3; i++){
        Bdot.maxMagneticDipole[i] = maxMagneticDipole[i];
    }
}

/**
 * derivation value getter
 * @return derivation value
 */
float Bdot_getTimestep(){
    return Bdot.timestep;
}

/**
 * derivation value setter
 * @param timestep derivation value
 */
void Bdot_setTimestep(float timestep){
    Bdot.timestep = timestep;
}

#endif //ADCS_ONBOARD_SOFTWARE_BDOT_H