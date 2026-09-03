/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-07 12:13:04
Description: 查询工器具子设备
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme61_inq2)


int f_tmsme61_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 业务变量 */
	CString	datetime("");
	CString	datetemp("");
	CDecimal stop_use_time = 0;

	CString	sm_plan_no("");
	CString	furnace_no_bof("");
	CString	furnace_no_lf("");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sql_pssm13 = "";
	CString sql_tmsm01 = "";
	CString sql_tmsm02 = "";
	CString sql_tmsm03 = "";
	CString sql_pssm_no1 = "";
	CString sql_pssm_no2 = "";

	/* 数据库操作类定义 */
	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);

	/* 实体类定义 */
	CModel ttmsm61("TTMSM61");
	CModel ttmsm62("TTMSM62");

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;

	try
	{
		//Log::Trace("", __FUNCTION__, "ttmsm31.MD_COPPER_PLATE_NO		= [{0}]", "11111111111111");
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		Log::Trace("", __FUNCTION__, "datetime		= [{0}]", datetime);
		/* ***** 获取输入参数 ***** */
		//if (bcls_rec->Tables["TMSME61_INQS"].Columns.Contains("DEV_TYPE"))
		if (bcls_rec->Tables[0].Columns.Contains("DEV_TYPE"))
			ttmsm62["DEV_TYPE"] = bcls_rec->Tables[0].Rows[0]["DEV_TYPE"];

		//ttmsm41["SM_UNIT_NO"] = "A";

		Log::Trace("", __FUNCTION__, "ttmsm62.DEV_TYPE		= [{0}]", ttmsm62["DEV_TYPE"].ToString());
		/* 查询梅钢设备类型子设备表 */
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	            // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	            // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = " SELECT  t.*       "
				" FROM    TTMSM62 t WHERE 1=1";

			if (ttmsm62["DEV_TYPE"].ToString().Trim() != "" )
			{
				sqlstr = sqlstr + "  AND t.DEV_TYPE = @ttmsm62.DEV_TYPE ";
			}

			sqlstr = sqlstr + "  ORDER BY t.DEV_TYPE,t.SECTION_MODE ";

			break;
		}

		bcls_ret->Tables.Add();
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("ttmsm62.DEV_TYPE", ttmsm62["DEV_TYPE"].ToString());
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


