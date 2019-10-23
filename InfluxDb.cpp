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

#include <curl/curl.h>
#include "InfluxDb.h"
#include "json/json.h"

size_t curl_rsp_header_cb(const char* ptr, size_t size, size_t nmemb, string* header)
{
	if (0 == size || NULL == ptr)
		return -1;
	header->append(ptr, nmemb * size);
	return nmemb * size;
}

size_t curl_rsp_cb(const char *ptr, size_t size, size_t nmemb, string *rsp)
{
	if (0 == size || NULL == ptr)
		return -1;
	rsp->append(ptr, nmemb * size);
	return nmemb * size;
}

InfluxDb::InfluxDb(const string& host, const string& usr, const string& pwd, const string& db)
{
	this->host = host;
	this->usr = usr;
	this->pwd = pwd;
	this->db = db;
	this->https = Misc::startWith(host, "https");
	this->auth = "Basic " + Crypto::base64enc(usr + ":" + pwd);
}

shared_ptr<InfluxDbResultSet> InfluxDb::insert(shared_ptr<InfluxDbInsertReq> req)
{
	if (req->row.empty())
	{
		shared_ptr<InfluxDbResultSet> rst(new InfluxDbResultSet());
		rst->err = "insert what? row is empty";
		return rst;
	}
	if (req->tag.empty())
	{
		shared_ptr<InfluxDbResultSet> rst(new InfluxDbResultSet());
		rst->err = "must be have tag in measurement";
		return rst;
	}
	string dat;
	for (auto& it : req->row)
	{
		if (req->tag.size() + req->field.size() + 1 != it->column.size()) 
		{
			shared_ptr<InfluxDbResultSet> rst(new InfluxDbResultSet());
			rst->err = "tag, field, ts not match with row data";
			return rst;
		}
		dat += (req->measurement + ",");
		for (size_t i = 0; i < req->tag.size(); ++i)
		{
			SPRINTF_STRING(&dat, "%s=%s%s", req->tag[i].c_str(), it->column[i].c_str(), (i + 1 == req->tag.size() ? "" : ","))
		}
		SPRINTF_STRING(&dat, " ")
		for (size_t i = 0; i < req->field.size(); ++i)
		{
			SPRINTF_STRING(&dat, "%s=\"%s\"%s", req->field[i].c_str(), it->column[i + req->tag.size()].c_str(), (i + 1 == req->field.size() ? "" : ","))
		}
		if (!req->field.empty())
		{
			SPRINTF_STRING(&dat, " ")
		}
		SPRINTF_STRING(&dat, "%s\n", it->column[it->column.size() - 1].c_str())
	}
	return this->write(dat);
}

shared_ptr<InfluxDbResultSet> InfluxDb::queryGet(const string& influxQl)
{
	map<string, string> header;
	header["Authorization"] = this->auth;
	string query = "db=" + this->db + "&epoch=ns&q=" + this->urlEncode(influxQl);
	return this->httpGet(header, query);
}

shared_ptr<InfluxDbResultSet> InfluxDb::queryPost(const string& influxQl)
{
	map<string, string> header;
	header["Authorization"] = this->auth;
	header["Content-Type"] = "application/x-www-form-urlencoded";
	string query = "db=" + this->db;
	return this->httpPost(header, "query", query, "q=" + this->urlEncode(influxQl));
}

shared_ptr<InfluxDbResultSet> InfluxDb::write(const string& dat)
{
	map<string, string> header;
	header["Authorization"] = this->auth;
	header["Content-Type"] = "application/x-www-form-urlencoded";
	string query = "db=" + this->db + "&precision=ns";
	return this->httpPost(header, "write", query, dat);
}

