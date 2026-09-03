/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2111
Version:     1.0
Date:        2023-11-08
Description: 工器具配置项刷新
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tm0097_copy)


int f_tm0097_copy(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString s_userid("");


	/* 实体类定义 */


	/* 数据库SQL操作字符串 */
	CString sqlstr("");

	/* 数据库操作类定义 */
	CModel ttm0097 = CModel("TTM0097");
	CModel ttm0099 = CModel("TTM0099");
	CModel ttm0099_old = CModel("TTM0099");
	CDbCommand cmd_inq(conn);



	try
	{
		ttm0097.Reset();
		ttm0097.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		ttm0097.TrimOrBlank();

		/* 新增事件信息 */
		ttm0097["REC_CREATOR"] = s.userid;   //记录创建责任者
		ttm0097["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");   //记录创建时刻
		ttm0097.TrimOrBlank();
		if (ttm0097.QueryCount("STATION_ID,EVENT_ID") > 0)
		{
			sprintf(s.msg, "该设备[%s]该事件[%s]已存在，不可重复新增。", (const char*)ttm0097["STATION_ID"], (const char*)ttm0097["EVENT_ID"]);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		ttm0097.Insert();
		sqlstr = " select * from ttm0099 where STATION_ID='" + ttm0097["STATION_ID"].ToString() + "' and EVENT_ID='" + ttm0097["EVENT_ID"].ToString() + "' ";
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			ttm0099.Reset();
			ttm0099.MergeFrom(bcls_ret->Tables[0].Rows[i]);
			ttm0099.TrimOrBlank();
			/* 新增事件信息 */
			ttm0099["REC_CREATOR"] = s.userid;   //记录创建责任者
			ttm0099["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");   //记录创建时刻

			ttm0099.TrimOrBlank();
			if (ttm0099.QueryCount("STATION_ID,EVENT_ID,ITEM_ENAME") > 0)
			{
				sprintf(s.msg, "该设备[%s]事件[%s]字段[%s]已存在，不可重复新增。", (const char*)ttm0099["STATION_ID"], (const char*)ttm0099["EVENT_ID"], (const char*)ttm0099["ITEM_ENAME"]);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			
			if (!ttm0099.Insert()) {
				sprintf(s.msg, "新增失败。");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

		}

		




	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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
	cmd_inq.Close();

	return doFlag;
}


