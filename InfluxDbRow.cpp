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

#include "InfluxDbRow.h"

InfluxDbRow::InfluxDbRow(const vector<string>& column)
{
	this->column = column;
}

bool InfluxDbRow::getBool(int indx)
{
	return this->column.at(indx) != "0";
}

uint InfluxDbRow::getInt(int indx)
{
	return ::atoi(this->column.at(indx).c_str());
}

ullong InfluxDbRow::getLong(int indx)
{
	return ::atoll(this->column.at(indx).c_str());
}

string InfluxDbRow::getString(int indx)
{
	return this->column.at(indx);
}

void InfluxDbRow::addTime(const ullong val)
{
	string str;
	SPRINTF_STRING(&str, "%llu", val)
	this->column.push_back(str);
}

string InfluxDbRow::toString(vector<string>& field)
{
	string str;
	for (size_t i = 0; i < this->column.size(); ++i)
	{
		SPRINTF_STRING(&str, "%s=%s%s", field[i].c_str(), this->column[i].c_str(), i + 1 == this->column.size() ? "" : ", ")
	}
	return str;
}

InfluxDbRow::~InfluxDbRow()
{

}

