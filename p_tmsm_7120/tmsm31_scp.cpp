/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sjh
Version:    1.0
Date:     2013-11-13
Description: 结晶器基本信息管理_报废
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
#include "ttmsm31.h"
#include "htmsm31.h"
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
//   结晶器基本信息__报废
/// <para>
/// 1.根据输入参数以炼钢单元号为基础报废《结晶器基本信息表》中的相应信息。
/// <para>数据库表： ttmsm31	结晶器基本信息表 </para>
/// <para>数据库表： htmsm31	结晶器基本信息历史表 </para>
/// </summary>
/// <param name="SM_UNIT_NO">炼钢单元号               </param>
/// <returns>指定炼钢单元号下的报废命令信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm31_scp)

int f_tmsm31_scp(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int i;
	int RowCount = 0;

	CString  sqlstr = "";

	CTTMSM31 ttmsm31(conn);
	CHTMSM31 htmsm31(conn);

	CDbCommand cmd(conn);	
	CDbCommand cmd_inq(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		/* ***** 程序处理 ***** */	
		ttmsm31.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		ttmsm31.CURRENT_STATUS = "99";
		ttmsm31.SCRAP_TIME = datetimeNow;
		ttmsm31.SCRAP_MAKER = s.userid;

		Log::Trace(" ", "tmsm31_scp", "ttmsm31.SM_UNIT_NO[{0}]"		,(const char*)ttmsm31.SM_UNIT_NO);
		Log::Trace(" ", "tmsm31_scp", "ttmsm31.MOLD_NO[{0}]"		,(const char*)ttmsm31.MOLD_NO);
		Log::Trace(" ", "tmsm31_scp", "ttmsm31.MOLD_SECTION[{0}]"	,(const char*)ttmsm31.MOLD_SECTION);
		Log::Trace(" ", "tmsm31_scp", "ttmsm31.CC_MACH_NO[{0}]"		,(const char*)ttmsm31.CC_MACH_NO);
		Log::Trace(" ", "tmsm31_scp", "ttmsm31.SCRAP_TIME[{0}]"		,(const char*)ttmsm31.SCRAP_TIME);
		Log::Trace(" ", "tmsm31_scp", "ttmsm31.SCRAP_MAKER[{0}]"	,(const char*)ttmsm31.SCRAP_MAKER);

		ttmsm31.Update("CURRENT_STATUS,SCRAP_TIME","SM_UNIT_NO,MOLD_NO,MOLD_SECTION,CC_MACH_NO");			

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT * FROM TTMSM31 WHERE 1=1 " ;
				if (ttmsm31.MOLD_NO.Trim() != "")  
				{
					sqlstr = sqlstr + " AND MOLD_NO = @ttmsm31.MOLD_NO ";
				}
				if (ttmsm31.MOLD_SECTION.Trim() != "")  
				{
					sqlstr = sqlstr + " AND MOLD_SECTION = @ttmsm31.MOLD_SECTION ";
				}
				if (ttmsm31.CC_MACH_NO.Trim() != "")  
				{
					sqlstr = sqlstr + " AND CC_MACH_NO = @ttmsm31.CC_MACH_NO ";
				}
				if (ttmsm31.SM_UNIT_NO.Trim() != "") 
				{
					sqlstr = sqlstr + " AND SM_UNIT_NO = @ttmsm31.SM_UNIT_NO ";
				}
				sqlstr = sqlstr +" ORDER BY MOLD_NO ASC ";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("ttmsm31.MOLD_NO"		,ttmsm31.MOLD_NO);
		cmd_inq.Parameters.Set("ttmsm31.MOLD_SECTION"	,ttmsm31.MOLD_SECTION);
		cmd_inq.Parameters.Set("ttmsm31.CC_MACH_NO"		,ttmsm31.CC_MACH_NO);
		cmd_inq.Parameters.Set("ttmsm31.SM_UNIT_NO"		,ttmsm31.SM_UNIT_NO);
		cmd_inq.ExecuteReader();
		if ( cmd_inq.Read() )
		{
			cmd_inq.Fetch(ttmsm31);//把数据都压在头文件里面
		}
		cmd_inq.Close();
	
		htmsm31.CopyFrom(ttmsm31);
		htmsm31.Insert();
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