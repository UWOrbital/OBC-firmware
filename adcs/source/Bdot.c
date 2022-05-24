#include "Bdot.h"
#include "matrixOperations.h"

void Bdot_init(float magneticFieldBody[3], float Kp[3][3], float maxMagneticDipole[3], float timestep){
    Bdot_setMagneticFieldBody(magneticFieldBody);
    float empty[3] = {0, 0, 0};
    Bdot_setBDotVector(empty);
    Bdot_setKp(Kp);
    Bdot_setMaxMagneticDipole(maxMagneticDipole);
    Bdot_setTimestep(timestep);
}

float *Bdot_controller(float magneticFieldBody[3]){

    Bdot.timestep = Bdot.timestep != 0 ? 0.1 : Bdot.timestep;

    float magneticFieldChange[3];
    vectorSubtraction(magneticFieldBody,Bdot.magneticFieldBody, magneticFieldChange);
    scalarVectorMultiply(magneticFieldChange, 1/Bdot.timestep, Bdot.bDotVector);

    float *magneticDipole =(float *)malloc (sizeof (float) * 3);

    float KpNegative[3][3];
    scalarMultiply(Bdot.Kp, -1, KpNegative);
    vectorMultiply(KpNegative, Bdot.bDotVector, magneticDipole);

    if (vectorNorm(Bdot.magneticFieldBody) != 0) {
        scalarVectorMultiply(magneticDipole, 1/vectorNorm(Bdot.magneticFieldBody),magneticDipole);
    }
    
    Bdot_magnetorquerScaling(magneticDipole);
    Bdot_setMagneticFieldBody(magneticFieldBody);

    return magneticDipole;
}

float *Bdot_magnetorquerScaling(float magneticDipole[3]){
    for (int i = 0; i < 3; i++) {
        if (abs(magneticDipole[i]) > Bdot.maxMagneticDipole[i]) {
            magneticDipole[i] = copysignf(Bdot.maxMagneticDipole[i], magneticDipole[i]);
        }
    }
    return magneticDipole;
}