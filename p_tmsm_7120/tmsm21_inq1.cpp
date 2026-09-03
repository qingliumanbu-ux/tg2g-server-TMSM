/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sjh
Version:    1.0
Date:     2013-11-18
Description: 中间罐基本信息_查询
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
#include "ttmsm21.h"
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
//   中间罐基本信息_查询
/// <para>
/// 1.根据输入参数以炼钢单元号、中间罐号为基础查询《中间罐基本信息表》中的相应信息。
/// <para>数据库表： ttmsm21	中间罐基本信息表 </para>
/// </summary>
/// <param name="SM_UNIT_NO">炼钢单元号               </param>
/// <returns>指定炼钢单元号下的查询命令信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm21_inq1)

int f_tmsm21_inq1(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
	
	CString sqlstr = "" ;

	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);

	CTTMSM21 ttmsm21(conn);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int RowCount = 0;
	int recordFrom = 0;
	int pageSize = -1;		// 查询所有记录
	int totalCount = 0;	

	try
	{
		/* ***** 初始化全局变量 ***** */

		/* ***** 获取输入参数 ***** */
		ttmsm21.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Trace(" ", "tmsm21_inq", "SM_UNIT_NO[{0}]"	,(const char*)ttmsm21.SM_UNIT_NO);
		Log::Trace(" ", "tmsm21_inq", "TD_NO[{0}]"		,(const char*)ttmsm21.TD_NO);

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT * FROM TTMSM21 WHERE 1=1 " ;
				if (ttmsm21.SM_UNIT_NO.Trim() != "")
				{
					sqlstr = sqlstr + " AND SM_UNIT_NO = @ttmsm21.SM_UNIT_NO ";
				}
				if (ttmsm21.TD_NO.Trim() != "")
				{
					sqlstr = sqlstr + " AND TD_NO LIKE @ttmsm21.TD_NO ";
				}
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("ttmsm21.SM_UNIT_NO"	,ttmsm21.SM_UNIT_NO);
		cmd_inq.Parameters.Set("ttmsm21.TD_NO"		,ttmsm21.TD_NO + "%");

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	cmd_inq.Close();

	return doFlag;

}