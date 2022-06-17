#include "b_dot.h"
#include "matrix_operations.h"

#include <math.h>
#include <stdlib.h>


void Bdot_init(float magneticFieldBody[3], float Kp[3][3],
               float maxMagneticDipole[3], float timeStep){
    Bdot_set_magnetic_field_body(magneticFieldBody);
    float empty[3] = {0, 0, 0};
    Bdot_set_Bdot_vector(empty);
    Bdot_set_kp(Kp);
    Bdot_set_max_magnetic_dipole(maxMagneticDipole);
    Bdot_set_time_step(timeStep);
}

void Bdot_controller(float magneticFieldBody[3], float magneticDipole){

    Bdot.timeStep = Bdot.timeStep != 0 ? 0.1 : Bdot.timeStep;

    float magneticFieldChange[3];
    vector_subtraction(magneticFieldBody,Bdot.magneticFieldBody, magneticFieldChange);
    scalar_vector_multiply(magneticFieldChange, 1/Bdot.timeStep, Bdot.bDotVector);

    float KpNegative[3][3];
    scalar_multiply(Bdot.Kp, -1, KpNegative);
    vector_multiply(KpNegative, Bdot.bDotVector, &magneticDipole);

    if (vector_norm(Bdot.magneticFieldBody) != 0) {
        scalar_vector_multiply(&magneticDipole, 1/vector_norm(Bdot.magneticFieldBody), &magneticDipole);
    }
    
    Bdot_magnetorquer_scaling(&magneticDipole);
    Bdot_set_magnetic_field_body(magneticFieldBody);

}

float *Bdot_magnetorquer_scaling(float magneticDipole[3]){
    for ( int i = 0; i < 3; i++ ) {
        if ( fabs(magneticDipole[i]) > Bdot.maxMagneticDipole[i] ) {
            magneticDipole[i] = copysignf(Bdot.maxMagneticDipole[i], magneticDipole[i]);
        }
    }
    return magneticDipole;
}