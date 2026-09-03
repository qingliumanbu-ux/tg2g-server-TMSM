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
BM2F_ENTERACE(tmsm121av_upd)

int f_tmsm121av_upd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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

	CModel ttmsm12("TTMSM12");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass tmp;


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)//0#
		{
			if (bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "熔炼号不能为空！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm12.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			ttmsm12["DATE_C"] = ttmsm12["DATE_C"].ToString().SubstringNE(0, 8);
			if (ttmsm12.QueryCount("HEAT_NO") == 0)
			{
				sprintf(s.msg, "熔炼号[%s]不存在，请刷新重试！",(const char*)ttmsm12["HEAT_NO"]);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			CString upd_name = "";
			for (int i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
			{
				if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_STRING
					&& bcls_rec->Tables[0].Rows[0][i].ToString().Trim().IsEmpty())
				{
					continue;
				}

				if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_DECIMAL
					&& bcls_rec->Tables[0].Rows[0][i].ToDecimal() == 0)
				{
					continue;
				}

				if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_DATETIME
					&& bcls_rec->Tables[0].Rows[0][i].ToString().Trim().IsEmpty())
				{
					continue;
				}

				if (bcls_rec->Tables[0].Columns[i].get_ColumnName() == "REC_CREATOR"
					|| bcls_rec->Tables[0].Columns[i].get_ColumnName() == "REC_CREATE_TIME"
					|| bcls_rec->Tables[0].Columns[i].get_ColumnName() == "HEAT_NO")
				{
					continue;
				}
				upd_name += bcls_rec->Tables[0].Columns[i].get_ColumnName() + ",";
			}
			if (ttmsm12["LADLE_NO"].ToString().Trim() != "")
			{
				ttmsm12["BACK_C1"] = Db::QueryCString(" SELECT WORK_MAKER FROM TTMSM11 WHERE LADLE_NO='TS" + ttmsm12["LADLE_NO"].ToString() + "' AND RESUME_SEQ_NO =(SELECT MAX(RESUME_SEQ_NO) FROM TTMSM11 WHERE LADLE_NO='TS" + ttmsm12["LADLE_NO"].ToString() + "') ");
				upd_name += "BACK_C1,";
			}
			if (upd_name.GetLength() > 0)
			{
				upd_name = upd_name.SubstringNE(0, upd_name.GetLength() - 1);
			}
			ttmsm12["REC_REVISE_TIME"] = datetimeNow;
			ttmsm12["REC_REVISOR"] = s.userid;;
			ttmsm12.Update(upd_name, "HEAT_NO");
		}
		for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)//1#
		{
			if (bcls_rec->Tables[1].Rows[0]["HEAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "熔炼号不能为空！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm12.MergeFrom(bcls_rec->Tables[1].Rows[0]);
			ttmsm12["DATE_C"] = ttmsm12["DATE_C"].ToString().SubstringNE(0, 8);
			if (ttmsm12.QueryCount("HEAT_NO") == 0)
			{
				sprintf(s.msg, "熔炼号[%s]不存在，请刷新重试！", (const char*)ttmsm12["HEAT_NO"]);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			CString upd_name = "";
			for (int i = 0; i < bcls_rec->Tables[1].Columns.get_Count(); i++)
			{
				if (bcls_rec->Tables[1].Columns[i].get_DataType() == DT_STRING
					&& bcls_rec->Tables[1].Rows[0][i].ToString().Trim().IsEmpty())
				{
					continue;
				}

				if (bcls_rec->Tables[1].Columns[i].get_DataType() == DT_DECIMAL
					&& bcls_rec->Tables[1].Rows[0][i].ToDecimal() == 0)
				{
					continue;
				}

				if (bcls_rec->Tables[1].Columns[i].get_DataType() == DT_DATETIME
					&& bcls_rec->Tables[1].Rows[0][i].ToString().Trim().IsEmpty())
				{
					continue;
				}

				if (bcls_rec->Tables[1].Columns[i].get_ColumnName() == "REC_CREATOR"
					|| bcls_rec->Tables[1].Columns[i].get_ColumnName() == "REC_CREATE_TIME"
					|| bcls_rec->Tables[1].Columns[i].get_ColumnName() == "HEAT_NO")
				{
					continue;
				}
				upd_name += bcls_rec->Tables[1].Columns[i].get_ColumnName() + ",";
			}
			if (ttmsm12["LADLE_NO"].ToString().Trim() != "")
			{
				ttmsm12["BACK_C1"] = Db::QueryCString(" SELECT WORK_MAKER FROM TTMSM11 WHERE LADLE_NO='TS" + ttmsm12["LADLE_NO"].ToString() + "' AND RESUME_SEQ_NO =(SELECT MAX(RESUME_SEQ_NO) FROM TTMSM11 WHERE LADLE_NO='TS" + ttmsm12["LADLE_NO"].ToString() + "') ");
				upd_name += "BACK_C1,";
			}
			if (upd_name.GetLength() > 0)
			{
				upd_name = upd_name.SubstringNE(0, upd_name.GetLength() - 1);
			}
			ttmsm12["REC_REVISE_TIME"] = datetimeNow;
			ttmsm12["REC_REVISOR"] = s.userid;;
			ttmsm12.Update(upd_name, "HEAT_NO");
		}
		for (int i = 0; i < bcls_rec->Tables[2].Rows.get_Count(); i++)//2#
		{
			if (bcls_rec->Tables[2].Rows[0]["HEAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "熔炼号不能为空！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm12.MergeFrom(bcls_rec->Tables[2].Rows[0]);
			ttmsm12["DATE_C"] = ttmsm12["DATE_C"].ToString().SubstringNE(0, 8);
			if (ttmsm12.QueryCount("HEAT_NO") == 0)
			{
				sprintf(s.msg, "熔炼号[%s]不存在，请刷新重试！", (const char*)ttmsm12["HEAT_NO"]);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			CString upd_name = "";
			for (int i = 0; i < bcls_rec->Tables[2].Columns.get_Count(); i++)
			{
				if (bcls_rec->Tables[2].Columns[i].get_DataType() == DT_STRING
					&& bcls_rec->Tables[2].Rows[0][i].ToString().Trim().IsEmpty())
				{
					continue;
				}

				if (bcls_rec->Tables[2].Columns[i].get_DataType() == DT_DECIMAL
					&& bcls_rec->Tables[2].Rows[0][i].ToDecimal() == 0)
				{
					continue;
				}

				if (bcls_rec->Tables[2].Columns[i].get_DataType() == DT_DATETIME
					&& bcls_rec->Tables[2].Rows[0][i].ToString().Trim().IsEmpty())
				{
					continue;
				}

				if (bcls_rec->Tables[2].Columns[i].get_ColumnName() == "REC_CREATOR"
					|| bcls_rec->Tables[2].Columns[i].get_ColumnName() == "REC_CREATE_TIME"
					|| bcls_rec->Tables[2].Columns[i].get_ColumnName() == "HEAT_NO")
				{
					continue;
				}
				upd_name += bcls_rec->Tables[2].Columns[i].get_ColumnName() + ",";
			}
			if (upd_name.GetLength() > 0)
			{
				upd_name = upd_name.SubstringNE(0, upd_name.GetLength() - 1);
			}
			if (ttmsm12["LADLE_NO"].ToString().Trim() != "")
			{
				ttmsm12["BACK_C1"] = Db::QueryCString(" SELECT WORK_MAKER FROM TTMSM11 WHERE LADLE_NO='TS" + ttmsm12["LADLE_NO"].ToString() + "' AND RESUME_SEQ_NO =(SELECT MAX(RESUME_SEQ_NO) FROM TTMSM11 WHERE LADLE_NO='TS" + ttmsm12["LADLE_NO"].ToString() + "') ");
				upd_name += "BACK_C1,";
			}
			ttmsm12["REC_REVISE_TIME"] = datetimeNow;
			ttmsm12["REC_REVISOR"] = s.userid;;
			ttmsm12.Update(upd_name, "HEAT_NO");
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
