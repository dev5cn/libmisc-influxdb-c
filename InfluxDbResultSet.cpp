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

#include "InfluxDbResultSet.h"

InfluxDbResultSet::InfluxDbResultSet()
{
	this->rst = NULL;
}

bool InfluxDbResultSet::getBool(int row, int column)
{
	return this->getRow(row)->getBool(column);
}

uint InfluxDbResultSet::getInt(int row, int column)
{
	return this->getRow(row)->getInt(column);
}

ullong InfluxDbResultSet::getLong(int row, int column)
{
	return this->getRow(row)->getLong(column);
}

string InfluxDbResultSet::getString(int row, int column)
{
	return this->getRow(row)->getString(column);
}

bool InfluxDbResultSet::getBool(int row, const string& column)
{
	return this->getRow(row)->getBool(this->columnIndx(column));
}

uint InfluxDbResultSet::getInt(int row, const string& column)
{
	return this->getRow(row)->getInt(this->columnIndx(column));
}

ullong InfluxDbResultSet::getLong(int row, const string& column)
{
	return this->getRow(row)->getLong(this->columnIndx(column));
}

string InfluxDbResultSet::getString(int row, const string& column)
{
	return this->getRow(row)->getString(this->columnIndx(column));
}

bool InfluxDbResultSet::success()
{
	return this->err.empty();
}

bool InfluxDbResultSet::empty()
{
	return this->rst == NULL || this->rst->row.empty();
}

int InfluxDbResultSet::rowSize()
{
	return this->rst == NULL ? 0 : (int) this->rst->row.size();
}

int InfluxDbResultSet::columnSize()
{
	return this->rst == NULL ? 0 : (int) this->rst->column.size();
}

string InfluxDbResultSet::getColumn(int indx)
{
	return this->rst == NULL ? NULL : this->rst->column.at(indx);
}

InfluxDbRow* InfluxDbResultSet::getRow(int indx)
{
	return this->rst == NULL ? NULL : this->rst->row.at(indx);
}

vector<string>& InfluxDbResultSet::getColumn()
{
	return this->rst->column;
}

int InfluxDbResultSet::columnIndx(const string& column)
{
	auto it = this->rst->columnIndx.find(column);
	return it == this->rst->columnIndx.end() ? -1 : it->second;
}

string InfluxDbResultSet::toString()
{
	if (!this->err.empty())
		return this->err;
	string str;
	for (int i = 0; i < this->rowSize(); ++i)
	{
		SPRINTF_STRING(&str, "%s\n", this->getRow(i)->toString(this->rst->column).c_str())
	}
	return str;
}

InfluxDbResultSet::~InfluxDbResultSet()
{
	if (this->rst != NULL)
	{
		for (size_t i = 0; i < this->rst->row.size(); ++i)
			delete this->rst->row[i];
		delete this->rst;
	}
	this->rst = NULL;

}

