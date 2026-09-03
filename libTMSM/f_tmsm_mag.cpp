/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2111
Version:     1.0
Date:        2023-11-03
Description: 
**************************************************/

#include "stdafx.h"


BM2_FUNCTION_EXPORT


//记录履历

int f_tmsm_mag(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */

	CString v_heat_no("");
	CString v_msg("");
	CString v_state_time("");


	/* 实体类定义 */
	CModel ttmsm12("TTMSM12");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDataTable dt_temp;

	try
	{
		v_heat_no = bcls_rec->Tables["TMSM_MSG"].Rows[0]["HEAT_NO"].ToString();//熔炼号
		v_msg = bcls_rec->Tables["TMSM_MSG"].Rows[0]["MSG"].ToString();//信号
		v_state_time = bcls_rec->Tables["TMSM_MSG"].Rows[0]["STATE_TIME"].ToString();//状态时间
		Log::Trace("", __FUNCTION__, "v_heat_no=[{0}] v_msg=[{1}]{2}", v_heat_no, v_msg, v_state_time);
		if (v_msg.SubstringNE(0, 1) == "3" )
		{
			ttmsm12["HEAT_NO"] = v_heat_no;
			ttmsm12["OUT_STEEL_TIME"] = v_state_time;
			ttmsm12.Update("OUT_STEEL_TIME", "HEAT_NO");
		}
		if (v_msg.SubstringNE(0, 1) == "5")
		{
			ttmsm12["HEAT_NO"] = v_heat_no;
			ttmsm12["CAST_END_TIME"] = v_state_time;
			ttmsm12.Update("CAST_END_TIME", "HEAT_NO");
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace((1,1, "[%s]", s.sysmsg);
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

	cmd_inq.Close();
	return doFlag;
}


