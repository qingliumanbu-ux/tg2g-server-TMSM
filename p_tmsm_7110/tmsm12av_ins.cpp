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
BM2F_ENTERACE(tmsm12av_ins)

int f_tmsm12av_ins(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
		//计算班次班组
		f_epep_get_shift_group("SMDD", datetimeNow, v_shift_no, v_shift_group, conn);
				
		sqlstr=" select SUBSTR(HEAT_NO,0,1) NEW_HEAT_NO1,lpad(substr(A.HEAT_NO, 2) + 1,7,'0') NEW_HEAT_NO2,A.*\
			from ttmsm12 A\
		where FURNACE_NO = '"+bcls_rec->Tables[0].Rows[0]["FURNACE_NO"].ToString() + "' and C_DIV='" + bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString() + "'\
			and REC_CREATE_TIME = (select max(REC_CREATE_TIME)\
				from ttmsm12\
		where FURNACE_NO = '"+ bcls_rec->Tables[0].Rows[0]["FURNACE_NO"].ToString() +"' and C_DIV='" + bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString() + "') order by HEAT_NO desc ";
		Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(tmp.Tables[0]);
		cmd_inq.Close();
		//Log::Trace("", __FUNCTION__, "DRTYGBHJKL		= [{0}]", tmp.Tables[0].Rows[0]["HEAT_NO"].ToString() + "1", tmp.Tables[0].Rows[0]["HEAT_NO"].ToDecimal(), tmp.Tables[0].Rows[0]["HEAT_NO"].ToString());
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SHIFT_GROUP");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REPAIR_SHIFT_NO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "DATE_C");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "LD_TYPE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_NO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SLIDE_NOZZLE_CHECK_OP");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "BOTTOM_BLOW_CHECK_OP");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ADJUST_LADLE_PERSON");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "PRE_HEAT_CAST_E_TIME");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "FURNACE_NO");

		if (bcls_ret->Tables[0].Rows.get_Count() == 0)
			bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0]["FURNACE_NO"] = bcls_rec->Tables[0].Rows[0]["FURNACE_NO"].ToString();
		bcls_ret->Tables[0].Rows[0]["SHIFT_GROUP"] = v_shift_group;
		bcls_ret->Tables[0].Rows[0]["REPAIR_SHIFT_NO"] = v_shift_no;
		bcls_ret->Tables[0].Rows[0]["DATE_C"] = datetimeNow.SubstringNE(0,8);
		if (bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString() == "A")//不锈
		{
			bcls_ret->Tables[0].Rows[0]["LD_TYPE"] = "TS";
		}
		else
		{
			bcls_ret->Tables[0].Rows[0]["LD_TYPE"] = "TC";
		}
		if (tmp.Tables[0].Rows.get_Count()>0)
		{
			bcls_ret->Tables[0].Rows[0]["HEAT_NO"] = tmp.Tables[0].Rows[0]["NEW_HEAT_NO1"].ToString() + tmp.Tables[0].Rows[0]["NEW_HEAT_NO2"].ToString();
			bcls_ret->Tables[0].Rows[0]["ST_NO"] = tmp.Tables[0].Rows[0]["ST_NO"];
			bcls_ret->Tables[0].Rows[0]["SLIDE_NOZZLE_CHECK_OP"] = tmp.Tables[0].Rows[0]["SLIDE_NOZZLE_CHECK_OP"];
			bcls_ret->Tables[0].Rows[0]["BOTTOM_BLOW_CHECK_OP"] = tmp.Tables[0].Rows[0]["BOTTOM_BLOW_CHECK_OP"];
			bcls_ret->Tables[0].Rows[0]["ADJUST_LADLE_PERSON"] = tmp.Tables[0].Rows[0]["ADJUST_LADLE_PERSON"];
			bcls_ret->Tables[0].Rows[0]["PRE_HEAT_CAST_E_TIME"] = tmp.Tables[0].Rows[0]["PRE_HEAT_CAST_E_TIME"];
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
