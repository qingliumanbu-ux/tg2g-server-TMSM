/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sjh
Version:    1.0
Date:     2013-11-14
Description: 钢包基本信息_查询
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
#include "ttmsm01.h"
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
//   钢包基本信息_查询
/// <para>
/// 1.根据输入参数以炼钢单元号、钢包号为基础查询《钢包基本信息表》中的相应信息。
/// <para>数据库表： ttmsm01	钢包基本信息表 </para>
/// </summary>
/// <param name="SM_UNIT_NO">炼钢单元号               </param>
/// <returns>指定炼钢单元号下的查询命令信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm01_inq)

int f_tmsm01_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
	
	CString sqlstr = "" ;

	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);

	CTTMSM01 ttmsm01(conn);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int RowCount = 0;
	int recordFrom = 0;
	int pageSize = -1;		// 查询所有记录
	int totalCount = 0;	

	try
	{
		/* ***** 获取输入参数 ***** */
		ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Trace(" ", "tmsm01_inq", "SM_UNIT_NO[{0}]"	,(const char*)ttmsm01.SM_UNIT_NO);
		Log::Trace(" ", "tmsm01_inq", "LADLE_NO[{0}]"	,(const char*)ttmsm01.LADLE_NO);

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT * FROM TTMSM01 WHERE 1=1 " ;
				if (ttmsm01.SM_UNIT_NO.Trim() != "")
				{
					sqlstr = sqlstr + " AND SM_UNIT_NO    = @ttmsm01.SM_UNIT_NO ";
				}
				if (ttmsm01.LADLE_NO.Trim() != "")
				{
					sqlstr = sqlstr + " AND LADLE_NO LIKE @ttmsm01.LADLE_NO ";
				}
				break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("ttmsm01.SM_UNIT_NO"	,ttmsm01.SM_UNIT_NO);
		cmd_inq.Parameters.Set("ttmsm01.LADLE_NO"	,ttmsm01.LADLE_NO + "%");

		if (bcls_rec->Tables.Contains("PageInfo"))
		{
			cmd_inq.SetCommandText(sqlstr);
			totalCount = cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			bcls_ret->Tables.Add();
			bcls_ret->Tables[1].Columns.Add(DT_INT32, "TOTAL_COUNT");
			bcls_ret->Tables[1].Rows.Add();
			bcls_ret->Tables[1].Rows[0][0] = totalCount;

			// 获得前台传入的分页信息
			recordFrom = (int)bcls_rec->Tables[1].Rows[0]["START"];
			pageSize   = (int)bcls_rec->Tables[1].Rows[0]["PAGE_SIZE"];		
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], recordFrom, pageSize);
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