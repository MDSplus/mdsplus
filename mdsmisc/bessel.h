/*  DEC/CMS REPLACEMENT HISTORY, Element BESSEL.H */
/*  *21   13-MAR-1995 17:31:51 MDSPLUS "New Version" */
/*  *20   14-DEC-1994 17:51:02 MDSPLUS "Proceede" */
/*  *19   14-DEC-1994 17:48:51 MDSPLUS "Proceede" */
/*  *18   16-NOV-1994 12:44:07 MDSPLUS "OSF/1 compatibile" */
/*  *17   15-NOV-1994 16:31:58 MDSPLUS "Ported to OSF1" */
/*  *16    8-NOV-1994 17:30:57 MDSPLUS "Proceede" */
/*  *15    7-NOV-1994 13:47:51 MDSPLUS "Proceede" */
/*  *14    7-NOV-1994 13:46:22 MDSPLUS "Proceede" */
/*  *13    7-NOV-1994 13:45:14 MDSPLUS "Proceede" */
/*  *12    7-NOV-1994 13:43:12 MDSPLUS "Proceede" */
/*  *11    7-NOV-1994 13:41:48 MDSPLUS "Proceede" */
/*  *10    7-NOV-1994 12:53:28 MDSPLUS "Proceede" */
/*  *9     7-NOV-1994 12:48:37 MDSPLUS "Proceede" */
/*  *8     7-NOV-1994 11:45:55 MDSPLUS "Proceede" */
/*  *7     7-NOV-1994 11:45:04 MDSPLUS "Proceede" */
/*  *6     7-NOV-1994 11:42:03 MDSPLUS "Proceede" */
/*  *5     7-NOV-1994 11:36:39 MDSPLUS "Proceede" */
/*  *4     5-OCT-1994 11:15:30 MDSPLUS "Fix" */
/*  *3     5-OCT-1994 11:03:18 MDSPLUS "Fix" */
/*  *2    22-SEP-1994 18:26:56 MDSPLUS "" */
/*  *1    22-SEP-1994 18:26:03 MDSPLUS "Include file for Bessel management
 * (Roots)" */
/*  DEC/CMS REPLACEMENT HISTORY, Element BESSEL.H */
#ifndef COMPLEX_H
#include "complex.h"
#endif

#define MAX_BESSEL_EXPANSION 40
#define MAX_BESSEL_IDX 2
#define NUM_ROOTS 40

/*
static double bessel_root[MAX_BESSEL_IDX + 1][NUM_ROOTS] = {
{2.40482556, 5.52007811, 8.65372792, 11.79153444, 14.93091771,
18.07106397, 21.21163663, 24.35247153, 27.49347913, 30.63460647,
33.77582022, 36.91709835, 40.05842576, 43.19979171, 46.34118837, 49.48260990,
52.62405184, 55.76551075, 58.90698393, 62.04846919, 65.18996480,
68.33146933, 71.47298160, 74.61450064, 77.75602563, 80.89755587,
84.03909078, 87.18062985, 90.32217264, 93.46371878, 96.60526795, 99.74681986,
102.88837425, 106.02993092, 109.17148965, 112.31305028, 115.45461265,
118.59617663, 121.73774209, 124.87930891 },

{0.00000000, 3.83170597, 7.01558668, 10.17346813, 13.32369194,
16.47063005, 19.61585851, 22.76008438, 25.90367209, 29.04682854,
32.18967991, 35.33230755, 38.47476623, 41.61709422, 44.75931900,
47.90146089, 51.04353518, 54.18555364, 57.32752544, 60.46945784,
63.61135670, 66.75322674, 69.89507184, 73.03689522, 76.17869959,
79.32048717, 82.46225992, 85.60401944, 88.74576714, 91.88750425,
95.02923181, 98.17095073, 101.31266182, 104.45436579, 107.59606326,
110.73775478, 113.87944085, 117.02112190, 120.16279833, 123.30447049},

{0.00000000, 5.15640219, 8.41724414, 11.61984117, 14.79595178,
17.95981949, 21.11699705, 24.27011231, 27.42057355, 30.56920449,
33.71651951, 36.86285651, 40.00844673, 43.15345378, 46.29799668, 49.44216411,
52.58602351, 55.72962705, 58.87301577, 62.01622236, 65.15927319,
68.30218978, 71.44498987, 74.58768817, 77.73029706, 80.87282695,
84.01528671, 87.15768394, 90.30002516, 93.44231602, 96.58456145, 99.72676574,
102.86893265, 106.01106552, 109.15316729, 112.29524056, 115.43728766,
118.57931068, 121.72131148, 124.86329174}};
*/

