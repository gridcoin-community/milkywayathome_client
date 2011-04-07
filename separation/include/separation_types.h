/* Copyright 2010 Matthew Arsenault, Travis Desell, Boleslaw
Szymanski, Heidi Newberg, Carlos Varela, Malik Magdon-Ismail and
Rensselaer Polytechnic Institute.

This file is part of Milkway@Home.

Milkyway@Home is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Milkyway@Home is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Milkyway@Home.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SEPARATION_TYPES_H_
#define _SEPARATION_TYPES_H_

#include "separation_config.h"
#include "milkyway_math.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MSC_VER
  #define SEPARATION_ALIGN(x) __attribute__ ((packed, aligned(x)))
#else
  #define SEPARATION_ALIGN(x) __declspec(align(x))
#endif /* _MSC_VER */

typedef struct
{
    real l;
    real b;
} LB;

#define LB_L(x) ((x).l)
#define LB_B(x) ((x).b)




#ifndef __OPENCL_VERSION__
typedef struct
{
    real irv_reff_xr_rp3;
    real gPrime;
} RConsts;

  #define IRV_REFF_XR_RP3(r) ((r).irv_reff_xr_rp3)
  #define GPRIME(r) ((r).gPrime)
#else

typedef real2 RConsts;

  #define IRV_REFF_XR_RP3(r) ((r).x)
  #define GPRIME(r) ((r).y)
#endif

typedef struct
{
    real nu;
    real id;
} NuId;


typedef struct
{
    unsigned int number_stars;
    mwvector* stars;
} StarPoints;

#define EMPTY_STAR_POINTS { 0, NULL }

typedef struct SEPARATION_ALIGN(128)
{
    mwvector a;
    mwvector c;
    real sigma_sq2_inv;
    int large_sigma;          /* abs(stream_sigma) > SIGMA_LIMIT */
} StreamConstants;

#ifndef __OPENCL_VERSION__
typedef struct
{
    real r_point;
    real qw_r3_N;
} RPoints;

#define R_POINT(r) ((r).r_point)
#define QW_R3_N(r) ((r).qw_r3_N)

typedef struct
{
    real lCosBCos;
    real lSinBCos;
    real bSin;
    real _pad;
} LBTrig;

#define LCOS_BCOS(x) ((x).lCosBCos)
#define LSIN_BCOS(x) ((x).lSinBCos)
#define BSIN(x) ((x).bSin)

#else

/* x = r_point; y = qw_r3_N */
typedef real2 RPoints;

#define R_POINT(r) ((r).x)
#define QW_R3_N(r) ((r).y)


typedef real4 LBTrig;

#define LCOS_BCOS(l) ((l).x)
#define LSIN_BCOS(l) ((l).y)
#define BSIN(l) ((l).z)

#endif /* __OPENCL_VERSION__ */

/* Convenience structure for passing mess of LBTrig to CAL kernel in 2 parts */
typedef struct
{
    real lCosBCos, lSinBCos;
} LTrigPair;

typedef struct
{
    real nu;
    real id;
} NuConstants;

typedef struct
{
    real* dx;
    real* qgaus_W;
} StreamGauss;


/* Parameter related types */

typedef struct SEPARATION_ALIGN(128)
{
    real r_min, r_max, r_step_size;
    real nu_min, nu_max, nu_step_size;
    real mu_min, mu_max, mu_step_size;
    unsigned int r_steps, nu_steps, mu_steps;
} IntegralArea;

typedef struct
{
    real weight;
    real step;
    real min;
    real max;
    int optimize;
} StreamWeight;

#define EMPTY_STREAM_WEIGHT { 0.0, 0.0, 0.0, 0.0, 0 }

typedef struct
{
    real* stream_parameters;
    real* stream_step;
    real* stream_min;
    real* stream_max;
    int* stream_optimize;
} StreamParameters;

#define EMPTY_STREAM_PARAMETERS { NULL, NULL, NULL, NULL, NULL }

typedef struct
{
    StreamWeight* stream_weight;
    StreamParameters* parameters;

    unsigned int number_streams;
    unsigned int number_stream_parameters;
} Streams;

#define EMPTY_STREAMS { NULL, NULL, 0, 0 }

typedef struct
{
    real* parameters;
    real* step;
    real* min;
    real* max;
    int* optimize;
} BackgroundParameters;

#define EMPTY_BACKGROUND_PARAMETERS { NULL, NULL, NULL, NULL, NULL }

typedef struct
{
    real irv;
    real rPrime;
} RPrime;



typedef struct SEPARATION_ALIGN(128) _AstronomyParameters
{
    /* Constants determined by other parameters */
    real m_sun_r0;
    real q_inv_sqr;  /* 1 / q^2 */
    real r0;
    real alpha;

    real alpha_delta3;
    real bg_a, bg_b, bg_c;

    unsigned int convolve;
    unsigned int number_streams;

    int wedge;
    int aux_bg_profile;
    real sun_r0;
    real delta;
    real q;
    real sn;
    real coeff;

    real parameters_version;
    real total_calc_probs;  /* sum of (r_steps * mu_steps * nu_steps) for all integrals */
    int sgr_coordinates;
    unsigned int number_integrals;
    unsigned int number_background_parameters;

    real background_weight;
    int fast_h_prob;
} AstronomyParameters;

#define EMPTY_ASTRONOMY_PARAMETERS { 0.0, 0.0, \
                                     0.0, 0.0,   \
                                     0.0, 0.0, 0.0, 0.0, 0, 0, \
                                     0, 0, 0.0, 0.0,        \
                                     0, 0, 0.0, 0.0, 0, 0, 0, \
                                     0, 0.0, 0 }

typedef struct SEPARATION_ALIGN(16)
{
    real sum;
    real correction;
} Kahan;

#define ZERO_KAHAN { 0.0, 0.0 }

#define CLEAR_KAHAN(k) { (k).sum = 0.0; (k).correction = 0.0; }

typedef struct
{
    real likelihood;

    real backgroundIntegral;
    real backgroundLikelihood;

    real* streamIntegrals;
    real* streamLikelihoods;
} SeparationResults;

#ifdef __cplusplus
}
#endif

#endif /* _SEPARATION_TYPES_H_ */

