/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      admin
Version:     1.0
Date:
Description: 炼钢设备功能管理趋势-查询
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE(qxsm23_qs_inq1)


int f_qxsm23_qs_inq1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString start_time = " ";
	CString end_time = " ";
	CString unit = " ";
	CString version = " ";
	CString devname = " ";
	CString func = " ";
	CString unit2 = " ";
	CString devname2 = " ";
	CString func2 = " ";
	CString submit_cycle = " ";
	CString sqlstr0 = "";
	CString sqlstr1 = "";
	CString sqlstr2 = "";
	
	CString sql_where = "WHERE 1 = 1";
	CString sql_where2 = "WHERE 1 = 1 ";
	CDbCommand cmd(conn);
	CDbCommand cmd_sql(conn);
	int   fetchRowCount = 0;
	try
	{
		start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim().Substring(0, 8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim().Substring(0, 8);
		unit = bcls_rec->Tables[0].Rows[0]["UNIT"].ToString().Trim();
		devname = bcls_rec->Tables[0].Rows[0]["DEVICENAME"].ToString().Trim();
		func = bcls_rec->Tables[0].Rows[0]["FUNCTION"].ToString().Trim();
		unit2 = bcls_rec->Tables[0].Rows[0]["UNIT2"].ToString().Trim();
		devname2 = bcls_rec->Tables[0].Rows[0]["DEVICENAME2"].ToString().Trim();
		func2 = bcls_rec->Tables[0].Rows[0]["FUNCTION2"].ToString().Trim();
		
		Log::Trace("", "", "start_time = [{0}],end_time = [{1}],unit = [{2}],version = [{3}], submit_cycle = [{4}]", start_time, end_time, unit, version, submit_cycle);
		sqlstr1 = "SELECT REC_CREATE_TIME,REC_CREATOR,DEVICENAME,TIME";
		sqlstr2 = "SELECT REC_CREATE_TIME,REC_CREATOR,DEVICENAME,TIME";
		if (start_time != "" || end_time != "")
		{
			
			if (start_time.Trim() != "")
			{
				sql_where += " AND TIME >='" + start_time + "'";
				sql_where2 += " AND TIME >='" + start_time + "'";
			}
			if (end_time.Trim() != "")
			{
				sql_where += " AND TIME <= '" + end_time + "'";
				sql_where2 += " AND TIME <= '" + end_time + "'";
			}
		}
		if (unit != "")
		{  // 按条件查

			sql_where += " AND UNIT LIKE '%" + unit + "%'";

		}
		else
		{
			sqlstr1 += "||UNIT";
		}
		if (devname != "")
		{  // 按条件查

			sql_where += " AND DEVICENAME LIKE '%" + devname + "%'";
		}
		else

		{
			sqlstr1 += "||'-'||DEVICENAME";
		}
		if (func != "")
		{  // 按条件查

			sql_where += " AND FUNCTION LIKE '%" + func + "%'";

		}
		else
		{
			sqlstr1 += "|| '-' || FUNCTION";
		}
		if (unit2 != "")
		{  // 按条件查

			sql_where2 += " AND UNIT LIKE '%" + unit2 + "%'";

		}
		else
		{
			sqlstr2 += "||UNIT";
		}
		if (devname2 != "")
		{  // 按条件查

			sql_where2 += " AND DEVICENAME LIKE '%" + devname2 + "%'";
		}
		else

		{
			sqlstr2 += "||'-'||DEVICENAME";
		}
		if (func2 != "")
		{  // 按条件查

			sql_where2 += " AND FUNCTION LIKE '%" + func2 + "%'";

		}
		else
		{
			sqlstr2 += "|| '-' || FUNCTION";
		}
		sqlstr1 += " as FUNCTION,ITEM_COUNTS,INFLUENCE,KEY_POINT,EVA_CRI,ACC_UNIT,UNIT,CYCLE,MON_METHOD,LEVEL_,SUB_,ROOT_,RESULT_DESC,REASON,OPINION,USER_NAME,TYPE,REMARK,VERSION,TIME,SUBMIT_CYCLE,SUBMIT_USER,ACC_UNIT_USER,PROPOR FROM TTMSMQXDR ";
		sqlstr2 += " as FUNCTION,ITEM_COUNTS,INFLUENCE,KEY_POINT,EVA_CRI,ACC_UNIT,UNIT,CYCLE,MON_METHOD,LEVEL_,SUB_,ROOT_,RESULT_DESC,REASON,OPINION,USER_NAME,TYPE,REMARK,VERSION,TIME,SUBMIT_CYCLE,SUBMIT_USER,ACC_UNIT_USER,PROPOR FROM TTMSMQXDR ";
		// 提报周期
		if (submit_cycle != "")
		{
			if (submit_cycle == "02"){
				sql_where += " AND SUBMIT_CYCLE LIKE '%1D%'";
			}
			else if (submit_cycle == "03"){
				sql_where += " AND SUBMIT_CYCLE LIKE '%1M%'";
			}
			else if (submit_cycle == "04"){
				sql_where += " AND SUBMIT_CYCLE LIKE '%1W%'";
			}
			else if (submit_cycle == "05"){
				sql_where += " AND SUBMIT_CYCLE LIKE '%2W%'";
			}
			else if (submit_cycle == "06"){
				sql_where += " AND SUBMIT_CYCLE LIKE '%检修后%'";
			}
			else if (submit_cycle == "07"){
				sql_where += " AND SUBMIT_CYCLE LIKE '%上机时%'";
			}
			else if (submit_cycle == "08"){
				sql_where += " AND SUBMIT_CYCLE LIKE '下线后%'";
			}
		}
		sqlstr1 = sqlstr1 + sql_where ;
		sqlstr2 = sqlstr2 + sql_where2;
		sqlstr1 = "SELECT t.FUNCTION,t.PROPOR, r.PROPOR as PROPOR2,CASE WHEN t.EVA_CRI like '≥%'  THEN TO_NUMBER(substr(t.EVA_CRI,instr(t.EVA_CRI,'≥')+1,instr(t.EVA_CRI,'%')-instr(t.EVA_CRI,'≥')-1)) ELSE TO_NUMBER(t.EVA_CRI) END as EVA_CRI1,CASE WHEN r.EVA_CRI like '≥%'  THEN TO_NUMBER(substr(r.EVA_CRI,instr(r.EVA_CRI,'≥')+1,instr(r.EVA_CRI,'%')-instr(r.EVA_CRI,'≥')-1)) ELSE TO_NUMBER(r.EVA_CRI) END as EVA_CRI2 from("+sqlstr1 + ")t left join (" + sqlstr2+") r on t.FUNCTION=r.FUNCTION order by t.FUNCTION";
		Log::Trace("", "", "sqlstr:{0}", sqlstr1);
		cmd.SetCommandText(sqlstr1);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);
		
		cmd.Close();

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr1 + "\r\n" + ex.GetMsg();
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

