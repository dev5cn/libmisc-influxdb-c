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

#ifndef INFLUXDB_H_
#define INFLUXDB_H_

#include "InfluxDbResultSet.h"
#include "InfluxDbInsertReq.h"

class InfluxDb
{
public:
	shared_ptr<InfluxDbResultSet> insert(shared_ptr<InfluxDbInsertReq> req); 
	shared_ptr<InfluxDbResultSet> queryGet(const string& influxQl); 
	shared_ptr<InfluxDbResultSet> queryPost(const string& influxQl); 
	shared_ptr<InfluxDbResultSet> write(const string& dat); 
public:
	InfluxDb(const string& host, const string& usr, const string& pwd, const string& db);
	virtual ~InfluxDb();
private:
	string host; 
	string usr; 
	string pwd; 
	string db; 
	bool https; 
	string auth; 
private:
	shared_ptr<InfluxDbResultSet> httpGet(map<string, string> header, const string& query); 
	shared_ptr<InfluxDbResultSet> httpPost(map<string, string> header, const string& endPoint, const string& query, const string& dat); 
	string urlEncode(const string& url); 
};

#endif 
