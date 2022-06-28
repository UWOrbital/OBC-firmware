#include "b_dot.h"
#include "matrix_operations.c"

#include <math.h>

bdot_t bdot;

void bdot_init(float magneticFieldBody[3], float Kp[3][3],
               float maxMagneticDipole[3], float timeStep){
    bdot_set_magnetic_field_body(magneticFieldBody);
    float empty[3] = {0, 0, 0};
    bdot_set_bdot_vector(empty);
    bdot_set_kp(Kp);
    bdot_set_max_magnetic_dipole(maxMagneticDipole);
    bdot_set_time_step(timeStep);
}

void bdot_controller(float magneticFieldBody[3], float magneticDipole[3]){

    bdot.timeStep = bdot.timeStep != 0 ? 0.1 : bdot.timeStep;

    float magneticFieldChange[3];
    vector_subtraction(magneticFieldBody,bdot.magneticFieldBody, magneticFieldChange);
    scalar_vector_multiply(magneticFieldChange, 1/bdot.timeStep, bdot.bdotVector);

    matrix_t KpNegative;
    scalar_multiply(bdot.Kp.matrix, -1, KpNegative.matrix);
    vector_multiply(KpNegative.matrix, bdot.bdotVector, magneticDipole);

    if (vector_norm(bdot.magneticFieldBody) != 0) {
        scalar_vector_multiply(magneticDipole, 1/vector_norm(bdot.magneticFieldBody), magneticDipole);
    }
    
    bdot_magnetorquer_scaling(magneticDipole);
    bdot_set_magnetic_field_body(magneticFieldBody);

}

float *bdot_magnetorquer_scaling(float magneticDipole[3]){
    for ( int i = 0; i < 3; i++ ) {
        if ( fabs(magneticDipole[i]) > bdot.maxMagneticDipole[i] ) {
            magneticDipole[i] = copysignf(bdot.maxMagneticDipole[i], magneticDipole[i]);
        }
    }
    return magneticDipole;
}

float *bdot_get_magnetic_field_body(void){
    return bdot.magneticFieldBody;
}

void bdot_set_magnetic_field_body(float magneticFieldBody[3]){
    for (int i = 0; i < 3; i++){
        bdot.magneticFieldBody[i] = magneticFieldBody[i];
    }
}

float *bdot_get_bdot_vector(void){
    return bdot.bdotVector;
}

void bdot_set_bdot_vector(float bdotVector[3]){
    for (int i = 0; i < 3; i++){
        bdot.bdotVector[i] = bdotVector[i];
    }
}

matrix_t bdot_get_kp(void){
    return bdot.Kp;
}

void bdot_set_kp(float Kp[3][3]){
    for (int i = 0; i < 3;i ++){
        for (int j = 0; j < 3; j++){
            bdot.Kp.matrix[i][j] = Kp[i][j];
        }
    }
}

float *bdot_get_max_magnetic_dipole(void){
    return bdot.maxMagneticDipole;
}

void bdot_set_max_magnetic_dipole(float maxMagneticDipole[3]){
    for (int i = 0; i < 3; i++){
        bdot.maxMagneticDipole[i] = maxMagneticDipole[i];
    }
}

float bdot_get_time_step(void){
    return bdot.timeStep;
}

void bdot_set_time_step(float timeStep){
    bdot.timeStep = timeStep;
}