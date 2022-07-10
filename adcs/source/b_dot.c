#include "b_dot.h"
#include "matrix_operations.h"

#include <math.h>

static bdot_t bdot;

uint8_t bdot_init(float magneticFieldBody[3], float kp[3][3],
               float maxMagneticDipole[3], float timeStep){
    uint8_t setMagneticField = bdot_set_magnetic_field_body(magneticFieldBody);
    float empty[3] = {0, 0, 0};
    uint8_t setBdotVector = bdot_set_bdot_vector(empty);
    uint8_t setKp = bdot_set_kp(kp);
    uint8_t setMaxMagenticDipole = bdot_set_max_magnetic_dipole(maxMagneticDipole);
    uint8_t setTimeStep = bdot_set_time_step(timeStep);

    if ( !setMagneticField || !setBdotVector || !setKp ||
        !setMaxMagenticDipole || !setTimeStep ) {
        return 0;
    }

    return 1;
}

void bdot_controller(float magneticFieldBody[3], float magneticDipole[3]){

    bdot.timeStep = bdot.timeStep != 0 ? 0.1 : bdot.timeStep;

    // Find the B_dot vecotr (derivative of the magnetic field with respect to time)
    float magneticFieldChange[3];
    vector_subtraction(magneticFieldBody,bdot.magneticFieldBody, magneticFieldChange, 3);
    scalar_vector_multiply(magneticFieldChange, 1/bdot.timeStep, bdot.bdotVector, 3);

    // Calculate the necessary magnetic dipole moment which is -Kp * dB/dt (B_dot vector)
    float kpNegative[3][3];
    scalar_matrix_3x3_multiply(bdot.kp, -1, kpNegative);
    vector_matrix_3x3_multiply(kpNegative, bdot.bdotVector, magneticDipole);

    if ( vector_norm(bdot.magneticFieldBody, 3) != 0 ) {
        scalar_vector_multiply(magneticDipole, 1/vector_norm(bdot.magneticFieldBody, 3), magneticDipole, 3);
    }

    // Scale the magnetic dipole moment so that it is never greater than the maximum possible magnetic dipole
    bdot_magnetorquer_scaling(magneticDipole);

    // Update the magnetic_field_body value
    bdot_set_magnetic_field_body(magneticFieldBody);

}

static void bdot_magnetorquer_scaling(float magneticDipole[3]){
    for ( int i = 0; i < 3; i++ ) {
        if ( fabs(magneticDipole[i]) > bdot.maxMagneticDipole[i] ) {
            magneticDipole[i] = copysignf(bdot.maxMagneticDipole[i], magneticDipole[i]);
        }
    }
}

float *bdot_get_magnetic_field_body(void){
    return bdot.magneticFieldBody;
}

uint8_t bdot_set_magnetic_field_body(float magneticFieldBody[3]){
    for ( int i = 0; i < 3; i++ ) {
        bdot.magneticFieldBody[i] = magneticFieldBody[i];
    }
    
    return 1;
}

float *bdot_get_bdot_vector(void){
    return bdot.bdotVector;
}

uint8_t bdot_set_bdot_vector(float bdotVector[3]){
    for ( int i = 0; i < 3; i++ ) {
        bdot.bdotVector[i] = bdotVector[i];
    }

    return 1;
}

void bdot_get_kp(float output[3][3]){
    for ( int i = 0; i < 3; i ++ ) {
        for ( int j = 0; j < 3; j++ ) {
            output[i][j] = bdot.kp[i][j];
        }
    }
}

uint8_t bdot_set_kp(float kp[3][3]){
    for ( int i = 0; i < 3; i ++ ) {
        for ( int j = 0; j < 3; j++ ) {
            bdot.kp[i][j] = kp[i][j];
        }
    }

    return 1;
}

float *bdot_get_max_magnetic_dipole(void){
    return bdot.maxMagneticDipole;
}

uint8_t bdot_set_max_magnetic_dipole(float maxMagneticDipole[3]){
    for ( int i = 0; i < 3; i++ ) {
        if ( maxMagneticDipole[i] < 0 ) {
            return 0;
        }
        bdot.maxMagneticDipole[i] = maxMagneticDipole[i];
    }

    return 1;
}

float bdot_get_time_step(void){
    return bdot.timeStep;
}

uint8_t bdot_set_time_step(float timeStep){
    bdot.timeStep = timeStep;

    return 1;
}