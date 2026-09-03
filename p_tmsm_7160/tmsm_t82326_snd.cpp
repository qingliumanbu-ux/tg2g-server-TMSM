/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      lizhen
Version:     1.0
Date:        2025-8-19 12:13:28
Description: 低频工艺数据发送
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE(tmsm_t82326_snd)
int f_tmsm_t82326_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int mm2b_count = 0;
	CString sqlstr = "";
	CString datetime_e = CDateTime::Now().ToString("yyyyMMdd000000");
	CString datetime_b = CDateTime::Now().AddDays(-2).ToString("yyyyMMdd000000");
	EPEX epex;
	CDbCommand cmd_sql(conn);
	
	int i = 0;
	try
	{
		if (epex.Initialize("T82326") < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		sqlstr = " SELECT * FROM V_GTL_TG23 WHERE  TAP_END_TIME <= '"+ datetime_e +"'\
			and TAP_END_TIME >= '"+ datetime_b +"' ";
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{

			if (epex.SetValue("HEAT_NO",i, cmd_sql.GetString(1)) < 0
				|| epex.SetValue("MOLECULE", i, cmd_sql.GetDecimal(2)) < 0
				|| epex.SetValue("DENOMINATOR", i, cmd_sql.GetDecimal(3)) < 0
				|| epex.SetValue("GTL", i, cmd_sql.GetDecimal(4)) < 0
				|| epex.SetValue("TAP_END_TIME", i, cmd_sql.GetString(5)) < 0)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			i++;
		}
		

		if (epex.SendTele() < 0)
		{
			Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		epex.Uninitialize();
		cmd_sql.Close();
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


