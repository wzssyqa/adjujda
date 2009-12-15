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


#ifndef _LEVEL_XML_H_
#define _LEVEL_XML_H_
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "leveling.h"

int leveling_read_xml(xmlNodePtr root,struct level_network *lnw);

int level_write_xml(struct level_network *lnw,const char *outputfile);

int init_level_network(struct level_network *lnw);

#endif
