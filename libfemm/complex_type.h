#pragma once

#include <complex>

// Transitional complex type used throughout the project.
// Use complex_t in new code; CComplex is kept as an alias for compatibility.
using complex_t = std::complex<double>;
using CComplex = complex_t; // compatibility alias (existing code may still use CComplex)

// Imaginary unit constant (previously provided as I)
static const complex_t I(0.0, 1.0);
