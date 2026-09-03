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
BM2F_ENTERACE(tmsmpz_f3)

int f_tmsmpz_f3(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
				ttmsmpz.Reset();
				ttmsmpz.MergeFrom(bcls_rec->Tables["TMSM_INS"].Rows[i]);
				ttmsmpz.TrimOrBlank();

				if (ttmsmpz["ACTIVITY_NAME"].ToString().Trim() == "")
				{
					sprintf(s.msg, "分页名不可为空。");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				//ttmsmpz.Print();


				/* 新增事件信息 */
				ttmsmpz["REC_CREATOR"] = s.userid;   //记录创建责任者
				ttmsmpz["REC_CREATE_TIME"] = datetimeNow;   //记录创建时刻

				ttmsmpz.TrimOrBlank();



				if (ttmsmpz.QueryCount("ACTIVITY_NAME") > 0)
				{
					sprintf(s.msg, "分页名[%s]已存在，不可重复新增。", (const char*)ttmsmpz["ACTIVITY_NAME"]);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				ttmsmpz.Insert();

			}

		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("TMSM_UPD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["TMSM_UPD"].Rows.get_Count(); i++)
			{
				ttmsmpz.Reset();

				ttmsmpz.MergeFrom(bcls_rec->Tables["TMSM_UPD"].Rows[i]);
				ttmsmpz.TrimOrBlank();


				/* 修改事件信息 */
				ttmsmpz["REC_REVISOR"] = s.userid;
				ttmsmpz["REC_REVISE_TIME"] = datetimeNow;
				ttmsmpz.TrimOrBlank();

				ttmsmpz.Delete("ACTIVITY_NAME");
				ttmsmpz.Insert();

			}
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("TMSM_DEL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["TMSM_DEL"].Rows.get_Count(); i++)
			{
				ttmsmpz.Reset();
				ttmsmpz.MergeFrom(bcls_rec->Tables["TMSM_DEL"].Rows[i]);
				ttmsmpz.TrimOrBlank();


				/* 删除事件信息 */
				ttmsmpz.Delete("ACTIVITY_NAME");


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
