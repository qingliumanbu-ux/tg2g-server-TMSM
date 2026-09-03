/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    KE2111
Version:    1.0
Date:     2024-02-16
Description:
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"

/******后台pc文件标准注释标记*****/

/* ***** 静态函数申明 ***** */


/*<remark>=========================================================

===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsmdr_f3)

int f_tmsmdr_f3(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义	

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int doflag = 0;
	int fetchRowCount = 0;
	int i;
	int ret;
	int RowCount = 0;
	int blkNum = 0;
	CDecimal i_count = 0;
	CDecimal i_status = 0;
	CString activity_name("");
	CString v_name("");
	CString v_shift_group("");
	CString sqlstr("");
	CString sqlstr1("");

	CModel ttmsmdr("TTMSMDR");
	CModel ttmsmpz01("TTMSMPZ01");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass tmp;


	try
	{
		
		v_name = bcls_rec->Tables[0].Rows[0]["NAME"].ToString().SubstringNE(3);
		sqlstr = " select ACTIVITY_NAME from  TTMSMPZ b  where b.SHOW_SEQ='" + v_name + "' ";
		Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			activity_name = cmd_inq.GetString(1);
		}
		
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "activity_name		= [{0}]", activity_name);
		for (int i=0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsmdr.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsmdr["ACTIVITY_NAME"] = activity_name;
			ttmsmdr["LOG_TIME"]= CDateTime::Now().ToString("yyyyMMddHHmmssmsff");
			ttmsmdr["REC_CREATOR"] = s.userid;
			ttmsmdr["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			ttmsmdr.Insert();
		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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



	return doFlag;

}
