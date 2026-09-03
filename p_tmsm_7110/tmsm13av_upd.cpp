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
BM2F_ENTERACE(tmsm13av_upd)

int f_tmsm13av_upd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString v_shift_no("");
	CString v_shift_group("");
	CString sqlstr("");
	CString sqlstr1("");
	CString code_class("");
	CString time("");

	CModel ttmsm13("TTMSM13");


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass tmp;


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++) {
			ttmsm13.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm13["DATE_C"] = ttmsm13["DATE_C"].ToString().SubstringNE(0, 8);
			ttmsm13.Update("*", "DATE_C,SHIFT_NO,LADLE_NO");
		}
		for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++) {
			ttmsm13.MergeFrom(bcls_rec->Tables[1].Rows[i]);
			ttmsm13["DATE_C"] = ttmsm13["DATE_C"].ToString().SubstringNE(0, 8);
			ttmsm13.Update("*", "DATE_C,SHIFT_NO,LADLE_NO");
		}
		for (int i = 0; i < bcls_rec->Tables[2].Rows.get_Count(); i++) {
			ttmsm13.MergeFrom(bcls_rec->Tables[2].Rows[i]);
			ttmsm13["DATE_C"] = ttmsm13["DATE_C"].ToString().SubstringNE(0, 8);
			ttmsm13.Print();
			ttmsm13.Update("*", "DATE_C,SHIFT_NO,LADLE_NO");
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
