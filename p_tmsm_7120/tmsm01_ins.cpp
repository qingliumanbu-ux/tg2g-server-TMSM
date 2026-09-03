/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sjh
Version:    1.0
Date:     2013-11-14
Description: 钢包基本信息_新增
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
#include "ttmsm01.h"
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
//   钢包基本信息_新增
/// <para>
/// 1.根据输入参数以炼钢单元号为基础新增《钢包基本信息表》中的相应信息。
/// <para>数据库表： ttmsm01	钢包基本信息表 </para>
/// </summary>
/// <param name="SM_UNIT_NO">炼钢单元号               </param>
/// <returns>指定炼钢单元号下的新增命令信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm01_ins)

int f_tmsm01_ins(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义	
	
	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int i;
	int ret;
	int RowCount = 0;
	CDecimal i_count = 0;

	CDbCommand cmd(conn);	
	CDbCommand cmd_inq(conn);

	CString  sqlstr = "";

	CTTMSM01 ttmsm01(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		/* 对输入信息循环处理 */
		RowCount = bcls_rec->Tables[0].Rows.get_Count();
		for (i = 0; i < RowCount; i++ ) 
		{ 
			/* 取得单行传入信息 */
			ttmsm01.Reset();
			ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace(" ", "tmsm01_ins", "ttmsm01.SM_UNIT_NO[{0}]"		,(const char*)ttmsm01.SM_UNIT_NO);
			Log::Trace(" ", "tmsm01_ins", "ttmsm01.LADLE_NO[{0}]"	,(const char*)ttmsm01.LADLE_NO);

			if(ttmsm01.START_USE_DATE.Trim() != "")
			{
				ttmsm01.START_USE_DATE = ttmsm01.START_USE_DATE.Substring(0,8);
			}
			if(ttmsm01.BIG_REPAIR_DATE.Trim() != "")
			{
				ttmsm01.BIG_REPAIR_DATE = ttmsm01.BIG_REPAIR_DATE.Substring(0,8);
			}
			if(ttmsm01.MIDDLE_REPAIR_DATE.Trim() != "")
			{
				ttmsm01.MIDDLE_REPAIR_DATE = ttmsm01.MIDDLE_REPAIR_DATE.Substring(0,8);
			}
			if(ttmsm01.SMALL_REPAIR_DATE.Trim() != "")
			{
				ttmsm01.SMALL_REPAIR_DATE = ttmsm01.SMALL_REPAIR_DATE.Substring(0,8);
			}
			if(ttmsm01.FOREVER_REPAIR_DATE.Trim() != "")
			{
				ttmsm01.FOREVER_REPAIR_DATE = ttmsm01.FOREVER_REPAIR_DATE.Substring(0,8);
			}
			Log::Trace(" ", "tmsm01_ins", "ttmsm01.START_USE_DATE[{0}]"		,(const char*)ttmsm01.START_USE_DATE);
			Log::Trace(" ", "tmsm01_ins", "ttmsm01.BIG_REPAIR_DATE[{0}]"	,(const char*)ttmsm01.BIG_REPAIR_DATE);
			Log::Trace(" ", "tmsm01_ins", "ttmsm01.MIDDLE_REPAIR_DATE[{0}]"	,(const char*)ttmsm01.MIDDLE_REPAIR_DATE);
			Log::Trace(" ", "tmsm01_ins", "ttmsm01.SMALL_REPAIR_DATE[{0}]"	,(const char*)ttmsm01.SMALL_REPAIR_DATE);
			Log::Trace(" ", "tmsm01_ins", "ttmsm01.FOREVER_REPAIR_DATE[{0}]",(const char*)ttmsm01.FOREVER_REPAIR_DATE);

			/*判断主键钢包是否已经存在*/
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT COUNT(1) "
							 " FROM TTMSM01 "
							 " WHERE LADLE_NO = @ttmsm01.LADLE_NO "
							 " AND SM_UNIT_NO = @ttmsm01.SM_UNIT_NO";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("ttmsm01.LADLE_NO"	,ttmsm01.LADLE_NO);
			cmd_inq.Parameters.Set("ttmsm01.SM_UNIT_NO"	,ttmsm01.SM_UNIT_NO);
			i_count = cmd_inq.ExecuteScalar();
			cmd_inq.Close();

			if(i_count > 0)
			{
				sprintf(s.msg, "钢包号[%s]已存在，不可重复新增。",(const char*)ttmsm01.LADLE_NO);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm01.REC_CREATE_TIME = datetimeNow;
			ttmsm01.REC_CREATOR = s.userid;
			ttmsm01.Insert();
		}
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
