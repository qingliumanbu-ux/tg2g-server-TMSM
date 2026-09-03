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
BM2F_ENTERACE(tmsm15av_f4)

int f_tmsm15av_f4(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString sqlstr("");

	CModel ttmsm15("TTMSM15");

	CDbCommand cmd_inq(conn);


	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");




	try
	{
	
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm15.Reset();
			ttmsm15.MergeFrom(bcls_rec->Tables[0].Rows[i]);
		
			ttmsm15.Update("ACTUAL_MEASURE_EAST,ACTUAL_MEASURE_WEST,NOTE", "CC_MACH_NO,STRAND_DIV,SEQ_NO,ROLLER_NO_DESC");

		}
		Log::Trace("", __FUNCTION__, "b		= [{0}][{1}]", bcls_rec->Tables[0].Rows[0]["SEQ_NO"].ToString(), bcls_rec->Tables[0].Rows[0]["ROLLER_NO_DESC"].ToString());

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
