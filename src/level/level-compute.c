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


#include "level-compute.h"
#include "leveling.h"
#include "../base/base.h"


void compute_P(struct level_network *lnw){
	for(int i=0;i<lnw->obsn;i++){
		lnw->P[i]=lnw->s0/(lnw->dist[i]);
	}
}

void compute_H0(struct level_network *lnw){
	int pcompd=0;
	int obscompd=0;

	while(1){
		obscompd=0;
		for(int i=0;i<lnw->obsn;i++){
			double levofsp=lnw->level[lnw->StartP[i]];
			double levofep=lnw->level[lnw->EndP[i]];
			if(levofsp <1.0e6 && levofep <1.0e6){
				obscompd++;
			}
			else if(levofsp >1.0e6 && levofep <1.0e6){
				lnw->level[lnw->StartP[i]]=levofep-lnw->L[i];
			}
			else if(levofsp <1.0e6 && levofep >1.0e6){
				lnw->level[lnw->EndP[i]]=levofsp+lnw->L[i];
			}
		}
		if(obscompd==lnw->obsn){
				break;
		}
	}
	for(int i=0;i<lnw->pointnum;i++){
		if(lnw->level[i]>1.0e6){
			exit(0);
		}
	}
}

void compute_ATPA(struct level_network *lnw){
	for(int i=0;i<lnw->pointnum;i++){
		lnw->ATPL[i]=0.0;
	}
	for(int i=0;i<lnw->pointnum*(lnw->pointnum+1)/2;i++){
		lnw->ATPA[i]=0.0;
	}

	for(int i=0;i<lnw->obsn;i++){
		int sp=lnw->StartP[i];
		int ep=lnw->EndP[i];

		double Pi=lnw->P[i];
		double Li=lnw->L[i]-(lnw->level[ep]-lnw->level[sp]);

		lnw->ATPL[sp]-=Pi*Li;
		lnw->ATPL[ep]+=Pi*Li;

		lnw->ATPA[ij(sp,sp)]+=Pi;
		lnw->ATPA[ij(ep,ep)]+=Pi;
		lnw->ATPA[ij(sp,ep)]-=Pi;
		
	}
}



int leveling_adj_compute(struct level_network *lnw){

	compute_ATPA(lnw);
	for(int i=0;i< lnw->known;i++){
		lnw->ATPA[ij(i,i)]=1.0e29;
	}

	if(podimat_inversion(lnw->ATPA,lnw->pointnum)){
		return inversionerr;
	}

	for(int i=0;i<lnw->pointnum;i++){
		double xi=0;
		for(int j=0;j<lnw->pointnum;j++){
			xi+=lnw->ATPA[ij(i,j)] * lnw->ATPL[j];
		}
		lnw->dX[i]=xi;
		lnw->level[i]+=xi;
	}

	///////////////////////////////////////////////
	double pvv=0.0;
	for(int i=0;i<lnw->obsn;i++){
		int ks=lnw->StartP[i];
		int ke=lnw->EndP[i];
		lnw->V[i]=lnw->level[ke]-lnw->level[ks]-lnw->L[i];
		pvv+=lnw->V[i] * lnw->V[i] * lnw->P[i];
	}

	lnw->m_mu=sqrt(pvv/(lnw->obsn-(lnw->pointnum-lnw->known)));

	return 0;
}


int leveling_data_snooping(struct level_network *lnw){
	double upu=probability_re_norm(1.0-lnw->alpha/2);
	double pvv=0.0;
	int k=0;
	for(k=0;;k++){
		compute_ATPA(lnw);
		for(int i=0;i<lnw->known;i++){
			lnw->ATPA[ij(i,i)]=1.0e30;
		}

		if(podimat_inversion(lnw->ATPA,lnw->pointnum)){
		return inversionerr;
		}

		for(int i=0;i<lnw->pointnum;i++){
			double xi=0;
			for(int j=0;j<lnw->pointnum;j++){
			xi+=lnw->ATPA[ij(i,j)] * lnw->ATPL[j];
			}
			lnw->dX[i]=xi;
			lnw->level[i]+=xi;
		}

		for(int i=0;i<lnw->obsn;i++){
			int ks=lnw->StartP[i];
			int ke=lnw->EndP[i];
			lnw->V[i]=lnw->level[ke]-lnw->level[ks]-lnw->L[i];
			pvv+=lnw->V[i] * lnw->V[i] * lnw->P[i];
		}

		double max_v=0.0;
		int max_i;
		for(int i=0;i<lnw->obsn;i++){
			if(lnw->P[i]<1.0e-10) continue;
			int k1=lnw->StartP[i];
			int k2=lnw->EndP[i];
			double qii=lnw->ATPA[ij(k1,k1)];
			double qjj=lnw->ATPA[ij(k2,k2)];
			double qij=lnw->ATPA[ij(k1,k2)];

			double qv=1.0/(lnw->P[i])-(qii+qjj-2*qij);
			double mv=sqrt(qv)*lnw->m_Sigma;

			double vi=lnw->V[i]/mv;

			if(fabs(vi)>max_v){
				max_v=fabs(vi);
				max_i=i;
			}
		}

		if(max_v>upu) lnw->P[max_i]=0.0;
		else break;
	}

	lnw->m_mu=sqrt(pvv/(lnw->obsn-k-(lnw->pointnum-lnw->known)));
	return 0;
}

