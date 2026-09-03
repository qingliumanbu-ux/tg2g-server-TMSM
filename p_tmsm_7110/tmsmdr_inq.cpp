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
BM2F_ENTERACE(tmsmdr_inq)

int f_tmsmdr_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString v_name("");
	CString v_shift_group("");
	CString sqlstr("");
	CString sqlstr1("");

	CModel ttmsmpz("TTMSMPZ");
	CModel ttmsmpz01("TTMSMPZ01");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass tmp;
	EIClass tmp1;

	try
	{
		v_name = bcls_rec->Tables[0].Rows[0]["NAME"].ToString().SubstringNE(3);
		sqlstr = " select CHINESE_USER_NAME,ITEM_KIND,a.ACTIVITY_NAME from TTMSMPZ01 a left join TTMSMPZ b on a.ACTIVITY_NAME=b.ACTIVITY_NAME where b.SHOW_SEQ='"+ v_name +"' ";
		Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(tmp.Tables[0]);
		cmd_inq.Close();

		sqlstr = " select * from ttmsmdr where ACTIVITY_NAME='"+tmp.Tables[0].Rows[0]["ACTIVITY_NAME"].ToString() + "' ";
		Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(tmp1.Tables[0]);
		Log::Trace("", __FUNCTION__, "tmp1.Tables[0]		= [{0}]", tmp1.Tables[0].Columns.get_Count());
		for (int i = 0; i < 9; i++)
		{
			tmp1.Tables[0].Columns.RemoveAt(0);
		}
		
		Log::Trace("", __FUNCTION__, "tmp1.Tables[0]		= [{0}]", tmp1.Tables[0].Columns.get_Count());
		cmd_inq.Close();

		CDecimal seq_s = 1;
		CDecimal seq_i = 1;
		CString h_s = "";
		CString h_i = "";

		bcls_ret->Tables.Add();

		for (int i = 0; i < tmp.Tables[0].Rows.get_Count(); i++)
		{
			if (tmp.Tables[0].Rows[i]["ITEM_KIND"].ToString() == "S")
			{
				h_s = "BACK_S" + seq_s.ToString();
				bcls_ret->Tables[0].Columns.Add(DT_STRING, h_s);
				bcls_ret->Tables[1].Columns.Add(DT_STRING, (const char*)tmp.Tables[0].Rows[i]["CHINESE_USER_NAME"]);
				seq_s = seq_s + 1;
			}
			if (tmp.Tables[0].Rows[i]["ITEM_KIND"].ToString() == "N")
			{
				h_i = "BACK_F" + seq_i.ToString();
				bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, h_i);
				bcls_ret->Tables[1].Columns.Add(DT_STRING, (const char*)tmp.Tables[0].Rows[i]["CHINESE_USER_NAME"]);
				seq_i = seq_i + 1;
			}
		}

		
		/*for (int i = 0; i < bcls_ret->Tables[0].Columns.get_Count(); i++)
		{
			if (bcls_ret->Tables[0].Columns[i].get_DataType() == DT_STRING)
			{
				h_s = "BACK_S" + seq_s.ToString();
				tmp1.Tables[0].Columns[h_s].set_Caption(bcls_ret->Tables[0].Columns[i].get_ColumnName());
				
			}
			if (bcls_ret->Tables[0].Columns[i].get_DataType() == DT_DECIMAL)
			{
				h_i = "BACK_F" + seq_i.ToString();
				tmp1.Tables[0].Columns[h_i].set_Caption(bcls_ret->Tables[0].Columns[i].get_ColumnName());
				seq_i = seq_i + 1;
			}
		}*/
		for (int i = 0; i < tmp1.Tables[0].Rows.get_Count(); i++)
		{
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0][i].Merge(tmp1.Tables[0][i]);
		}
		if (bcls_ret->Tables[0].Rows.get_Count() == 0)
		{
			bcls_ret->Tables[0].Rows.Add();
		}
		bcls_ret->Tables[1].Rows.Add();
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
