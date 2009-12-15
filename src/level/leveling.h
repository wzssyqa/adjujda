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


#ifndef _LEVELING_H_
#define _LEVELING_H_
#include <libxml/parser.h>
#include <libxml/tree.h>

int leveling(xmlNodePtr inproot,const char *outputfile);

void adjujda_level_error(int lev_err);

//enum leveling_read_xml_return{
#define loadfail 1;
#define emptyfile 2;
#define woalpha 3;
#define pointwithoutid 4;
//	nopoint=5,
#define noobservation 6;
#define wnetworktag 7;
#define wdescriptiontag 8;
#define rootempty 9;
#define wknpoint 10;
#define getknpointerr 11;
//	get0knpoint=12,
#define initlnwerr 13;
#define pointforerr 14;
#define wheightdiff 15;
#define obsforerr 16;
#define allocatpaerr 17;
#define allocatplerr 18;
#define inversionerr 19;
#define wpointobs 20;
#define allocadxerr 21;
#define allocaverr 22;
#define sucess 0;
//};



struct level_network{
	//
	double s0;

	//observations
	int obsn;
	int mobsn;
	double *L;
	int *StartP;
	int *EndP;
	double *P;
	double *dist;

	//points
	char **Pname;
	int mpn;
	int known;
	int pointnum;
	double *level;
	
	double *ATPA;
	double *ATPL;
	double *dX;
	double *V;

	double m_pvv;
	double m_mu;

	char *description;
	int issnoop;
	int isclosure;

	double alpha;
	double m_Sigma;
};


#endif
