# libmisc-influxdb-c

### how to build?

* mkdir build;cd build

* git clone https://github.com/dev5cn/libmisc-cpp

* git clone https://github.com/dev5cn/libmisc-influxdb-c

* cd libmisc-influxdb-c

* export LIB_CURL=/home/dev5/tools/libcurl-7.xx.0/

* export CXX_FLAGS="-g3 -O3"

* chmod 775 build.sh clean.sh

* ./build.sh

* done.


### how to use?

```cpp
#include <libmisc-influxdb-c.h>

int main(int argc, char **argv)
{
	shared_ptr<InfluxDb> db(new InfluxDb("http://172.17.0.2:8086/", "dev5", "39e21400e8604b260b441ac5fb6c563b", "x-msg-im-core-db"));
	//
	shared_ptr<InfluxDbInsertReq> req(new InfluxDbInsertReq("ms_x_msg_dst_event"));
	req->addTag( { "tid", "ne", "sne", "dne" })->addFiled( { "sid", "pid", "gts", "pdu" }); /* add tag & fields. */
	req->addRow( { "tid0", "ne0", "sne0", "dne0", "sid0", "pid0", "gts0", "pdu0" })->addTime(DateMisc::nowGmt0() * 1000 * 1000L /* nano second. */);
	req->addRow( { "tid1", "ne1", "sne1", "dne1", "sid1", "pid1", "gts1", "pdu1" })->addTime(DateMisc::nowGmt0() * 1000 * 1000L /* nano second. */+ 1);
	shared_ptr<InfluxDbResultSet> rst = db->insert(req);
	//
	rst = db->queryGet("select * from ms_x_msg_dst_event");
	for (int i = 0; i < rst->rowSize(); ++i)
	{
		LOG_DEBUG("time: %s, tid: %s, ne: %s, sne: %s, dne: %s, sid: %s, pid: %s, gts: %s, pdu: %s", //
				rst->getString(i, "time").c_str(),
				rst->getString(i, "tid").c_str(),
				rst->getString(i, "ne").c_str(),
				rst->getString(i, "sne").c_str(),
				rst->getString(i, "dne").c_str(),
				rst->getString(i, "sid").c_str(),
				rst->getString(i, "pid").c_str(),
				rst->getString(i, "gts").c_str(),
				rst->getString(i, "pdu").c_str())
	}
	LOG_DEBUG("result-set:\n%s", rst->toString().c_str())
}
```




```js
> select * from ms_x_msg_dst_event;
name: ms_x_msg_dst_event
time                dne  gts  ne  pdu  pid  sid  sne  tid
----                ---  ---  --  ---  ---  ---  ---  ---
1571801331292000000 dne0 gts0 ne0 pdu0 pid0 sid0 sne0 tid0
1571801331292000001 dne1 gts1 ne1 pdu1 pid1 sid1 sne1 tid1
```                                                                                   
