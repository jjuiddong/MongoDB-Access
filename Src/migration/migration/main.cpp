//
// 2020-06-12, jjuiddong
// migraion database MySQL -> MongoDB
//
#include "../../../../Common/Common/common.h"
#include "../../../../Common/Network2/network2.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#pragma comment( lib, "libmysql.lib" )

uint GetTableCount(MySQLConnection &con, const string &tableName);
bool Migration(MySQLConnection &con, const string &fromTableName
	, const string &toTableName);

int main()
{
	common::cConfig config("config.txt");

	// connect db server
	const string dbIp = config.GetString("db_ip", "127.0.0.1");
	const int dbPort = config.GetInt("db_port", 10002);
	const string dbId = config.GetString("db_id", "root");
	const string dbPasswd = config.GetString("db_passwd", "root");
	const string dbName = config.GetString("db_name", "db");
	MySQLConnection sqlCon;
	if (sqlCon.Connect(dbIp, dbPort, dbId, dbPasswd, dbName))
	{
		std::cout << "Success MySQL DB Connection\n";
	}
	else
	{
		std::cout << "DB Connection Error\n";
		return 0;
	}

	mongocxx::instance inst{};
	
	Migration(sqlCon, "navi.user", "users");
	//Migration(sqlCon, "navi.journey_date", "journeys");
	//Migration(sqlCon, "navi.landmark", "landmarks");
	//Migration(sqlCon, "navi.path", "paths");
}


// return table count
uint GetTableCount(MySQLConnection &con, const string &tableName)
{
	if (!con.IsConnected())
		return 0;

	const string sql = common::format("SELECT COUNT(*) FROM %s", tableName.c_str());
	MySQLQuery query(&con, sql);
	query.ExecuteQuery();

	const uint row = query.GetResultRowCount();
	const uint col = query.GetFieldCount();
	if (row == 1 && col == 1)
	{
		const uint cnt = (uint)query.getInt(1, "COUNT(*)");
		return cnt;
	}
	return 0;
}


// migration MySQL -> MongoDB
// fromTableName -> toTableName
// mongodb database: navi
bool Migration(MySQLConnection &con
	, const string &fromTableName
	, const string &toTableName)
{
	if (!con.IsConnected())
		return false;

	// language(Hangul) setting
	MySQLQuery query(&con, "set names euckr");
	query.ExecuteQuery();

	mongocxx::client conn{ mongocxx::uri{} };
	auto collection = conn["navi"][toTableName];

	const uint cnt = GetTableCount(con, fromTableName);
	for (uint i = 0; i < cnt; )
	{
		const uint start = i;
		const uint size = std::min(cnt - i, (uint)30);
		const string sql =
			common::format("SELECT * FROM %s LIMIT %d, %d",
				fromTableName.c_str(), start, size);
		MySQLQuery query(&con, sql);
		query.ExecuteQuery();

		const uint row = query.GetResultRowCount();
		const uint col = query.GetFieldCount();
		for (uint k = 0; k < row; ++k)
		{
			bsoncxx::builder::stream::document document{};
			for (uint m = 0; m < col; ++m)
			{
				const string fieldName = query.getFieldName(m+1);
				if (fieldName.empty())
					continue;

				// converting UTF-8
				const string data = query.getString(k + 1, fieldName);
				Str512 str = data;
				document << fieldName << str.utf8().c_str();
			}
			collection.insert_one(document.view());
		}

		i += size;
	}
	return true;
}
