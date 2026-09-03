/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2111
Version:     1.0
Date:        2023-11-08
Description: 工器具跟踪事件配置项查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tm0099_inq)


int f_tm0099_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString s_userid("");


	/* 实体类定义 */
	CModel ttm0099 = CModel("TTM0099");

	/* 数据库SQL操作字符串 */
	CString sqlstr("");

	/* 数据库操作类定义 */

	CDbCommand cmd_inq(conn);

	
	try
	{
		// 获取前台传入参数
		s_userid = s.userid;

		

	
		ttm0099.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		sqlstr =
			" SELECT * FROM TTM0099 T99 WHERE 1=1 ";
		if (bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString().Trim() != "")
		{
			sqlstr += " AND STATION_ID LIKE @station_id ";
		}
		if (bcls_rec->Tables[0].Rows[0]["EVENT_ID"].ToString().Trim() != "")
		{
			sqlstr += " AND EVENT_ID LIKE @event_id ";
		}
		sqlstr += " ORDER BY REC_CREATE_TIME ";
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);

		cmd_inq.Parameters.Set("station_id", ttm0099["STATION_ID"].ToString() + "%");
		cmd_inq.Parameters.Set("event_id", ttm0099["EVENT_ID"].ToString() + "%");
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();



		

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	cmd_inq.Close();

	return doFlag;
}


