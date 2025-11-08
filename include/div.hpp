#pragma once

// include dependencies
#include "math.h"

/// Returns `a / d` rounded down.
///
/// @param a Divident.
/// @param d Divisor.
///
/// @return Proper quotient.
int idiv(int a, int d);

/// Returns `a % b` rounded down.
///
/// @param a Divident.
/// @param d Divisor.
///
/// @return Proper remainder ("modulo").
int imod(int a, int d);

/// Calculates quotient and remainder rounded down.
///
/// @param a Divident.
/// @param d Divisor.
/// @param q Quotient.
/// @param r Remainder.
void idivmod(int a, int d, int& q, int& r);