static double bessel_root[MAX_BESSEL_IDX + 1][NUM_ROOTS - 1] = {
    {2.40482556,   5.52007811,   8.65372792,   11.79153444,  14.93091771,
     18.07106397,  21.21163663,  24.35247153,  27.49347913,  30.63460647,
     33.77582022,  36.91709835,  40.05842576,  43.19979171,  46.34118837,
     49.48260990,  52.62405184,  55.76551075,  58.90698393,  62.04846919,
     65.18996480,  68.33146933,  71.47298160,  74.61450064,  77.75602563,
     80.89755587,  84.03909078,  87.18062985,  90.32217264,  93.46371878,
     96.60526795,  99.74681986,  102.88837425, 106.02993092, 109.17148965,
     112.31305028, 115.45461265, 118.59617663, 121.7377420},

    {3.83170597,   7.01558668,   10.17346813,  13.32369194,  16.47063005,
     19.61585851,  22.76008438,  25.90367209,  29.04682854,  32.18967991,
     35.33230755,  38.47476623,  41.61709422,  44.75931900,  47.90146089,
     51.04353518,  54.18555364,  57.32752544,  60.46945784,  63.61135670,
     66.75322674,  69.89507184,  73.03689522,  76.17869959,  79.32048717,
     82.46225992,  85.60401944,  88.74576714,  91.88750425,  95.02923181,
     98.17095073,  101.31266182, 104.45436579, 107.59606326, 110.73775478,
     113.87944085, 117.02112190, 120.16279833, 123.30447049},

    {5.15640219,   8.41724414,   11.61984117,  14.79595178,  17.95981949,
     21.11699705,  24.27011231,  27.42057355,  30.56920449,  33.71651951,
     36.86285651,  40.00844673,  43.15345378,  46.29799668,  49.44216411,
     52.58602351,  55.72962705,  58.87301577,  62.01622236,  65.15927319,
     68.30218978,  71.44498987,  74.58768817,  77.73029706,  80.87282695,
     84.01528671,  87.15768394,  90.30002516,  93.44231602,  96.58456145,
     99.72676574,  102.86893265, 106.01106552, 109.15316729, 112.29524056,
     115.43728766, 118.57931068, 121.72131148, 124.86329174}};

static double BesselFactors[10][10] = {
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {3, 3, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 2.322, 3.678, 6.46, 0, 0, 0, 0, 0, 0},
    {5.792, 9.140, 4.208, 11.488, 0, 0, 0, 0, 0, 0},
    {0, 3.6467, 3.3520, 1.7427, 2.3247, 3.571, 0, 0, 0, 0},
    {4.2484, 0.8675, 3.7356, 2.6263, 2.5159, 4.4927, 0, 0, 0, 0},
    {0, 4.9718, 4.7583, 1.7393, 4.0701, 3.5172, 2.6857, 5.4207, 0, 0},
    {5.5879, 0.8676, 2.8390, 6.3539, 4.3683, 4.1444, 5.2048, 2.6162, 0, 0},
    {0, 6.297, 6.1294, 1.7378, 5.6044, 3.4982, 4.6384, 5.3173, 2.9793, 7.2915},
    {6.922, 0.8677, 3.1089, 8.2327, 6.6153, 2.6116, 5.9675, 4.358, 4.8862,
     6.225}};

static double MagnitudeAtt[7][8] = {
    {0.25, 0.95, 2.1, 4, 6.5, 100, 100, 100},
    {0.24, 0.6, 1.5, 2.8, 4.5, 8, 100, 100},
    {0.23, 0.5, 1.15, 2, 3.2, 4.8, 8, 100},
    {0.22, 0.45, 0.9, 1.6, 2.6, 3.8, 5.4, 9},
    {0.21, 0.4, 0.75, 1.4, 2.1, 3.1, 4.35, 5.8},
    {0.20, 0.35, 0.6, 1.15, 1.85, 2.75, 3.75, 5},
    {0.19, 0.25, 0.55, 1, 1.6, 2.35, 3.2, 4.2}};

#define NUM_STEP_MAG_ATT 8

static double DelayErr[7][14] = {
    {0.1, 0.5, 3.5, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
    {0, 0.1, 0.25, 1.1, 10, 100, 100, 100, 100, 100, 100, 100, 100, 100},
    {0, 0, 0, 0.25, 0.5, 2.2, 10, 100, 100, 100, 100, 100, 100, 100},
    {0, 0, 0, 0, 0.1, 0.4, 1, 3.5, 100, 100, 100, 100, 100, 100},
    {0, 0, 0, 0, 0, 0, 0.2, 0.5, 1.5, 3.8, 100, 100, 100, 100},
    {0, 0, 0, 0, 0, 0, 0, 0.1, 0.25, 0.6, 1.8, 5, 100, 100},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.05, 0.25, 0.9, 2, 7}};

#define NUM_STEP_DEL_ERR 14

Complex *BessCRoots(int n, double *g);
void BessRoots();
