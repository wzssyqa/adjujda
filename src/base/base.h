//an adjustment program under GPLv3
//    Copyright (C) 2009  YunQiang Su <wzssyqa@gmail.com>
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.


#ifndef _BASE_H_
#define _BASE_H_
#include <math.h>

#define _PROBABILITY_NORM_A1 0.0705230784
#define _PROBABILITY_NORM_A2 0.0422820123
#define _PROBABILITY_NORM_A3 0.0092705272
#define _PROBABILITY_NORM_A4 0.0001520143
#define _PROBABILITY_NORM_A5 0.0002765672
#define _PROBABILITY_NORM_A6 0.0000430638

#define _PROBABILITY_RENORM_B0 1.570796288
#define _PROBABILITY_RENORM_B1 0.3706987906e-1
#define _PROBABILITY_RENORM_B2 -0.8364353589e-3
#define _PROBABILITY_RENORM_B3 -0.2250947176e-3
#define _PROBABILITY_RENORM_B4 0.6841218299e-5
#define _PROBABILITY_RENORM_B5 0.5824238515e-5
#define _PROBABILITY_RENORM_B6 -0.1045274970e-5
#define _PROBABILITY_RENORM_B7 0.8360937017e-7
#define _PROBABILITY_RENORM_B8 -0.3231081277e-8
#define _PROBABILITY_RENORM_B9 0.3657763036e-10
#define _PROBABILITY_RENORM_B10 0.6936233982e-12

double probability_re_norm(double p);

double probability_norm(double u);

int ij(int i,int j);

int podimat_inversion(double *a,int n);
#endif
