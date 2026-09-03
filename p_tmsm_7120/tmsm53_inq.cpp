/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-11
Description: 结晶器信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

//业务头文件

//外部函数声明

BM2F_ENTERACE(tmsm53_inq)

int f_tmsm53_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	long  v_total_count = 0;
	long  fetchRowCount = 0;
	/* 业务变量 */
	CString	datetime("");
	CString	cs_stride_code("");//跨号
	CString	cs_type_code("");//跨类别
	CString	cs_craneno("");//行车号
	CString	pono("");//制造命令号
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		/* 获取输入参数 */
		if (bcls_rec->Tables[0].Columns.Contains("STRIDE_CODE"))
			cs_stride_code = bcls_rec->Tables[0].Rows[0]["STRIDE_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("CRANENO"))
			cs_craneno = bcls_rec->Tables[0].Rows[0]["CRANENO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("TYPE_CODE"))
			cs_type_code = bcls_rec->Tables[0].Rows[0]["TYPE_CODE"].ToString().Trim();
		Log::Info("", __FUNCTION__, "cs_stride_code		= [{0}]", cs_stride_code);
		Log::Info("", __FUNCTION__, "cs_craneno	= [{0}]", cs_craneno);
		Log::Info("", __FUNCTION__, "pono	= [{0}]", pono);
		Log::Info("", __FUNCTION__, "cs_type_code	= [{0}]", cs_type_code);

		CString  c_sql_where = "  WHERE  1 = 1 "; //查询条件。
		CString c_sql_condition = " SELECT * FROM ttmsm53  ";
		CString c_sql_condition2 = " SELECT COUNT(1) FROM ttmsm53  ";
		CString c_sql_orderBY = " ORDER BY stride_code ASC,crane_inst_status DESC,crane_inst_seq ASC ";

		if (cs_stride_code.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND STRIDE_CODE = @cs_stride_code ";
		}
		if (cs_craneno.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND CRANENO = @cs_craneno ";
		}
		if (pono.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND PONO = @pono ";
		}
		if (cs_type_code.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND TYPE_CODE = @cs_type_code ";
		}
		//查询语句+ WHERE 语句。
		c_sql_condition = c_sql_condition + c_sql_where + c_sql_orderBY;

		Log::Trace("", __FUNCTION__, "c_sql_condition[{0}]  ", c_sql_condition);

		cmd_sql.SetCommandText(c_sql_condition);// 设置执行的SQL语句
		// 设置SQL中的变量
		cmd_sql.Parameters.Set("cs_stride_code", cs_stride_code);
		cmd_sql.Parameters.Set("cs_craneno", cs_craneno);
		cmd_sql.Parameters.Set("pono", pono);
		cmd_sql.Parameters.Set("cs_type_code", cs_type_code);
		cmd_sql.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_sql.Close();

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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
