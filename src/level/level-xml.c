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


#include "level-xml.h"
#include "leveling.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <math.h>
#include "../base/base.h"

int resize_lnwob(struct level_network *lnw,int obn){
	lnw->L=(double *)realloc(lnw->L,obn*sizeof(double));
	if(lnw->L==NULL) return -1;
	lnw->StartP=(int *)realloc(lnw->StartP,obn*sizeof(int));
	if(lnw->StartP==NULL) return -3;
	lnw->EndP=(int *)realloc(lnw->EndP,obn*sizeof(int));
	if(lnw->EndP==NULL) return -4;
	lnw->dist=(double *)realloc(lnw->dist,obn*sizeof(double));
	if(lnw->dist==NULL) return -5;
	lnw->mobsn=obn;
	return 0;
}

int resize_lnwp(struct level_network *lnw,int pn){
	lnw->level=(double *)realloc(lnw->level,pn*sizeof(double));
	if(lnw->level==NULL) return -5;
	lnw->Pname=(char **)realloc(lnw->Pname,pn*sizeof(char *));
	if(lnw->Pname==NULL) return -6;
	lnw->mpn=pn;
	return 0;
}

int get_one_point(struct level_network *lnw,int pos,xmlNodePtr node){
	if(lnw->mpn <= lnw->known){
		if(resize_lnwp(lnw,2*lnw->mpn)){
			return -3;
		}
	}
	
		
	lnw->Pname[pos]=(char *)xmlGetProp(node,"id");
	if(lnw->Pname[pos]==NULL && strlen(lnw->Pname[pos])==0) return -1;
	char *strlevel=(char *)xmlGetProp(node,"z");
	if(strlevel==NULL) return -2;
	lnw->level[pos]=strtod(strlevel,NULL);
	xmlFree(strlevel);
	

	lnw->known++;
	lnw->pointnum++;
	return 0;
}


int init_level_network(struct level_network *lnw){
	lnw->s0=0.0;
	
	lnw->ATPA=NULL;
	lnw->ATPL=NULL;
	lnw->dX=NULL;
	lnw->V=NULL;
	lnw->description=NULL;
	lnw->L=NULL;
	lnw->P=NULL;
	lnw->StartP=NULL;
	lnw->EndP=NULL;
	lnw->level=NULL;
	lnw->Pname=NULL;
	lnw->m_Sigma=0;
	lnw->alpha=0;
	lnw->issnoop=0;
	lnw->isclosure=0;
	
	int revalue=0;
	if(revalue=resize_lnwob(lnw,5)){
		return revalue;
	}
	lnw->obsn=0;

	if(revalue=resize_lnwp(lnw,5)){
		return revalue;
	}

	lnw->known=0;
	lnw->pointnum=0;
	return 0;
}



int append_unknown_point(struct level_network *lnw,char *name){
	if(lnw->mpn <= lnw->pointnum){
		if(resize_lnwp(lnw,2*lnw->mpn)){
			return -1;
		}
		lnw->mpn*=2;
	}
	lnw->level[lnw->pointnum]=1.1e6;
	lnw->Pname[lnw->pointnum]=(char *)malloc(sizeof(char)*(strlen(name)+1));
	strcpy(lnw->Pname[lnw->pointnum],name);
	lnw->pointnum++;
	return 0;
}

int get_one_observation(struct level_network *lnw,int pos,xmlNodePtr node){
	char *from=BAD_CAST xmlGetProp(node,"from");
	char *to=BAD_CAST xmlGetProp(node,"to");
	char *value=BAD_CAST xmlGetProp(node,"val");
	double dh=strtod(value,NULL);
	xmlFree(value);
	char *dist=(char *)xmlGetProp(node,"dist");
	double dt=strtod(dist,NULL);
	xmlFree(dist);

	if(lnw->mobsn <= pos){
		if(resize_lnwob(lnw,2*lnw->mobsn)){
			return -3;
		}
	}

	int kks=0;
	int kke=0;
	for(int i=0;i<lnw->pointnum;i++){
		if(kks==0){
			if(!strcmp(from,lnw->Pname[i])){
				lnw->StartP[pos]=i;
				kks=1;
			}
		}
		if(kke==0){
			if(!strcmp(to,lnw->Pname[i])){
				lnw->EndP[pos]=i;
				kke=1;
			}
		}
		if(kks && kke) break;
	}
	if(kks==0){
		if(append_unknown_point(lnw,from)){
			return -1;
		}
		lnw->StartP[pos]=lnw->pointnum-1;
	}
	if(kke==0){
		if(append_unknown_point(lnw,to)){
			return -2;
		}
		lnw->EndP[pos]=lnw->pointnum-1;
	}
	lnw->dist[pos]=dt;
	lnw->L[pos]=dh;
	lnw->obsn++;
	xmlFree(from);
	xmlFree(to);
	return 0;
}

