/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      admin
Version:     1.0
Date:
Description: 炼钢设备功能管理项目导入-查询
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE(qxsm23_dr_inq1)


int f_qxsm23_dr_inq1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString start_time = " ";
	CString end_time = " ";
	CString unit = " ";
	CString version = " ";
	CString submit_cycle = " ";
	CString sqlstr0 = "";
	CString sqlstr = "SELECT REC_CREATE_TIME,REC_CREATOR,DEVICENAME,FUNCTION,ITEM_COUNTS,INFLUENCE,KEY_POINT,EVA_CRI,ACC_UNIT,UNIT,CYCLE,MON_METHOD,LEVEL_,SUB_,ROOT_,RESULT_DESC,REASON,OPINION,USER_NAME,TYPE,REMARK,VERSION,TIME,SUBMIT_CYCLE,SUBMIT_USER,ACC_UNIT_USER,PROPOR||'%' PROPOR FROM TTMSMQXDR ";
	CString sql_where = "WHERE 1 = 1";
	CString sql_where2 = " ";
	CString sql_where3 = " ";
	CDbCommand cmd(conn);
	CDbCommand cmd_sql(conn);
	int   fetchRowCount = 0;
	try
	{
		start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim().Substring(0,8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim().Substring(0, 8);
		unit = bcls_rec->Tables[0].Rows[0]["UNIT"].ToString().Trim();
		version = bcls_rec->Tables[0].Rows[0]["VERSION"].ToString().Trim();
		submit_cycle = bcls_rec->Tables[0].Rows[0]["SUBMIT_CYCLE"].ToString().Trim();
		Log::Trace("", "", "start_time = [{0}],end_time = [{1}],unit = [{2}],version = [{3}], submit_cycle = [{4}]", start_time, end_time, unit, version, submit_cycle);
		if (start_time != "" || end_time != "")
		{
			if (start_time.Trim() != "")
			{
				sql_where += " AND TIME >='" + start_time + "'";
			}
			if (end_time.Trim() != "")
			{
				sql_where += " AND TIME <= '" + end_time + "'";
			}
		}
		CString unit_desc = "";
		if (unit != "")
		{  // 按条件查
			if (unit == "YYL"){
				sql_where += " AND UNIT LIKE '%一冶炼%'";
				unit_desc = "一冶炼";
			}
			else if (unit == "EYL"){
				sql_where += " AND UNIT LIKE '%二冶炼%'";
				unit_desc = "二冶炼";
			}
			else if (unit == "SYL"){
				sql_where += " AND UNIT LIKE '%三冶炼%'";
				unit_desc = "三冶炼";
			}
			else if (unit == "YLZ"){
				sql_where += " AND UNIT LIKE '%一连铸%'";
				unit_desc = "一连铸";
			}
			else if (unit == "SLZ"){
				sql_where += " AND UNIT LIKE '%三连铸%'";
				unit_desc = "三连铸";
			}
			else if (unit == "CP"){
				sql_where += " AND UNIT LIKE '%成品%'";
				unit_desc = "成品";
			}
		}
		
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
		sqlstr = sqlstr + sql_where +" ORDER BY REC_CREATE_TIME DESC";
		
		Log::Trace("", "", "sqlstr:{0}", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);
	    cmd.Close();
		
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


