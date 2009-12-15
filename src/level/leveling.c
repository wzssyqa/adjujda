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


#include "leveling.h"
#include "level-compute.h"
#include "level-xml.h"
#include <malloc.h>
/*#include <string.h>*/
/*#include <math.h>*/
#include <libxml/parser.h>
#include <libxml/tree.h>
/*#include "../base/base.h"*/

char *leveling_error_string[22]={
	"",//0
	"load input file error.\n",//1
	"input file is empty.\n",//2
	"ask for snoop,but without alpha.\n",//3
	"find one point without id prop.\n",//4
	"",//5
	"find zero observation.\n",//6
	"can not find network tag.\n",//7
	"input file without description tag.\n",//8
	"input file's root element has no subelements.\n",//9
	"find zero known point.\n"//10
	"",//11
	"",//12
	"when init lnw struct,error occured.\n",//13
	"find one <point /> tag has error format.\n",//14
	"can not find <dh /> tag.\n",//15
	"<dh /> tag format error.\n",//16
	"when alloca memory for ATPA,error occured.\n",//17
	"when alloca memory for ATPL,error occured.\n",//18
	"ATPA is not full rank.\n",//19
	"without points-observations tag.\n",//20
	"when alloca memory for dX,error occured.\n",//21
	"when alloca memory for V,error occured.\n"//22
};

void adjujda_level_error(int lev_err){
	if(lev_err>22){
		fprintf(stderr,"error code is too big.\n");
		exit(0);
	}
	fprintf(stderr,"%s",leveling_error_string[lev_err]);
}

int leveling(xmlNodePtr root,const char *outputfile){

	struct level_network lnws;
	struct level_network *lnw=&lnws;
	int initr=init_level_network(lnw);
	if(initr!=0) return initlnwerr;

	int revalue;
	if(revalue=leveling_read_xml(root,lnw)){
		return revalue;
	}

	lnw->P=(double *)malloc(sizeof(double)*lnw->obsn);
	lnw->ATPA=(double *)malloc(sizeof(double)*lnw->pointnum*(lnw->pointnum+1)/2);
	if(lnw->ATPA==NULL) return allocatpaerr;
	lnw->ATPL=(double *)malloc(sizeof(double)*lnw->pointnum);
	if(lnw->ATPL==NULL) return allocatplerr;

	lnw->dX=(double *)malloc(lnw->pointnum*sizeof(double));
	if(lnw->dX==NULL) return allocadxerr;
	lnw->V=(double *)malloc(lnw->obsn*sizeof(double));
	if(lnw->V==NULL) return allocaverr;

	compute_H0(lnw);
	compute_P(lnw);

	if(lnw->isclosure){
		printf("sorry,not implement yet!\n");
	}

	if(lnw->issnoop){
		if(revalue=leveling_data_snooping(lnw)){
			return revalue;
		}
	}
	else if(revalue=leveling_adj_compute(lnw)){
		return revalue;
	}

	if(revalue=level_write_xml(lnw,outputfile)){
		return revalue;
	}

	return 0;
}


