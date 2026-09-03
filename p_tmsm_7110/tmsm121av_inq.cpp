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
BM2F_ENTERACE(tmsm121av_inq)

int f_tmsm121av_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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


	try
	{
		
		Log::Trace("", __FUNCTION__, "ABC[{0}]", bcls_rec->Tables[0].Columns.get_Count());

		for (int i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
		{
			get_columnname = bcls_rec->Tables[0].Columns[i].get_ColumnName();

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



			
			if (get_columnname.SubstringNE(get_columnname.GetLength() - 4, get_columnname.GetLength()) == "FROM")
			{

				sql += " AND " + bcls_rec->Tables[0].Columns[i].get_ColumnName().Replace("_FROM", "") + ">= @" + bcls_rec->Tables[0].Columns[i].get_ColumnName();
				cmd_inq.Parameters.Set(bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
				Log::Trace("", __FUNCTION__, "ABC[{0}]", get_columnname.SubstringNE(get_columnname.GetLength() - 4, get_columnname.GetLength()));
			}
			else if (get_columnname.SubstringNE(get_columnname.GetLength() - 2, get_columnname.GetLength()) == "TO")
			{
				sql += " AND " + bcls_rec->Tables[0].Columns[i].get_ColumnName().Replace("_TO", "") + "<= @" + bcls_rec->Tables[0].Columns[i].get_ColumnName();
				cmd_inq.Parameters.Set(bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
				Log::Trace("", __FUNCTION__, "ABC[{0}]", get_columnname.SubstringNE(get_columnname.GetLength() - 2, get_columnname.GetLength()));
			}
			else
			{



				Log::Trace("", __FUNCTION__, "ABC[{0}]", bcls_rec->Tables[0].Columns[i].get_ColumnName().Replace("_FROM", ""));

				Log::Trace("", __FUNCTION__, "b		= [{0}][{1}]", bcls_rec->Tables[0].Columns[i].get_ColumnName().SubstringNE(-1, -4), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
				sql += " AND " + bcls_rec->Tables[0].Columns[i].get_ColumnName() + " LIKE @" + bcls_rec->Tables[0].Columns[i].get_ColumnName() + "||'%' ";
				cmd_inq.Parameters.Set(bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());

			}



		}
		
		sql += " order by heat_no,REC_CREATE_TIME ";
		sqlstr ="SELECT gi.*,SUM(gi.onlion_time) OVER(PARTITION BY gi.group_id ORDER BY gi.LADLE_LIFE) AS SUM_ONLION_TIME"
			" from(SELECT gf.*, SUM(is_new_group) OVER(ORDER BY rn) AS group_id"
			" FROM(SELECT rd.*, CASE WHEN rn = 1 THEN 1 WHEN LADLE_LIFE < LAG(LADLE_LIFE) OVER(ORDER BY rn) THEN 1 ELSE 0 END AS is_new_group"
			" FROM(SELECT T.*, ROW_NUMBER() OVER(ORDER BY t.Ld_Type || t.ladle_no, T.REC_CREATE_TIME, T.LADLE_LIFE) AS rn"
			" FROM(select  T.*,"
			"case when T.OUT_STEEL_TIME != ' ' and T.PRE_HEAT_CAST_E_TIME != ' ' then ROUND((TO_DATE(T.OUT_STEEL_TIME, 'YYYY-MM-DD HH24:MI:SS') -"
			"TO_DATE(T.PRE_HEAT_CAST_E_TIME, 'YYYY-MM-DD HH24:MI:SS')) * 24 * 60, 0) else 0 end RETENTION_TIME,"
			"case when T.CAST_END_TIME != ' ' and T.OUT_STEEL_TIME != ' ' then ROUND((TO_DATE(T.CAST_END_TIME, 'YYYY-MM-DD HH24:MI:SS') -"
			"TO_DATE(T.OUT_STEEL_TIME, 'YYYY-MM-DD HH24:MI:SS')) * 24 * 60, 0) else 0 end ONLION_TIME"
			" from ttmsm12 T where 1 = 1 and FURNACE_NO = '0' AND LD_TYPE = 'TS')T) rd)gf)gi where 1 = 1  ";
		sqlstr += sql;
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		bcls_ret->Tables.Add();
		sqlstr = "SELECT gi.*,SUM(gi.onlion_time) OVER(PARTITION BY gi.group_id ORDER BY gi.LADLE_LIFE) AS SUM_ONLION_TIME"
			" from(SELECT gf.*, SUM(is_new_group) OVER(ORDER BY rn) AS group_id"
			" FROM(SELECT rd.*, CASE WHEN rn = 1 THEN 1 WHEN LADLE_LIFE < LAG(LADLE_LIFE) OVER(ORDER BY rn) THEN 1 ELSE 0 END AS is_new_group"
			" FROM(SELECT T.*, ROW_NUMBER() OVER(ORDER BY t.Ld_Type || t.ladle_no, T.REC_CREATE_TIME, T.LADLE_LIFE) AS rn"
			" FROM(select  T.*,"
			"case when T.OUT_STEEL_TIME != ' ' and T.PRE_HEAT_CAST_E_TIME != ' ' then ROUND((TO_DATE(T.OUT_STEEL_TIME, 'YYYY-MM-DD HH24:MI:SS') -"
			"TO_DATE(T.PRE_HEAT_CAST_E_TIME, 'YYYY-MM-DD HH24:MI:SS')) * 24 * 60, 0) else 0 end RETENTION_TIME,"
			"case when T.CAST_END_TIME != ' ' and T.OUT_STEEL_TIME != ' ' then ROUND((TO_DATE(T.CAST_END_TIME, 'YYYY-MM-DD HH24:MI:SS') -"
			"TO_DATE(T.OUT_STEEL_TIME, 'YYYY-MM-DD HH24:MI:SS')) * 24 * 60, 0) else 0 end ONLION_TIME"
			" from ttmsm12 T where 1 = 1 and FURNACE_NO = '1' AND LD_TYPE = 'TS')T) rd)gf)gi where 1 = 1  ";
		sqlstr += sql;
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();

		bcls_ret->Tables.Add();
		sqlstr ="SELECT gi.*,SUM(gi.onlion_time) OVER(PARTITION BY gi.group_id ORDER BY gi.LADLE_LIFE) AS SUM_ONLION_TIME"
			" from(SELECT gf.*, SUM(is_new_group) OVER(ORDER BY rn) AS group_id"
			" FROM(SELECT rd.*, CASE WHEN rn = 1 THEN 1 WHEN LADLE_LIFE < LAG(LADLE_LIFE) OVER(ORDER BY rn) THEN 1 ELSE 0 END AS is_new_group"
			" FROM(SELECT T.*, ROW_NUMBER() OVER(ORDER BY t.Ld_Type || t.ladle_no, T.REC_CREATE_TIME, T.LADLE_LIFE) AS rn"
			" FROM(select  T.*,"
			"case when T.OUT_STEEL_TIME != ' ' and T.PRE_HEAT_CAST_E_TIME != ' ' then ROUND((TO_DATE(T.OUT_STEEL_TIME, 'YYYY-MM-DD HH24:MI:SS') -"
			"TO_DATE(T.PRE_HEAT_CAST_E_TIME, 'YYYY-MM-DD HH24:MI:SS')) * 24 * 60, 0) else 0 end RETENTION_TIME,"
			"case when T.CAST_END_TIME != ' ' and T.OUT_STEEL_TIME != ' ' then ROUND((TO_DATE(T.CAST_END_TIME, 'YYYY-MM-DD HH24:MI:SS') -"
			"TO_DATE(T.OUT_STEEL_TIME, 'YYYY-MM-DD HH24:MI:SS')) * 24 * 60, 0) else 0 end ONLION_TIME"
			" from ttmsm12 T where 1 = 1 and FURNACE_NO = '2' AND LD_TYPE = 'TS')T) rd)gf)gi where 1 = 1  ";
		sqlstr += sql;
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
		cmd_inq.Close();


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
