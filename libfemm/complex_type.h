#pragma once

#include <complex>

// Transitional complex type used throughout the project.
using complexd_t = std::complex<double>;
using complexf_t = std::complex<float>;


// Imaginary unit constant (previously provided as I)
static const complexd_t I(0.0, 1.0);


