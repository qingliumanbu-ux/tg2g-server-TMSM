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
BM2F_ENTERACE(tmsm12av_inq1)

int f_tmsm12av_inq1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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

	EIClass tmp;


	try
	{
		sqlstr = " select LADLE_LIFE,\
			NOZZLE_BRICK_LIFE,\
			UP_NOZZLE_LIFE,\
			SLIDE_LIFE,\
			DOWN_NOZZLE_LIFE,\
			BOTTOM_BLOW1_LIFE,\
			BOTTOM_BLOW2_LIFE,\
			LADLE_RETURN_TIME\
			from ttmsm12\
		where LADLE_NO = '"+bcls_rec->Tables[0].Rows[0]["LADLE_NO"].ToString() + "' \
			AND LD_TYPE = '"+bcls_rec->Tables[0].Rows[0]["LD_TYPE"].ToString() + "'\
			and REC_CREATE_TIME = (select max(REC_CREATE_TIME) from ttmsm12 where\
		 LADLE_NO = '" + bcls_rec->Tables[0].Rows[0]["LADLE_NO"].ToString() + "'\
		  AND LD_TYPE='"+bcls_rec->Tables[0].Rows[0]["LD_TYPE"].ToString() + "') order by HEAT_NO desc ";
		Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		if (bcls_ret->Tables[0].Rows.get_Count() > 0)
		{
			bcls_ret->Tables[0].Rows[0]["LADLE_LIFE"] = bcls_ret->Tables[0].Rows[0]["LADLE_LIFE"].ToDecimal() + 1;
			bcls_ret->Tables[0].Rows[0]["NOZZLE_BRICK_LIFE"] = bcls_ret->Tables[0].Rows[0]["NOZZLE_BRICK_LIFE"].ToDecimal() + 1;
			bcls_ret->Tables[0].Rows[0]["UP_NOZZLE_LIFE"] = bcls_ret->Tables[0].Rows[0]["UP_NOZZLE_LIFE"].ToDecimal() + 1;
			bcls_ret->Tables[0].Rows[0]["SLIDE_LIFE"] = bcls_ret->Tables[0].Rows[0]["SLIDE_LIFE"].ToDecimal() + 1;
			bcls_ret->Tables[0].Rows[0]["DOWN_NOZZLE_LIFE"] = bcls_ret->Tables[0].Rows[0]["DOWN_NOZZLE_LIFE"].ToDecimal() + 1;
			bcls_ret->Tables[0].Rows[0]["BOTTOM_BLOW1_LIFE"] = bcls_ret->Tables[0].Rows[0]["BOTTOM_BLOW1_LIFE"].ToDecimal() + 1;
			bcls_ret->Tables[0].Rows[0]["BOTTOM_BLOW2_LIFE"] = bcls_ret->Tables[0].Rows[0]["BOTTOM_BLOW2_LIFE"].ToDecimal() + 1;
			bcls_ret->Tables[0].Columns["LADLE_RETURN_TIME"].set_ColumnName("PRE_HEAT_LADLE_RT_TIME");
		}
		else
		{
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[0]["LADLE_LIFE"] =  1;
			bcls_ret->Tables[0].Rows[0]["NOZZLE_BRICK_LIFE"] =  1;
			bcls_ret->Tables[0].Rows[0]["UP_NOZZLE_LIFE"] =  1;
			bcls_ret->Tables[0].Rows[0]["SLIDE_LIFE"] =  1;
			bcls_ret->Tables[0].Rows[0]["DOWN_NOZZLE_LIFE"] =  1;
			bcls_ret->Tables[0].Rows[0]["BOTTOM_BLOW1_LIFE"] =  1;
			bcls_ret->Tables[0].Rows[0]["BOTTOM_BLOW2_LIFE"] =  1;
			bcls_ret->Tables[0].Columns["LADLE_RETURN_TIME"].set_ColumnName("PRE_HEAT_LADLE_RT_TIME");
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
