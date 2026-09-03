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
BM2F_ENTERACE(tmsm12av_inq)

int f_tmsm12av_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString sql_time("");
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
	CModel ttmsm12("TTMSM12");
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass tmp;


	try
	{
		
		Log::Trace("", __FUNCTION__, "ABC[{0}]", bcls_rec->Tables[0].Columns.get_Count());
		c_out_steel_time_from = bcls_rec->Tables[0].Rows[0]["OUT_STEEL_TIME_FROM"].ToString();
		c_out_steel_time_to= bcls_rec->Tables[0].Rows[0]["OUT_STEEL_TIME_TO"].ToString();
		c_cast_end_time_from = bcls_rec->Tables[0].Rows[0]["CAST_END_TIME_FROM"].ToString();
		c_cast_end_time_to = bcls_rec->Tables[0].Rows[0]["CAST_END_TIME_TO"].ToString();
		c_date_c_from= bcls_rec->Tables[0].Rows[0]["DATE_C_FROM"].ToString();
		c_date_c_to = bcls_rec->Tables[0].Rows[0]["DATE_C_TO"].ToString();
		ttmsm12.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (c_out_steel_time_from != "")
		{
			sql += " AND t.OUT_STEEL_TIME >=@OUT_STEEL_TIME_FROM";
		}
		if (c_out_steel_time_to != "")
		{
			sql += " AND t.OUT_STEEL_TIME <=@OUT_STEEL_TIME_TO";
		}
		if (c_cast_end_time_from != "")
		{
			sql += " AND t.CAST_END_TIME >=@CAST_END_TIME_FROM";
		}
		if (c_cast_end_time_to != "")
		{
			sql += " AND t.CAST_END_TIME <=@CAST_END_TIME_TO";
		}
		if (ttmsm12["FURNACE_NO"].ToString().Trim() != "")
		{
			sql += " AND t.FURNACE_NO like '%'|| @FURNACE_NO||'%'";
		}
		if (ttmsm12["SHIFT_GROUP"].ToString().Trim() != "")
		{
			sql += " and t.SHIFT_GROUP= @SHIFT_GROUP ";
		}
		if (ttmsm12["LADLE_NO"].ToString().Trim() != "")
		{
			sql += " and t.LADLE_NO= @LADLE_NO ";
		}

		if (c_date_c_from != "")
		{
			sql_time += " AND REC_CREATE_TIME >=@DATE_C_FROM";
		}
		if (c_date_c_to != "")
		{
			sql_time += " AND REC_CREATE_TIME <=@DATE_C_TO";
		}

		sqlstr = "select t_sumed.* from (SELECT t.*,SUM(t.onlion_time) OVER(PARTITION BY t.group_id ORDER BY t.LADLE_LIFE) AS SUM_ONLION_TIME"
			" from(SELECT gf.*, SUM(is_new_group) OVER(ORDER BY rn) AS group_id"
			" FROM(SELECT rd.*, CASE WHEN rn = 1 THEN 1 WHEN LADLE_LIFE < LAG(LADLE_LIFE) OVER(ORDER BY rn) THEN 1 ELSE 0 END AS is_new_group"
			" FROM(SELECT T.*, ROW_NUMBER() OVER(ORDER BY t.Ld_Type || t.ladle_no, T.REC_CREATE_TIME, T.LADLE_LIFE) AS rn"
			" FROM(select  T.*,"
			"case when T.OUT_STEEL_TIME != ' ' and T.PRE_HEAT_CAST_E_TIME != ' ' then ROUND((TO_DATE(T.OUT_STEEL_TIME, 'YYYY-MM-DD HH24:MI:SS') -"
			"TO_DATE(T.PRE_HEAT_CAST_E_TIME, 'YYYY-MM-DD HH24:MI:SS')) * 24 * 60, 0) else 0 end RETENTION_TIME,"
			"case when T.CAST_END_TIME != ' ' and T.OUT_STEEL_TIME != ' ' then ROUND((TO_DATE(T.CAST_END_TIME, 'YYYY-MM-DD HH24:MI:SS') -"
			"TO_DATE(T.OUT_STEEL_TIME, 'YYYY-MM-DD HH24:MI:SS')) * 24 * 60, 0) else 0 end ONLION_TIME"
			" from ttmsm12 T where 1 = 1 AND LD_TYPE = 'TS')T) rd)gf)t where 1 = 1  ";
		sqlstr += sql;
		sqlstr += " and t.rec_create_time>=(select  min(p.rec_create_time) as min_1_time  from (SELECT t.ladle_no, t.rec_create_time"
			" FROM ttmsm12 t"
			" INNER JOIN(SELECT ladle_no"
			" FROM ttmsm12"
			" WHERE LD_TYPE = 'TS'";
		sqlstr += sql_time;
		sqlstr += " GROUP BY ladle_no HAVING MIN(LADLE_LIFE) = 1) q ON t.ladle_no = q.ladle_no"
			" WHERE t.LD_TYPE = 'TS' ";
		sqlstr += sql;
		sqlstr += sql_time;
		sqlstr += " AND LADLE_LIFE = 1)p)) t_sumed  order by REC_CREATE_TIME ";
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);

		cmd_inq.Parameters.Set("DATE_C_TO", c_date_c_to);
		cmd_inq.Parameters.Set("DATE_C_FROM", c_date_c_from);
		cmd_inq.Parameters.Set("OUT_STEEL_TIME_FROM", c_out_steel_time_from);
		cmd_inq.Parameters.Set("OUT_STEEL_TIME_TO", c_out_steel_time_to);
		cmd_inq.Parameters.Set("CAST_END_TIME_FROM", c_cast_end_time_from);
		cmd_inq.Parameters.Set("CAST_END_TIME_TO", c_cast_end_time_to);
		cmd_inq.Parameters.Set("FURNACE_NO", ttmsm12["FURNACE_NO"].ToString());
		cmd_inq.Parameters.Set("SHIFT_GROUP",ttmsm12["SHIFT_GROUP"].ToString());
		cmd_inq.Parameters.Set("LADLE_NO", ttmsm12["LADLE_NO"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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
