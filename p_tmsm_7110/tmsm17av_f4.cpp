/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    gongnn
Version:    1.0
Date:     2024-04-01
Description:
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"

/******后台pc文件标准注释标记*****/

/* ***** 静态函数申明 ***** */


/*<remark>=========================================================

===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm17av_f4)

int f_tmsm17av_f4(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString v_date_c("");
	CString sqlstr("");

	CModel ttmsm17("TTMSM17");

	CDbCommand cmd_inq(conn);


	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");




	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm17.Reset();
			ttmsm17.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm17["DATE_C"] = bcls_rec->Tables[0].Rows[i]["DATE_C"].ToString();
			ttmsm17["SEQ_NO"] = bcls_rec->Tables[0].Rows[i]["SEQ_NO"].ToDecimal();
			Log::Trace("", __FUNCTION__, "b		= [{0}][{1}]", bcls_rec->Tables[0].Rows[i]["DATE_C"].ToString(), bcls_rec->Tables[0].Rows[i]["SEQ_NO"].ToDecimal());
			ttmsm17.Delete("DATE_C,SEQ_NO");

		}
		Log::Trace("", __FUNCTION__, "b		= [{0}][{1}]", bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString(), bcls_rec->Tables[0].Rows[0]["SEQ_NO"].ToDecimal());





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
