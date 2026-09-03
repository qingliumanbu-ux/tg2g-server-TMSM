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
BM2F_ENTERACE(tmsmpz_f4)

int f_tmsmpz_f4(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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

	CModel ttmsmpz("TTMSMPZ");
	CModel ttmsmpz01("TTMSMPZ01");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass tmp;


	try
	{
		if (bcls_rec->Tables.IndexOf("TMSM_INS") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["TMSM_INS"].Rows.get_Count(); i++)
			{
				ttmsmpz01["ACTIVITY_NAME"] = bcls_rec->Tables["TMSM_INS"].Rows[i]["ACTIVITY_NAME"];
				ttmsmpz01["ITEM_KIND"] = "S";
				if (ttmsmpz01.QueryCount("ACTIVITY_NAME,ITEM_KIND")>=100)
				{
					sprintf(s.msg, "字符型字段最大100个，如需增加，请联系运维人员。");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				ttmsmpz01["ITEM_KIND"] = "N";
				if (ttmsmpz01.QueryCount("ACTIVITY_NAME,ITEM_KIND") >= 100)
				{
					sprintf(s.msg, "数值型字段最大100个，如需增加，请联系运维人员。");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				ttmsmpz01.Reset();
				ttmsmpz01.MergeFrom(bcls_rec->Tables["TMSM_INS"].Rows[i]);
				ttmsmpz01.TrimOrBlank();

				//ttmsmpz01.Print();
				if (ttmsmpz01["CODE_SEQ"].ToString().Trim() == "")
				{
					sprintf(s.msg, "字段顺序不可为空。");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}


				/* 新增事件信息 */
				ttmsmpz01["REC_CREATOR"] = s.userid;   //记录创建责任者
				ttmsmpz01["REC_CREATE_TIME"] = datetimeNow;   //记录创建时刻
				ttmsmpz01.TrimOrBlank();

				

				if (ttmsmpz01.QueryCount("ACTIVITY_NAME,CODE_SEQ") > 0)
				{
					sprintf(s.msg, "字段顺序[%s]已存在，不可重复新增。", (const char*)ttmsmpz01["CODE_SEQ"]);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				ttmsmpz01.Insert();

			}

		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("TMSM_UPD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["TMSM_UPD"].Rows.get_Count(); i++)
			{
				ttmsmpz01.Reset();

				ttmsmpz01.MergeFrom(bcls_rec->Tables["TMSM_UPD"].Rows[i]);
				ttmsmpz01.TrimOrBlank();


				/* 修改事件信息 */
				ttmsmpz01["REC_REVISOR"] = s.userid;
				ttmsmpz01["REC_REVISE_TIME"] = datetimeNow;
				ttmsmpz01.TrimOrBlank();

				ttmsmpz01.Delete("ACTIVITY_NAME,CODE_SEQ");
				ttmsmpz01.Insert();
				ttmsmpz01["ACTIVITY_NAME"] = bcls_rec->Tables["TMSM_INS"].Rows[i]["ACTIVITY_NAME"];
				ttmsmpz01["ITEM_KIND"] = "S";
				if (ttmsmpz01.QueryCount("ACTIVITY_NAME,ITEM_KIND") > 100)
				{
					sprintf(s.msg, "字符型字段最大100个，如需增加，请联系运维人员。");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				ttmsmpz01["ITEM_KIND"] = "N";
				if (ttmsmpz01.QueryCount("ACTIVITY_NAME,ITEM_KIND") > 100)
				{
					sprintf(s.msg, "数值型字段最大100个，如需增加，请联系运维人员。");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

			}
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("TMSM_DEL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["TMSM_DEL"].Rows.get_Count(); i++)
			{
				ttmsmpz01.Reset();
				ttmsmpz01.MergeFrom(bcls_rec->Tables["TMSM_DEL"].Rows[i]);
				ttmsmpz01.TrimOrBlank();


				/* 删除事件信息 */
				ttmsmpz01.Delete("ACTIVITY_NAME,CODE_SEQ");


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



	return doFlag;

}
