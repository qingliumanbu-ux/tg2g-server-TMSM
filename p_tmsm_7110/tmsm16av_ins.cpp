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
BM2F_ENTERACE(tmsm16av_ins)

int f_tmsm16av_ins(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel twmsmzd02("TWMSMZD02");
	EIClass tmp;


	try
	{
		twmsmzd02["CODE_CLASS"] = "TMSLH";
		twmsmzd02["CODE"] = bcls_rec->Tables[0].Rows[0]["STRAND_DIV"].ToString();
		twmsmzd02["CODE_DESC_2_CONTENT"] = bcls_rec->Tables[0].Rows[0]["CC_MACH_NO"].ToString();
		if (twmsmzd02.QueryCount("CODE_CLASS,CODE,CODE_DESC_2_CONTENT") == 0)
		{
			sprintf(s.msg, "该铸机号与流号不匹配！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sqlstr = " SELECT TO_NUMBER(T1.CODE) SEQ_NO,\
			'"+bcls_rec->Tables[0].Rows[0]["CC_MACH_NO"].ToString()+"'                CC_MACH_NO,\
			TO_NUMBER(" + bcls_rec->Tables[0].Rows[0]["STRAND_DIV"].ToString() + ")       STRAND_DIV\
			FROM TWMSMZD02 T1\
			WHERE T1.CODE_CLASS = 'TMSXH'\
			AND T1.CODE_DESC_1_CONTENT LIKE '%" + bcls_rec->Tables[0].Rows[0]["CC_MACH_NO"].ToString() + "%'\
			ORDER BY TO_NUMBER(T1.CODE) ";
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
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
