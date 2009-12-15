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


#include "base.h"
#include <malloc.h>

double probability_norm(double u)
{
	if(u<-5.0) return 0.0;
	if(u>5.0) return 1.0;
	
	double y=fabs(u)/sqrt(2.0);

    double p=1.0+y*(_PROBABILITY_NORM_A1+y*(_PROBABILITY_NORM_A2+y*(_PROBABILITY_NORM_A3+
		y*(_PROBABILITY_NORM_A4+y*(_PROBABILITY_NORM_A5+y*_PROBABILITY_NORM_A6)))));

	double er =1- pow( p, -16.0 );
    p=(u<0.0)? 0.5-0.5*er: 0.5+0.5*er;
    return p;
}

double probability_re_norm(double p)
{
      if(p==0.5) return 0.0;
      if(p>0.9999997) return 5.0;
      if(p<0.0000003) return -5.0;
      if(p<0.5) return -probability_re_norm(1.0-p);

      double y=-log(4.0*p*(1.0-p));
      y=y*(_PROBABILITY_RENORM_B0+y*(_PROBABILITY_RENORM_B1
	      +y*(_PROBABILITY_RENORM_B2+y*(_PROBABILITY_RENORM_B3
	      +y*(_PROBABILITY_RENORM_B4+y*(_PROBABILITY_RENORM_B5
	      +y*(_PROBABILITY_RENORM_B6+y*(_PROBABILITY_RENORM_B7
	      +y*(_PROBABILITY_RENORM_B8+y*(_PROBABILITY_RENORM_B9
	      +y*_PROBABILITY_RENORM_B10))))))))));

      return sqrt(y);

}

int ij(int i,int j){
	return (i>=j)?(i*(i+1)/2+j):(j*(j+1)/2+i);
}

int podimat_inversion(double *a, int n){
	double *a0=malloc(sizeof(double)*n);
    	for(int k=0;k<n;k++){ 
		double a00=a[0];
		if(a00+1.0==1.0)
		{
			free(a0); 
			return -1;
		}
		for(int i=1;i<n;i++)
		{
			double ai0 = a[i*(i+1)/2];
			
			if(i<=n-k-1)a0[i]= -ai0/a00;
			else        a0[i]=  ai0/a00;

			for(int j=1;j<=i;j++)
			{
				a[(i-1)*i/2+j-1]=a[i*(i+1)/2+j]+ai0*a0[j];
			} 
		}
		for(int i=1;i<n;i++)
		{
			a[(n-1)*n/2+i-1]=a0[i];
		}
		a[n*(n+1)/2-1]=1.0/a00;
	} 
	free(a0);
	return 0;
	
}
