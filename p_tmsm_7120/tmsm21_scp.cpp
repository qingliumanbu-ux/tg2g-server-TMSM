/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sjh
Version:    1.0
Date:     2013-11-13
Description: 中间罐基本信息管理_报废
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
#include "ttmsm21.h"
#include "htmsm21.h"
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
//   中间罐基本信息__报废
/// <para>
/// 1.根据输入参数以炼钢单元号为基础报废《中间罐基本信息表》中的相应信息。
/// <para>数据库表： ttmsm21	中间罐基本信息表 </para>
/// <para>数据库表： htmsm21	中间罐基本信息历史表 </para>
/// </summary>
/// <param name="SM_UNIT_NO">炼钢单元号               </param>
/// <returns>指定炼钢单元号下的报废命令信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm21_scp)

int f_tmsm21_scp(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int i;
	int RowCount = 0;

	CString  sqlstr = "";

	CTTMSM21 ttmsm21(conn);
	CHTMSM21 htmsm21(conn);

	CDbCommand cmd(conn);	
	CDbCommand cmd_inq(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		/* ***** 程序处理 ***** */	
		ttmsm21.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		ttmsm21.TD_STATUS = "99";
		ttmsm21.SCRAP_TIME = datetimeNow;
		ttmsm21.SCRAP_MAKER = s.userid;

		Log::Trace(" ", "tmsm21_scp", "ttmsm21.SM_UNIT_NO[{0}]"		,(const char*)ttmsm21.SM_UNIT_NO);
		Log::Trace(" ", "tmsm21_scp", "ttmsm21.TD_NO[{0}]"	,(const char*)ttmsm21.TD_NO);
		Log::Trace(" ", "tmsm21_scp", "ttmsm21.SCRAP_TIME[{0}]"		,(const char*)ttmsm21.SCRAP_TIME);
		Log::Trace(" ", "tmsm21_scp", "ttmsm21.SCRAP_MAKER[{0}]"	,(const char*)ttmsm21.SCRAP_MAKER);

		ttmsm21.Update("TD_STATUS,SCRAP_TIME","SM_UNIT_NO,TD_NO");			

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT * FROM TTMSM21 WHERE 1=1 " ;
				if (ttmsm21.TD_NO.Trim()   != "")  
				{
					sqlstr = sqlstr + " AND TD_NO = @ttmsm21.TD_NO ";
				}
				if (ttmsm21.SM_UNIT_NO.Trim()      != "") 
				{
					sqlstr = sqlstr + " AND SM_UNIT_NO    = @ttmsm21.SM_UNIT_NO ";
				}
				sqlstr = sqlstr +" ORDER BY TD_NO ASC ";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("ttmsm21.TD_NO"		,ttmsm21.TD_NO);
		cmd_inq.Parameters.Set("ttmsm21.SM_UNIT_NO"	,ttmsm21.SM_UNIT_NO);
		cmd_inq.ExecuteReader();
		if ( cmd_inq.Read() )
		{
			cmd_inq.Fetch(ttmsm21);//把数据都压在头文件里面
		}
		cmd_inq.Close();
	
		htmsm21.CopyFrom(ttmsm21);
		htmsm21.Insert();
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
	return doFlag;

}