shared_ptr<InfluxDbResultSet> InfluxDb::httpGet(map<string, string> extra, const string& query)
{
	CURL *curl = ::curl_easy_init();
	struct curl_slist* header = NULL;
	for (auto& it : extra)
	{
		string str;
		SPRINTF_STRING(&str, "%s: %s", it.first.c_str(), it.second.c_str())
		header = curl_slist_append(header, str.c_str());
	}
	if (this->https)
	{
		::curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		::curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	::curl_easy_setopt(curl, CURLOPT_URL, (this->host + "query?" + query).c_str());
	::curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
	::curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);
	string rspHeader;
	string rspBody;
	::curl_easy_setopt(curl, CURLOPT_WRITEHEADER, (void* ) &rspHeader);
	::curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_rsp_header_cb);
	::curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void* ) &rspBody);
	::curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_rsp_cb);
	CURLcode res = ::curl_easy_perform(curl);
	shared_ptr<InfluxDbResultSet> rst(new InfluxDbResultSet());
	if (res != CURLE_OK)
	{
		rst->err.assign(::curl_easy_strerror(res));
		::curl_slist_free_all(header);
		::curl_easy_cleanup(curl);
		return rst;
	}
	if (rspBody.empty())
		return rst;
	::curl_slist_free_all(header);
	::curl_easy_cleanup(curl);
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(rspBody, root) || !root.isObject())
	{
		rst->err = "parse response json failed";
		return rst;
	}
	if (!root["error"].isNull())
	{
		rst->err = root["error"].asString();
		return rst;
	}
	Json::Value results = root["results"];
	if (results.isNull() || !results.isArray() || results.empty())
	{
		rst->err = "results exception";
		return rst;
	}
	Json::Value series = results[(uint) 0]["series"];
	if (series.isNull() || !series.isArray() || series.empty())
		return rst; 
	Json::Value name = series[(uint) 0]["name"];
	if (name.isNull() || !name.isString())
	{
		rst->err = "no measurement name";
		return rst;
	}
	Json::Value columns = series[(uint) 0]["columns"];
	if (columns.isNull() || !columns.isArray() || columns.empty())
	{
		rst->err = "columns exception";
		return rst;
	}
	rst->rst = new influxdb_result_set();
	for (uint i = 0; i < columns.size(); ++i)
	{
		if (columns[i].empty() || !columns[i].isString())
		{
			rst->err = "field exception";
			return rst;
		}
		rst->rst->column.push_back(columns[i].asString());
		rst->rst->columnIndx[columns[i].asString()] = i;
	}
	Json::Value values = series[(uint) 0]["values"];
	if (values.isNull() || !values.isArray() || values.empty())
	{
		rst->err = "values exception";
		return rst;
	}
	for (uint i = 0; i < values.size(); ++i)
	{
		if (values[i].empty() || !values[i].isArray() || values[i].empty())
		{
			rst->err = "row exception";
			return rst;
		}
		if (values[i].size() != rst->rst->column.size())
		{
			rst->err = "column and filed`s value not match";
			return rst;
		}
		vector<string> vec;
		for (uint j = 0; j < values[i].size(); ++j)
		{
			auto val = values[i][j];
			if (val.isString())
				vec.push_back(val.asString());
			else if (val.isBool())
				vec.push_back(val.asBool() ? "1" : "0");
			else if (val.isDouble()) 
			{
				string str;
				SPRINTF_STRING(&str, "%llu", val.asDouble())
				vec.push_back(str);
			} else if (val.isInt() || val.isUInt())
			{
				string str;
				SPRINTF_STRING(&str, "%u", val.asUInt())
				vec.push_back(str);
			} else
			{
				rst->err = "unexpected column type";
				return rst;
			}
		}
		InfluxDbRow* row = new InfluxDbRow(vec);
		rst->rst->row.push_back(row);
	}
	return rst;
}

shared_ptr<InfluxDbResultSet> InfluxDb::httpPost(map<string, string> extra, const string& endPoint, const string& query, const string& dat)
{
	CURL *curl = ::curl_easy_init();
	struct curl_slist* header = NULL;
	for (auto& it : extra)
	{
		string str;
		SPRINTF_STRING(&str, "%s: %s", it.first.c_str(), it.second.c_str())
		header = curl_slist_append(header, str.c_str());
	}
	if (this->https)
	{
		::curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		::curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	::curl_easy_setopt(curl, CURLOPT_URL, (this->host + endPoint + "?" + query).c_str());
	::curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
	::curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);
	::curl_easy_setopt(curl, CURLOPT_POST, 1);
	::curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dat.data());
	::curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, dat.length());
	string rspHeader;
	string rspBody;
	::curl_easy_setopt(curl, CURLOPT_WRITEHEADER, (void* ) &rspHeader);
	::curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_rsp_header_cb);
	::curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void* ) &rspBody);
	::curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_rsp_cb);
	CURLcode res = ::curl_easy_perform(curl);
	shared_ptr<InfluxDbResultSet> rst(new InfluxDbResultSet());
	if (res != CURLE_OK)
	{
		rst->err.assign(::curl_easy_strerror(res));
		::curl_slist_free_all(header);
		::curl_easy_cleanup(curl);
		return rst;
	}
	if (rspBody.empty())
		return rst;
	::curl_slist_free_all(header);
	::curl_easy_cleanup(curl);
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(rspBody, root) || !root.isObject())
	{
		rst->err = "parse response json failed";
		return rst;
	}
	if (!root["error"].isNull())
	{
		rst->err = root["error"].asString();
		return rst;
	}
	return rst;
}

string InfluxDb::urlEncode(const string& url)
{
	CURL* curl = ::curl_easy_init();
	char* ret = ::curl_easy_escape(curl, url.c_str(), url.length());
	string str(ret);
	::curl_free(ret);
	return str;
}

InfluxDb::~InfluxDb()
{

}

