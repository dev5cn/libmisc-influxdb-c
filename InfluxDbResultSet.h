/*
  Copyright 2019 www.dev5.cn, Inc. dev5@qq.com
 
  This file is part of X-MSG-IM.
 
  X-MSG-IM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  X-MSG-IM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU Affero General Public License
  along with X-MSG-IM.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef INFLUXDBRESULTSET_H_
#define INFLUXDBRESULTSET_H_

#include "InfluxDbRow.h"

typedef struct
{
	vector<string> column; 
	map<string, int> columnIndx; 
	vector<InfluxDbRow*> row; 
} influxdb_result_set;

class InfluxDbResultSet
{
public:
	string err; 
	influxdb_result_set* rst;
public:
	bool getBool(int row, int column);
	uint getInt(int row, int column);
	ullong getLong(int row, int column);
	string getString(int row, int column);
	bool getBool(int row, const string& column);
	uint getInt(int row, const string& column);
	ullong getLong(int row, const string& column);
	string getString(int row, const string& column);
public:
	bool success();
	bool empty();
	int rowSize(); 
	int columnSize(); 
	string getColumn(int indx); 
	InfluxDbRow* getRow(int indx); 
	vector<string>& getColumn(); 
	string toString();
	InfluxDbResultSet();
	virtual ~InfluxDbResultSet();
private:
	int columnIndx(const string& column);
};

#endif 
