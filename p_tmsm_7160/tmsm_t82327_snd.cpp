/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      lizhen
Version:     1.0
Date:        2025-8-19 12:13:28
Description: 低频工艺数据发送
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE(tmsm_t82327_snd)
int f_tmsm_t82327_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int mm2b_count = 0;
	CString sqlstr = "";
	CString datetime_e = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString datetime_b = CDateTime::Now().AddHours(-2).ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd_sql(conn);
	CModel ttmsm203("TTMSM203");
	int i = 0;
	try
	{
		if (epex.Initialize("T82327") < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		sqlstr = " SELECT * FROM ttmsm203 WHERE  (REC_CREATE_TIME <= '" + datetime_e + "'\
			and REC_CREATE_TIME >= '" + datetime_b + "') or (REC_REVISE_TIME>='"+ datetime_e +"' and REC_REVISE_TIME<='"+ datetime_b +"') ";
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{
			ttmsm203.Reset();
			cmd_sql.Fetch(ttmsm203);
			if (epex.SetValue( i, ttmsm203) < 0)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			i++;
		}
		if (i > 0)
		{
			if (epex.SendTele() < 0)
			{
				Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
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


