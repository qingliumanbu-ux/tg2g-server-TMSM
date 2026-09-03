/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2111
Version:     1.0
Date:        2023-11-03 
Description: 炼钢工器具调用工器具事件函数
**************************************************/

#include "stdafx.h"

//公共工器具事件处理函数
int f_tm0097_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

BM2_FUNCTION_EXPORT


//记录履历

int f_tmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString userid = s.userid;
	/**事件调用类型 1-新增 2-修改 3-删除*/
	CString c_event_call_type("");
	/**事件号*/
	CString c_event_id("");
	/**设备类型*/
	CString c_station_id("");



	/* 实体类定义 */
	CModel ttm0097("TTM0097");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDataTable dt_temp;

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获得传入参数 */
		if (!bcls_rec->Tables.IndexOf("TM0096")) {
			sprintf(s.msg, "未获取到[TM0096]表！");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (!bcls_rec->Tables["TM0096"].Columns.Contains("EVENT_ID") ||
			bcls_rec->Tables["TM0096"].Rows[0]["EVENT_ID"].ToString().Trim() == "") {
			sprintf(s.msg, "未检测到事件号传入，请联系系统人员！");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (!bcls_rec->Tables["TM0096"].Columns.Contains("STATION_ID") ||
			bcls_rec->Tables["TM0096"].Rows[0]["STATION_ID"].ToString().Trim() == "") {
			sprintf(s.msg, "未检测到设备传入，请联系系统人员！");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		doFlag = f_tm0097_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
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


