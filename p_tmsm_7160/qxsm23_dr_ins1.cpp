/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      admin
Version:     1.0
Date:
Description: 炼钢设备功能管理项目导入-查询
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE(qxsm23_dr_ins1)


int f_qxsm23_dr_ins1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	CString sqlstr = "";
	int doFlag = 0;
	CDbCommand cmd(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel tqxsmdr = CModel("TTMSMQXDR");

	try
	{
		CString time = bcls_rec->Tables["time"].Rows[0]["TIME"].ToString().Trim();
		CString unit = bcls_rec->Tables["data"].Rows[0]["UNIT"].ToString().Trim();
		Log::Trace("", "", "time:[{0}],unit:[{1}]", time, unit);

		CString version = "0";
		Log::Trace("", "", "长度：【{0}】", time.GetLength());
		tqxsmdr["TIME"] = time;
		tqxsmdr["UNIT"] = unit;
		Log::Trace("", "", "111");
		if (tqxsmdr.QueryCount("TIME, UNIT") > 0) {
			CString sql_ = "SELECT MAX(VERSION)+1 FROM TTMSMQXDR WHERE TIME = '" + time + "' ";
			Log::Trace("", __FUNCTION__, "查询语句: {0}", sql_);

			cmd.SetCommandText(sql_);
			cmd.ExecuteReader();

			if (cmd.Read())
			{
				version = cmd.GetString(1);

				Log::Trace("", "", "版次version =[{0}]", version);
			}
			cmd.Close();
		}

		int counts = bcls_rec->Tables["data"].Rows.get_Count();
		for (int i = 0; i < counts; i++)
		{
			/* tqxsmdr 初始化*/
			tqxsmdr.Reset();
			
			tqxsmdr.MergeFrom(bcls_rec->Tables["data"].Rows[i]);
			Log::Trace("", "", "222");
			tqxsmdr["REC_CREATE_TIME"] = datetimeNow;
			tqxsmdr["REC_CREATOR"] = s.userid;
			tqxsmdr["VERSION"] = version;
			tqxsmdr["TIME"] = time;
			tqxsmdr["PROPOR"] =tqxsmdr["PROPOR"].ToString().Remove("%");
;
			Log::Trace("", "", "REC_CREATE_TIME[{0}],REC_CREATOR[{1}],VERSION[{2}],TIME[{3}]", datetimeNow, s.userid, version, time);
			Log::Trace("", "", "333");
			/*tqxsmdr.Print();*/
			if (tqxsmdr.Insert())
				Log::Trace("", __FUNCTION__, "子表写表成功。");
			else {
				Log::Trace(" ", " ", "!tqxsmdr.Insert() ");
				CFormattable args[] = { version };
				CMessageFormat::Format(s.msg, "钢卷[{0}]记录写入TQXCR23LB1表失败！", args, 1);
				throw CApplicationException(s.msg);
			}
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


