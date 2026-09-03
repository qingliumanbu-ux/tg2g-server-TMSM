/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    杨扬
Version:   1.0
Date:      2014-07-07
Description: 铜板详细信息_查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"



/*<remark>=========================================================
/// <summary>
///  钢包配包信息_查询
/// <para>
/// 根据输入参数“炼钢计划号”“钢包号”查询《炼钢计划表》《钢包基本信息表》《钢包配包信息表》中的相应信息。
/// <para>数据库表： ttmsm01   钢包基本信息表 </para>
/// <para>数据库表： ttmsm02   钢包配包信息表 </para>
/// <para>数据库表： tpssm11   炼钢计划信息表 </para>
/// <summary>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm41a2_inq)

int f_tmsm41a2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CModel ttmsm41("TTMSM41");
	CModel ttmsm42("TTMSM42");

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;

	try
	{
		Log::Trace("", __FUNCTION__, "ttmsm31.MD_COPPER_PLATE_NO		= [{0}]", "11111111111111");
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		Log::Trace("", __FUNCTION__, "datetime		= [{0}]", datetime);
		/* ***** 获取输入参数 ***** */
		if (bcls_rec->Tables["TMSM41A2_INQC"].Columns.Contains("MD_COPPER_PLATE_NO"))
			ttmsm41["MD_COPPER_PLATE_NO"] = bcls_rec->Tables["TMSM41A2_INQC"].Rows[0]["MD_COPPER_PLATE_NO"];
		if (bcls_rec->Tables["TMSM41A2_INQC"].Columns.Contains("TM_COPPER_NO"))
			ttmsm41["TM_COPPER_NO"] = bcls_rec->Tables["TMSM41A2_INQC"].Rows[0]["TM_COPPER_NO"];
		if (bcls_rec->Tables["TMSM41A2_INQC"].Columns.Contains("TM_COPPER_TYPE"))
			ttmsm41["TM_COPPER_TYPE"] = bcls_rec->Tables["TMSM41A2_INQC"].Rows[0]["TM_COPPER_TYPE"];

		ttmsm41["SM_UNIT_NO"] = "A";

		Log::Trace("", __FUNCTION__, "ttmsm31.MD_COPPER_PLATE_NO		= [{0}]", ttmsm41["MD_COPPER_PLATE_NO"].ToString());
		Log::Trace("", __FUNCTION__, "ttmsm31.TM_COPPER_NO		= [{0}]", ttmsm41["TM_COPPER_NO"].ToString());
		Log::Trace("", __FUNCTION__, "ttmsm31.TM_COPPER_TYPE		= [{0}]", ttmsm41["TM_COPPER_TYPE"].ToString());
		Log::Trace("", __FUNCTION__, "ttmsm31.SM_UNIT_NO		= [{0}]", ttmsm41["SM_UNIT_NO"].ToString());
		/* 查询铜板一区信息表 */
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	            // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	            // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = " SELECT  t.*       "
				" FROM    TTMSM41 t "
				;

			if (ttmsm41["SM_UNIT_NO"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + " WHERE t.SM_UNIT_NO = @ttmsm41.SM_UNIT_NO ";
			}
			if (ttmsm41["MD_COPPER_PLATE_NO"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + " AND t.MD_COPPER_PLATE_NO   = @ttmsm41.MD_COPPER_PLATE_NO ";
			}
			if (ttmsm41["TM_COPPER_NO"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + " AND t.TM_COPPER_NO=@ttmsm41.TM_COPPER_NO";
			}
			if (ttmsm41["TM_COPPER_TYPE"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + "AND t.TM_COPPER_TYPE=@ttmsm41.TM_COPPER_TYPE ";
				if (ttmsm41["TM_COPPER_TYPE"].ToString().Trim() == "0")
				{
					sqlstr = sqlstr + " AND  t.BACK_C1='01' ";
				}
				else
				{
					sqlstr = sqlstr + " AND  t.BACK_C1='03' ";
				}
			}

			sql_tmsm01 = sqlstr;

			break;
		}
		sql_tmsm01 = sqlstr;

		bcls_ret->Tables.Add();
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("ttmsm41.SM_UNIT_NO", ttmsm41["SM_UNIT_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm41.MD_COPPER_PLATE_NO", ttmsm41["MD_COPPER_PLATE_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm41.TM_COPPER_NO", ttmsm41["TM_COPPER_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm41.TM_COPPER_TYPE", ttmsm41["TM_COPPER_TYPE"].ToString());
		cmd_inq.SetCommandText(sql_tmsm01);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "sql_tmsm01		= [{0}]", sql_tmsm01);


		//查询铜板二区信息
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	            // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	            // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = " SELECT  t.*       "
				" FROM    TTMSM41 t "
				;

			if (ttmsm41["SM_UNIT_NO"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + " WHERE t.SM_UNIT_NO = @ttmsm41.SM_UNIT_NO ";
			}
			if (ttmsm41["MD_COPPER_PLATE_NO"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + " AND t.MD_COPPER_PLATE_NO   = @ttmsm41.MD_COPPER_PLATE_NO ";
			}
			if (ttmsm41["TM_COPPER_NO"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + " AND t.TM_COPPER_NO=@ttmsm41.TM_COPPER_NO";
			}
			if (ttmsm41["TM_COPPER_TYPE"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + "AND t.TM_COPPER_TYPE=@ttmsm41.TM_COPPER_TYPE";
				if (ttmsm41["TM_COPPER_TYPE"].ToString().Trim() == "0")
				{
					sqlstr = sqlstr + " AND  t.BACK_C1='02' ";
				}
				else
				{
					sqlstr = sqlstr + " AND  t.BACK_C1='04' ";
				}
			}

			sql_tmsm02 = sqlstr;

			break;
		}
		sql_tmsm02 = sqlstr;

		bcls_ret->Tables.Add();
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("ttmsm41.SM_UNIT_NO", ttmsm41["SM_UNIT_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm41.MD_COPPER_PLATE_NO", ttmsm41["MD_COPPER_PLATE_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm41.TM_COPPER_NO", ttmsm41["TM_COPPER_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm41.TM_COPPER_TYPE", ttmsm41["TM_COPPER_TYPE"].ToString());
		cmd_inq.SetCommandText(sql_tmsm02);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "ttmsm31.SM_UNIT_NO		= [{0}]", ttmsm41["SM_UNIT_NO"].ToString());

		//查询铜板配对表内信息
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	            // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	            // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = " SELECT  t.*       "
				" FROM    TTMSM42 t  WHERE 1=1  ";
			if (ttmsm41["MD_COPPER_PLATE_NO"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + "  AND t.MD_COPPER_PLATE_NO   = @ttmsm41.MD_COPPER_PLATE_NO ";
			}
			if (ttmsm41["TM_COPPER_TYPE"].ToString().Trim()!="")
			{
				sqlstr = sqlstr + "  AND t.TM_COPPER_TYPE=@ttmsm41.TM_COPPER_TYPE  ";
			}
			sql_tmsm01 = sqlstr;

			break;
		}
		sql_tmsm03 = sqlstr;
		Log::Trace("", __FUNCTION__, "sql_tmsm03		= [{0}]", sql_tmsm03);
		bcls_ret->Tables.Add();
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("ttmsm41.MD_COPPER_PLATE_NO", ttmsm41["MD_COPPER_PLATE_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm41.TM_COPPER_TYPE", ttmsm41["TM_COPPER_TYPE"].ToString());
		cmd_inq.SetCommandText(sql_tmsm03);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "ttmsm41.SM_UNIT_NO		= [{0}]", ttmsm41["SM_UNIT_NO"].ToString());

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
