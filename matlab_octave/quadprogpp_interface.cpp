//DD// Matlab interface for QLD using only bounds 2010/05/07

#include <iostream>
#include <fstream>
#include <math.h>
#include <limits>

#include "QuadProg++.hh"
#include "mex.h"

using namespace std;

enum qpStatus
{
    QP_OK = 0,
    QP_INFEASIBLE = 1,
};



void mexFunction( int num_output, mxArray *output[], int num_input, const mxArray *input[] )
{
    const mxArray *H = input[0];
    const mxArray *g = input[1];
    const mxArray *A = input[2];
    const mxArray *b = input[3];
    const mxArray *Ain = input[4];
    const mxArray *bin = input[5];


    mxArray *x = NULL;


    int num_var = mxGetM(H);
    int num_eq = mxGetM(A);
    int num_ineq = mxGetM(Ain);
    x = mxCreateDoubleMatrix(num_var, 1, mxREAL);

    Matrix<double> eH   ((double*) mxGetPr(H),   num_var, num_var);
    Vector<double> eg   ((double*) mxGetPr(g),   num_var);
    Matrix<double> eA   ((double*) mxGetPr(A),   num_var, num_eq);
    Vector<double> eb   ((double*) mxGetPr(b),   num_eq);
    Matrix<double> eAin ((double*) mxGetPr(Ain), num_var, num_ineq);
    Vector<double> ebin ((double*) mxGetPr(bin), num_ineq);
    Vector<double> ex   ((double*) mxGetPr(x),   num_var);

// solve the problem
    qpStatus qp_status;

    double return_value = solve_quadprog(eH, eg, eA, eb, eAin, ebin, ex);

    if (return_value == numeric_limits<double>::infinity())
    {
        qp_status = QP_INFEASIBLE;
    }
    else
    {
        qp_status = QP_OK;
        for(std::size_t i = 0; i < num_var; ++i)
        {
            ((double*) mxGetPr(x))[i] = ex[i];
        }
    }


// process results
    // solution
    output[0] = mxDuplicateArray(x);


    // info
    int num_info_fields = 2;
    const char *info_field_names[] = {
        "status",
        "obj"
    };

    output[1] = mxCreateStructMatrix(1, 1, num_info_fields, info_field_names);

    mxArray *info_status = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    ((INT32_T *) mxGetData (info_status))[0] = static_cast <int> (qp_status);
    mxSetField (output[1], 0, "status", info_status);

    mxArray *info_obj = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
    ((double *) mxGetData (info_obj))[0] = qp_status;
    mxSetField (output[1], 0, "obj", info_obj);

    return;
}