/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      971316
Version:     1.0
Date:        2023-06-15 16:22:18
Description: 查询子项信息管理
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsm37_inq2)


int f_tmsm37_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 业务变量 */
	CString	datetime("");
	
	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);

	/* 实体类定义 */
	CModel ttmsm37("TTMSM37");
	

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;

	try
	{
		
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		Log::Trace("", __FUNCTION__, "datetime		= [{0}]", datetime);
		
		if (bcls_rec->Tables[0].Columns.Contains("IRON_LADLE_NO"))
			ttmsm37["IRON_LADLE_NO"] = bcls_rec->Tables[0].Rows[0]["IRON_LADLE_NO"];

	
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	            // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	            // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = " SELECT  t.*       "
				" FROM    TTMSM37t WHERE 1=1";

			if (ttmsm37["IRON_LADLE_NO"].ToString().Trim() != "" )
			{
				sqlstr = sqlstr + "  AND t.IRON_LADLE_NO = @ttmsm37.IRON_LADLE_NO ";
			}

			sqlstr = sqlstr + "  ORDER BY t.IRON_LADLE_NO ";

			break;
		}

		bcls_ret->Tables.Add();
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("ttmsm37.IRON_LADLE_NO", ttmsm37["IRON_LADLE_NO"].ToString());
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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


