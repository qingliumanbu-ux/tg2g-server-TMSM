/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     张利锋
Version:    1.0
Date:       2014-07-11
Description: 历史行车信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

//业务头文件

//外部函数声明

BM2F_ENTERACE(tmsm54_inq)

int f_tmsm54_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString	crane_inst_no("");//行车命令号  
	CString	pono("");//制造命令号
	CString	start_time("");//
	CString	end_time("");//

	CDecimal cd_count = 0;
	CDecimal totalWt1 = 0;
	int	page_size = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */

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
		if (bcls_rec->Tables[0].Columns.Contains("CRANE_INST_NO"))
			crane_inst_no = bcls_rec->Tables[0].Rows[0]["CRANE_INST_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();

		if (bcls_rec->Tables.Contains("PageInfo"))
		{
			if (bcls_rec->Tables[0].Columns.Contains("PageNo"))
				current_page_no = bcls_rec->Tables["PageInfo"].Rows[0]["PageNo"].ToDecimal().ToInt32();
			if (bcls_rec->Tables[0].Columns.Contains("PageSize"))
				page_size = bcls_rec->Tables["PageInfo"].Rows[0]["PageSize"].ToDecimal().ToInt32();
		}

		Log::Info("", __FUNCTION__, "cs_stride_code		= [{0}]", cs_stride_code);
		Log::Info("", __FUNCTION__, "cs_craneno	= [{0}]", cs_craneno);
		Log::Info("", __FUNCTION__, "crane_inst_no	= [{0}]", crane_inst_no);
		Log::Info("", __FUNCTION__, "pono		= [{0}]", pono);
		Log::Info("", __FUNCTION__, "start_time	= [{0}]", start_time);
		Log::Info("", __FUNCTION__, "end_time		= [{0}]", end_time);
		Log::Info("", __FUNCTION__, "PageNo	= [{0}]", current_page_no);
		Log::Info("", __FUNCTION__, "PageSize		= [{0}]", current_page_no);

		CString  c_sql_where = "  WHERE  1 = 1 "; //查询条件。
		CString c_sql_condition = " SELECT * FROM ttmsm54  ";
		//CString c_sql_condition2 = " SELECT COUNT(1) FROM ttmsm53  ";
		CString c_sql_orderBY = " ORDER BY rec_create_time DESC ";

		if (cs_stride_code.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND STRIDE_CODE = @cs_stride_code ";
		}
		if (cs_craneno.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND CRANENO= @cs_craneno ";
		}
		if (crane_inst_no.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND CRANE_INST_NO like @crane_inst_no||'%' ";
		}
		if (pono.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND PONO like @pono||'%' ";
		}
		if (start_time.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND REC_CREATE_TIME >= @start_time ";
		}
		if (end_time.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND REC_CREATE_TIME <= @end_time ";
		}
		//查询语句+ WHERE 语句。
		c_sql_condition = c_sql_condition + c_sql_where + c_sql_orderBY;


		Log::Trace("", __FUNCTION__, "c_sql_condition[{0}]  ", c_sql_condition);



		// 设置SQL中的变量
		cmd_sql.Parameters.Set("cs_stride_code", cs_stride_code);
		cmd_sql.Parameters.Set("cs_craneno", cs_craneno);
		cmd_sql.Parameters.Set("crane_inst_no", crane_inst_no);
		cmd_sql.Parameters.Set("pono", pono);
		cmd_sql.Parameters.Set("start_time", start_time);
		cmd_sql.Parameters.Set("end_time", end_time);

		//获取查询返回的信息。(多记录获取模式)
		sqlstr = c_sql_condition;
		cmd_sql.SetCommandText(c_sql_condition);// 设置执行的SQL语句
		//逐行读取
		cmd_sql.ExecuteReader();
		if (page_size > 0)
		{
			cmd_sql.ExecuteQuery(bcls_ret->Tables[0],(current_page_no-1)*page_size,page_size);
		}
		else
		{
			cmd_sql.ExecuteQuery(bcls_ret->Tables[0]);
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
