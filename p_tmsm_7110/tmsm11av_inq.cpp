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
BM2F_ENTERACE(tmsm11av_inq)

int f_tmsm11av_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString sql("");
	CString sqlstr("");
	CString c_date_c_from = " ";
	CString c_date_c_to = " ";
	CString c_out_steel_time_from = " ";
	CString c_out_steel_time_to = " ";
	CString c_cast_end_time_from = " ";
	CString c_cast_end_time_to = " ";
	CString c_c_div = " ";
	CString get_columnname = " ";
	int i_flag = 0;

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass tmp;


	try
	{
		CString v_tab = bcls_rec->Tables[0].Rows[0]["TAB"].ToString();
		Log::Trace("", "", "", "v_tab", v_tab);
		if (v_tab=="TAB1")
		{
			sql = " WITH TM11 AS (SELECT * \
				FROM(SELECT row_number() over(partition by LADLE_NO ORDER BY RESUME_SEQ_NO desc) ROW_ID, A.* \
					FROM(SELECT * FROM TTMSM11) A) \
				WHERE ROW_ID = 1) \
				SELECT CODE_DESC_1_CONTENT LADLE_NO, TM11.WORK_MAKER, TM11.ABN_REASON,  tm11.DRYING_ST,TM11.DRYING_ET, TM11.BAKE_TYPE, TM11.BAKE_POS, TM11.UPLOAD_TIME, TM11.UPLOADER, TM11.MEMO_DETAIL, TM11.RESUME_SEQ_NO, TM11.LADLE_STATUS, \
				 case when TM11.LADLE_HEATING_DURATION = 0 and tm11.DRYING_ST != ' ' then CEIL((sysdate - TO_DATE(tm11.DRYING_ST, 'yyyy-mm-dd hh24-mi-ss')) * 24 ) else TM11.LADLE_HEATING_DURATION end LADLE_HEATING_DURATION,\
				 (select * from (select LADLE_LIFE from ttmsm12 where LADLE_NO = substr2(TM11.LADLE_NO, 3, 2) and REC_CREATE_TIME > TM11.UPLOAD_TIME and C_DIV = 'A' order by TTMSM12.REC_CREATE_TIME desc) where ROWNUM = 1)  LADLE_LIFE \
				FROM TWMSMZD02 T1 \
				LEFT JOIN TM11 ON T1.CODE_DESC_1_CONTENT = TM11.LADLE_NO \
				WHERE CODE_CLASS = 'TM01' \
				ORDER BY CODE_DESC_1_CONTENT ";
			Log::Trace("", "", "", "SQL", sql);
			cmd_inq.SetCommandText(sql);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		if (v_tab == "TAB2")
		{
			sql = " select * from ttmsm11 where 1=1  ";
			if (bcls_rec->Tables[0].Rows[0]["LADLE_NO"].ToString().Trim()!="")
			{
				sql += " AND LADLE_NO='" + bcls_rec->Tables[0].Rows[0]["LADLE_NO"].ToString() + "' ";
			}
			sql += " order by RESUME_SEQ_NO desc ";
			Log::Trace("", "", "", "SQL", sql);
			cmd_inq.SetCommandText(sql);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
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
