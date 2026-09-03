/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      lizhen
Version:     1.0
Date:        2025-8-19 12:13:28
Description: 低频工艺数据发送
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE(tmsm_l2_mess_auto_snd)
int f_tmsm_l2_mess_auto_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int mm2b_count = 0;
	CString sqlstr = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd_sql(conn);
	CModel ttmsm_heat("TTMSM_HEAT");
	CModel tmmsm2b("TMMSM2B");
	CModel ttmsm_mat("TTMSM_MAT");

	try
	{
		
		sqlstr = " select * from TTMSM_HEAT WHERE SEND_FLAG='0' and rownum<=100 ";
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{
			
			cmd_sql.Fetch(ttmsm_heat);
			if (epex.Initialize("T82311") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue( "T1",0, ttmsm_heat) < 0)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (epex.SendTele() < 0)
			{
				Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			epex.Uninitialize();
			ttmsm_heat["REC_REVISE_TIME"] = datetime;
			ttmsm_heat["SEND_FLAG"] = "1";
			ttmsm_heat.Update("REC_REVISE_TIME,SEND_FLAG","HEAT_NO");
		}
		cmd_sql.Close();

		sqlstr = " select * from tmmsm2b where REC_CREATE_TIME>=TO_CHAR(SYSDATE-10/24/60,'yyyyMMddhh24MISS') ";
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{
			Log::Trace("", "", "mm2b_count", mm2b_count++);
			cmd_sql.Fetch(tmmsm2b);
			if (epex.Initialize("T82311") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("TMMSM2B", 0, tmmsm2b) < 0)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (epex.SendTele() < 0)
			{
				Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			epex.Uninitialize();
			
		}
		cmd_sql.Close();
		
		sqlstr = " select * from TTMSM_MAT WHERE SEND_FLAG='0' and rownum<=100 ";
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{

			cmd_sql.Fetch(ttmsm_mat);
			if (epex.Initialize("T82323") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue(0, ttmsm_mat) < 0)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (epex.SendTele() < 0)
			{
				Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			epex.Uninitialize();
			ttmsm_mat["REC_REVISE_TIME"] = datetime;
			ttmsm_mat["SEND_FLAG"] = "1";
			ttmsm_mat.Update("REC_REVISE_TIME,SEND_FLAG", "SLAB_NO");
		}
		cmd_sql.Close();

		sqlstr = " select HEAT_NO, sum(CUT_SCRAP_WT) CUT_SCRAP_WT from tmmsm39 \
		where HEAT_NO in(Select HEAT_NO from tmmsm31 where START_TIME > to_char(sysdate - 6 / 24, 'yyyyMMddHHMISS')) \
			group by HEAT_NO ";
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{

			
			if (epex.Initialize("T82311") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("T39", "HEAT_NO", 0, cmd_sql.GetString(1)) < 0
				|| epex.SetValue("T39", "CUT_SCRAP_WT", 0, cmd_sql.GetDecimal(2)) < 0)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (epex.SendTele() < 0)
			{
				Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			epex.Uninitialize();

		}
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


