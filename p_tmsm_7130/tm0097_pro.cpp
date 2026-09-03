/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    KE2111
Version:    1.0
Date:     2023-11-08
Description: 工器具事件跟踪处理
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"


// service入口
BM2F_ENTERACE(tm0097_pro)

int f_tm0097_pro(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义	
	int doFlag = 0;
	/* 实体类定义 */
	CModel ttm0097 = CModel("TTM0097");
	CModel ttm0097_old = CModel("TTM0097");
	CModel ttm0099 = CModel("TTM0099");
	CModel ttm0099_old = CModel("TTM0099");

	/* 数据库SQL操作字符串 */
	CString sqlstr("");

	/* 数据库操作类定义 */

	CDbCommand cmd_inq(conn);

	

	try
	{

		//新增事件
		if (bcls_rec->Tables.IndexOf("ADD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				ttm0097.Reset();
				ttm0097.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
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

			}

		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("UPD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				ttm0097.Reset();

				
				ttm0097_old.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				ttm0097.TrimOrBlank();

				ttm0097_old.Query("STATION_ID,EVENT_ID");
				ttm0097.CopyFrom(ttm0097_old);
				ttm0097.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				/* 修改事件信息 */
				ttm0097["REC_REVISOR"] = s.userid;
				ttm0097["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				ttm0097.TrimOrBlank();

				ttm0097_old.Delete("STATION_ID,EVENT_ID");
				ttm0097.Insert();

			}
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("DEL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				ttm0097.Reset();
				ttm0097.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);
				ttm0097.TrimOrBlank();


				/* 删除事件信息 */
				ttm0097.Delete("STATION_ID,EVENT_ID");


			}
		}
		//新增字段
		if (bcls_rec->Tables.IndexOf("ADD_DETAIL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["ADD_DETAIL"].Rows.get_Count(); i++)
			{
				ttm0099.Reset();
				ttm0099.MergeFrom(bcls_rec->Tables["ADD_DETAIL"].Rows[i]);
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
				Log::Trace("", __FUNCTION__, "sqlstr[{0}]sqlstr[{1}]", ttm0099["ITEM_ENAME"].ToString(), ttm0099["ITEM_LEN"].ToString());
				if (!ttm0099.Insert()) {
					sprintf(s.msg, "新增失败。");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

			}

		}

		// 修改字段
		if (bcls_rec->Tables.IndexOf("UPD_DETAIL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["UPD_DETAIL"].Rows.get_Count(); i++)
			{
				ttm0099.Reset();

			
				ttm0099_old.MergeFrom(bcls_rec->Tables["UPD_DETAIL"].Rows[i]);
				ttm0099.TrimOrBlank();

				ttm0099_old.Query("STATION_ID,EVENT_ID,ITEM_ENAME");
				ttm0099.CopyFrom(ttm0097_old);
				ttm0099.MergeFrom(bcls_rec->Tables["UPD_DETAIL"].Rows[i]);
				/* 修改事件信息 */
				ttm0099["REC_REVISOR"] = s.userid;
				ttm0099["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				ttm0099.TrimOrBlank();

				ttm0099_old.Delete("STATION_ID,EVENT_ID,ITEM_ENAME");
				ttm0099.Insert();

			}
		}

		// 删除字段
		if (bcls_rec->Tables.IndexOf("DEL_DETAIL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["DEL_DETAIL"].Rows.get_Count(); i++)
			{
				ttm0099.Reset();
				ttm0099.MergeFrom(bcls_rec->Tables["DEL_DETAIL"].Rows[i]);
				ttm0099.TrimOrBlank();


				/* 删除事件信息 */
				ttm0099.Delete("STATION_ID,EVENT_ID,ITEM_ENAME");


			}
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

	cmd_inq.Close();

	return doFlag;

}
