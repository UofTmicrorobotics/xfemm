#pragma once

#include <complex>

// Transitional complex type used throughout the project.
// Use complex_t in new code; complexd_t is kept as an alias for compatibility.
using complex_t = std::complex<double>;
using complexd_t = complex_t; // compatibility alias (existing code may still use complexd_t)

// Imaginary unit constant (previously provided as I)
static const complex_t I(0.0, 1.0);
