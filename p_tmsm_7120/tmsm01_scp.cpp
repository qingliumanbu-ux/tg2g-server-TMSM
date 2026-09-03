/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sjh
Version:    1.0
Date:     2013-11-14
Description: 钢包基本信息管理_报废
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
#include "ttmsm01.h"
#include "htmsm01.h"
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
//   钢包基本信息__报废
/// <para>
/// 1.根据输入参数以炼钢单元号为基础报废《钢包基本信息表》中的相应信息。
/// <para>数据库表： ttmsm01	钢包基本信息表 </para>
/// <para>数据库表： htmsm01	钢包基本信息历史表 </para>
/// </summary>
/// <param name="SM_UNIT_NO">炼钢单元号               </param>
/// <returns>指定炼钢单元号下的报废命令信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm01_scp)

int f_tmsm01_scp(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int i;
	int RowCount = 0;

	CString sqlstr = "";

	CTTMSM01 ttmsm01(conn);
	CHTMSM01 htmsm01(conn);

	CDbCommand cmd(conn);	
	CDbCommand cmd_inq(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		/* ***** 程序处理 ***** */	
		Log::Trace(" ", "datetimeNow", "datetimeNow[{0}]"		,(const char*)datetimeNow);
		ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		ttmsm01.LADLE_STATUS = "99";
		ttmsm01.SCRAP_TIME = datetimeNow;
		ttmsm01.SCRAP_MAKER = s.userid;

		Log::Trace(" ", "tmsm01_scp", "ttmsm01.SM_UNIT_NO[{0}]"		,(const char*)ttmsm01.SM_UNIT_NO);
		Log::Trace(" ", "tmsm01_scp", "ttmsm01.LADLE_NO[{0}]"	,(const char*)ttmsm01.LADLE_NO);
		Log::Trace(" ", "tmsm01_scp", "ttmsm01.SCRAP_TIME[{0}]"		,(const char*)ttmsm01.SCRAP_TIME);
		Log::Trace(" ", "tmsm01_scp", "ttmsm01.SCRAP_MAKER[{0}]"	,(const char*)ttmsm01.SCRAP_MAKER);
		Log::Trace(" ", "tmsm01_scp", "ttmsm01.LADLE_STATUS[{0}]"	,(const char*)ttmsm01.LADLE_STATUS);

		ttmsm01.Update("LADLE_STATUS,SCRAP_TIME","SM_UNIT_NO,LADLE_NO");	
		Log::Trace(" ", "datetimeNow 222", "datetimeNow 222[{0}]"		,(const char*)datetimeNow);

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT * FROM TTMSM01 WHERE 1=1 " ;
				if (ttmsm01.LADLE_NO.Trim()!= "")  
				{
					sqlstr = sqlstr + " AND LADLE_NO = @ttmsm01.LADLE_NO ";
				}
				if (ttmsm01.SM_UNIT_NO.Trim()!= "") 
				{
					sqlstr = sqlstr + " AND SM_UNIT_NO  = @ttmsm01.SM_UNIT_NO ";
				}
				sqlstr = sqlstr +" ORDER BY LADLE_NO ASC ";
				//break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("ttmsm01.LADLE_NO"	,ttmsm01.LADLE_NO);
		cmd_inq.Parameters.Set("ttmsm01.SM_UNIT_NO"	,ttmsm01.SM_UNIT_NO);
		cmd_inq.ExecuteReader();
		if ( cmd_inq.Read() )
		{
			cmd_inq.Fetch(ttmsm01);//把数据都压在头文件里面
		}
		cmd_inq.Close();
	Log::Trace(" ", "datetimeNow 333", "datetimeNow 333[{0}]"		,(const char*)datetimeNow);
		htmsm01.CopyFrom(ttmsm01);
		Log::Trace(" ", "datetimeNow 555", "datetimeNow 555[{0}]"		,(const char*)datetimeNow);
		htmsm01.Insert();
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