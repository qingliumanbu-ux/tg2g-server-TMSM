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
BM2F_ENTERACE(tmsm11av_pro)

int f_tmsm11av_pro(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CModel ttmsm11("TTMSM11");
	CModel ttmsm11_o("TTMSM11");


	try
	{
		if (bcls_rec->Tables[0].Rows[0]["FN_NO"].ToString() == "F3")
		{
			ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			ttmsm11["REC_CREATOR"] = s.userid;
			ttmsm11["REC_CREATE_TIME"] = datetimeNow;
			ttmsm11["RESUME_SEQ_NO"]= CDateTime::Now().ToString("yyyyMMddHHmmssff6");
			if (ttmsm11["DRYING_ST"].ToString().Trim() != "" && ttmsm11["DRYING_ET"].ToString().Trim() != "")
			{
				ttmsm11["LADLE_HEATING_DURATION"] = (CDateTime::Parse(ttmsm11["DRYING_ET"].ToString()) - CDateTime::Parse(ttmsm11["DRYING_ST"].ToString())).TotalHours();
			}
			ttmsm11.Insert();
		}
		if (bcls_rec->Tables[0].Rows[0]["FN_NO"].ToString() == "F4")
		{
			ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			ttmsm11_o.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			ttmsm11_o.Query("RESUME_SEQ_NO");
			int count_row = bcls_rec->Tables[0].Columns.get_Count();
			for (int i = 0; i < count_row; i++)
			{
				/*如果查询条件的值为空则跳出*/
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

				Log::Trace("", "", "条件查询sql {0}++[{1}],", i, bcls_rec->Tables[0].Columns[i].get_ColumnName() + ":" + bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
				Log::Trace("", "", "asdfgfdswaq	sql {0}++[{1}],", i, bcls_rec->Tables[0].Columns[i].get_ColumnName().Substring(bcls_rec->Tables[0].Columns[i].get_ColumnName().GetLength()));

				if (bcls_rec->Tables[0].Columns[i].get_ColumnName().ToUpper() == "FN_NO"
					|| bcls_rec->Tables[0].Columns[i].get_ColumnName() == "LADLE_HEATING_DURATION"
					|| bcls_rec->Tables[0].Columns[i].get_ColumnName() == "LADLE_LIFE")
				{
					continue;
				}
				Log::Trace("", "", "sql,", sql, sql.GetLength());
				if (bcls_rec->Tables[0].Rows[0][i].ToString().Trim()!=ttmsm11_o[bcls_rec->Tables[0].Columns[i].get_ColumnName()].ToString())
				{
					sql += bcls_rec->Tables[0].Columns[i].get_ColumnName() + ",";
				}
				Log::Trace("", "", "sql,", sql, sql.GetLength());
			}
			if (ttmsm11["DRYING_ST"].ToString().Trim() != "" && ttmsm11["DRYING_ET"].ToString().Trim() != "")
			{
				ttmsm11["LADLE_HEATING_DURATION"] = (CDateTime::Parse(ttmsm11["DRYING_ET"].ToString()) - CDateTime::Parse(ttmsm11["DRYING_ST"].ToString())).TotalMinutes();
				sql += "LADLE_HEATING_DURATION,";
			}
			Log::Trace("", "", "sql,", sql, sql.GetLength());
			if (sql.GetLength()>0)
			{
				sql = sql.SubstringNE(0, sql.GetLength() - 1);
				ttmsm11.Update(sql, "RESUME_SEQ_NO");
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
