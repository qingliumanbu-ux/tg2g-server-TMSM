/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2111
Version:     1.0
Date:        2023-11-08
Description: 工器具配置项刷新
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tm009a_inq)


int f_tm009a_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString s_userid("");


	/* 实体类定义 */
	

	/* 数据库SQL操作字符串 */
	CString sqlstr("");

	/* 数据库操作类定义 */

	CDbCommand cmd_inq(conn);

	

	try
	{
		

		sqlstr =
			"	WITH TM0 AS (	"
			"	select t.table_name, t.COLUMN_NAME, t.DATA_TYPE, c.COMMENTS, t.DATA_LENGTH,t.COLUMN_ID "
			"	from user_tab_columns t, "
			"	user_col_comments c "
			"	where t.table_name = c.table_name "
			"	and t.COLUMN_NAME = c.column_name) "
			"	SELECT B.*FROM TTM0000 A LEFT JOIN TM0 B ON A.TABLE_NAME = B.TABLE_NAME "
			"	WHERE A.STATION_ID = '"+bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString() + "' ";
		if (bcls_rec->Tables[0].Rows[0]["COLUMN_NAME"].ToString().Trim() != "") {
			sqlstr += " AND B.COLUMN_NAME LIKE @column_name ";
		}
		sqlstr += "  ORDER BY COLUMN_ID ";
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("column_name", bcls_rec->Tables[0].Rows[0]["COLUMN_NAME"].ToString() + "%");
		
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


