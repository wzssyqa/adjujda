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



#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include "level/leveling.h"

int main(int argc,char **argv){
	int leveling_error;
	if(argc!=3){
		printf("usage:adjujda inputfile outputfile\n");
		return -1;
	}
	
	xmlDocPtr doc=NULL;
	xmlNodePtr  cur=NULL;
	char* name=NULL;
	char* value=NULL;

	xmlKeepBlanksDefault(0); 

	doc=xmlParseFile(argv[1]);
	if(doc==NULL){
		fprintf(stderr,"Loading xml file failed.\n");
		return 0; 
	}

	cur=xmlDocGetRootElement(doc);
	if(cur==NULL){
	 	fprintf(stdout,"empty file.\n");
	 	return 0;
	}

	name=BAD_CAST cur->name;
	if(strcmp(name,"gama-local")){
		fprintf(stdout,"file not support\n");
		return 0;
	}
	
	char *nwtype=(char *)xmlGetProp(cur,"network");
	if(!strcmp(nwtype,"leveling")){
		leveling_error=leveling(cur,argv[2]);
		if(leveling_error){
			fprintf(stderr,"some error occured in leveling adjustment\n");
			adjujda_level_error(leveling_error);
		}
	}

	xmlFree(nwtype);
	xmlFreeDoc(doc);
}
