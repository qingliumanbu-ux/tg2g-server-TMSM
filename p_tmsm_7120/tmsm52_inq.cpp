/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-11
Description: 结晶器信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 


//外部函数声明

BM2F_ENTERACE(tmsm52_inq)

int f_tmsm52_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString	cs_craneno("");//行车号

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

		Log::Info("", __FUNCTION__, "cs_stride_code		= [{0}]", cs_stride_code);
		Log::Info("", __FUNCTION__, "cs_craneno	= [{0}]", cs_craneno);



		CString  c_sql_where = "  WHERE  1 = 1 "; //查询条件。
		CString c_sql_condition = " SELECT * FROM ttmsm52  ";
		CString c_sql_condition2 = " SELECT COUNT(1) FROM ttmsm52  ";
		CString c_sql_orderBY = " ORDER BY stride_code ASC,crane_inst_status DESC,crane_inst_seq ASC ";

		if (cs_stride_code.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND STRIDE_CODE = @cs_stride_code ";
		}
		if (cs_craneno.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND CRANENO = @cs_craneno ";
		}
		//查询语句+ WHERE 语句。
		c_sql_condition = c_sql_condition + c_sql_where + c_sql_orderBY;
		c_sql_condition2 = c_sql_condition2 + c_sql_where;

		Log::Trace("", __FUNCTION__, "c_sql_condition[{0}]  ", c_sql_condition);
		Log::Trace("", __FUNCTION__, "c_sql_condition2[{0}]  ", c_sql_condition2);


		// 设置SQL中的变量
		cmd_sql.Parameters.Set("cs_stride_code", cs_stride_code);
		cmd_sql.Parameters.Set("cs_craneno", cs_craneno);


		//获取汇总行数。(单记录获取模式)	
		sqlstr = c_sql_condition2;
		cmd_sql.SetCommandText(c_sql_condition2);// 设置执行的SQL语句 
		//获取第一行+第一列的信息。
		v_total_count = cmd_sql.ExecuteScalar().ToInt32(); //获取第一行+第一列的信息。
		cmd_sql.Close(); //关闭游标  

		//获取查询返回的信息。(多记录获取模式)
		sqlstr = c_sql_condition;
		cmd_sql.SetCommandText(c_sql_condition);// 设置执行的SQL语句
		//逐行读取
		cmd_sql.ExecuteReader();
		cmd_sql.ExecuteQuery(bcls_ret->Tables[0]);

		//返回的记录数。
		Log::Trace("", __FUNCTION__, "RowCount[{0}]  ", v_total_count);


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