int leveling_read_xml(xmlNodePtr root,struct level_network *lnw){

	xmlNodePtr cur=root;
	cur=cur->xmlChildrenNode;
	if(cur==NULL){
		return rootempty;
	}

	char *value;
	char *value1;
	
	//get s0
	if(!strcmp(cur->name,"network")){
		value=(char *)xmlGetProp(cur,"s0");
		lnw->s0=strtod(value,NULL);
		xmlFree(value);	

		value=(char *)xmlGetProp(cur,"snoop");
		if(value==NULL);
		else if(value[0]=='y'){
			lnw->issnoop=1;
			value1=(char *)xmlGetProp(cur,"alpha");
			if(value1) lnw->alpha=strtod(value1,NULL);
			else return woalpha;
			xmlFree(value1);
		}
		else {
			lnw->issnoop=0;
		}
		xmlFree(value);

		value=(char *)xmlGetProp(cur,"closure");
		if(value==NULL);
		else if(value[0]=='y'){
			lnw->isclosure=1;
		}
		else {
			lnw->isclosure=0;
		}
		if(lnw->issnoop || lnw->isclosure){
			value=(char *)xmlGetProp(cur,"sigma");
			if(value!=NULL){
				lnw->m_Sigma=strtod(value,NULL);
			}
			xmlFree(value);
		}
		xmlFree(value);
	}
	else{
		return wnetworktag;
	}

	//get description
	cur=cur->next;
	if(!strcmp(cur->name,"description")){
		lnw->description = (char *)xmlNodeGetContent(cur);
	}
	else{
		return wdescriptiontag;
	}

	cur=cur->next;
	if(!strcmp(cur->name,"points-observations")){
		cur=cur->xmlChildrenNode;
		if(cur==NULL){
			return wknpoint;
		}
	}
	else{
		return wpointobs;
	}

	//get known point
	xmlNodePtr coornode=cur;
	if(!strcmp(coornode->name,"coordinates")){
		coornode=coornode->xmlChildrenNode;
		if(cur==NULL){
			return wknpoint;
		}
		int fdpo=-1;
		do{
			fdpo++;
			if(!strcmp(coornode->name,"point")){
				if(get_one_point(lnw,fdpo,coornode)){
					return pointforerr;
				}
			}
			else{
				return wknpoint;
			}
			coornode=coornode->next;
		}while(coornode);
	}
	else{
		return wknpoint;
	}

	//get observations
	cur=cur->next;
	int goob=0;
	if(!strcmp(cur->name,"height-differences")){
		cur=cur->xmlChildrenNode;
		if(cur==NULL){
			return wheightdiff;
		}
		int fdobs=-1;
		do{
			fdobs++;
			if(!strcmp(cur->name,"dh")){
				if(goob=get_one_observation(lnw,fdobs,cur)){
					return obsforerr;
				}
			}
			cur=cur->next;
		}while(cur);
		resize_lnwob(lnw,lnw->obsn);
		resize_lnwp(lnw,lnw->pointnum);
	}
	else{
		return wheightdiff;
	}

	return 0;
}

int level_write_xml(struct level_network *lnw,const char *outputfile){

	xmlDocPtr opdoc=NULL;
	xmlNodePtr oproot=NULL,opdesc=NULL;
	opdoc=xmlNewDoc(BAD_CAST "1.0");
	oproot=xmlNewNode(NULL, BAD_CAST "gama-local");
	xmlDocSetRootElement(opdoc, oproot);

	xmlNodePtr opnw;
	char ops0[16];
	snprintf(ops0,16,"%lf",lnw->s0);
	opnw=xmlNewNode(NULL,BAD_CAST"network");
	xmlSetProp(opnw,BAD_CAST"s0",ops0);
	xmlAddChild(oproot,opnw);

	opdesc=xmlNewNode(NULL,BAD_CAST"description");
	xmlNodeSetContent(opdesc,lnw->description);
	xmlAddChild(oproot,opdesc);

	xmlNodePtr oppoob;
	oppoob=xmlNewNode(NULL,BAD_CAST"points-observations");
	xmlAddChild(oproot,oppoob);

	xmlNodePtr opcoor;
	opcoor=xmlNewNode(NULL,BAD_CAST"coordinates");
	xmlAddChild(oppoob,opcoor);

	xmlNodePtr oppoint;
	char opl[16];
	char oppstdev[32];
	for(int i=0;i<lnw->pointnum;i++){
		oppoint=xmlNewNode(NULL,BAD_CAST"point");
		xmlSetProp(oppoint,"id",lnw->Pname[i]);
		snprintf(opl,16,"%lf",lnw->level[i]);
		xmlSetProp(oppoint,"z",opl);
		snprintf(oppstdev,32,"%lf",sqrt(lnw->ATPA[ij(i,i)])*lnw->m_mu);
		xmlSetProp(oppoint,"stdev",oppstdev);
		xmlAddChild(opcoor,oppoint);
	}

	xmlNodePtr ophidiff;
	ophidiff=xmlNewNode(NULL,BAD_CAST"height-differences");

	xmlNodePtr opdh;
	char opval[16];
	char opweight[16];
	char opstdev[16];
	int startp;
	int endp;
	for(int i=0;i<=lnw->obsn-1;i++){
		int k1=lnw->StartP[i];
		int k2=lnw->EndP[i];
		double qii=lnw->ATPA[ij(k1,k1)];
		double qjj=lnw->ATPA[ij(k2,k2)];
		double qij=lnw->ATPA[ij(k1,k2)];
		double ml=sqrt(qii+qjj-2.0*qij)*lnw->m_mu;

		opdh=xmlNewNode(NULL,BAD_CAST"dh");
		startp=lnw->StartP[i];
		xmlSetProp(opdh,"from",lnw->Pname[startp]);
		endp=lnw->EndP[i];
		xmlSetProp(opdh,"to",lnw->Pname[endp]);

		snprintf(opval,16,"%lf",lnw->L[i]+lnw->V[i]);
		xmlSetProp(opdh,"val",BAD_CAST opval);

		snprintf(opweight,16,"%lf",lnw->P[i]);
		xmlSetProp(opdh,"weight",BAD_CAST opweight);

		snprintf(opstdev,16,"%lf",ml);
		xmlSetProp(opdh,"stdev",BAD_CAST opstdev);

		xmlAddChild(ophidiff,opdh);
		
	}

	xmlAddChild(oppoob,ophidiff);
	
	xmlSaveFormatFile(outputfile,opdoc,1);
	xmlFreeDoc(opdoc);
	xmlCleanupParser();
	xmlMemoryDump();
	return 0;
}

