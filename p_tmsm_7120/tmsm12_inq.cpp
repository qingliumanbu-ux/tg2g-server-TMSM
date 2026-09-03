/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     张利锋
Version:    1.0
Date:       2023-05-29
Description: 铁包计划信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

//业务头文件

//外部函数声明

BM2F_ENTERACE(tmsm12_inq)

int f_tmsm12_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	long  v_total_count = 0;
	long  fetchRowCount = 0;
	/* 业务变量 */
	CString	datetime("");
	CString	cs_sm_unit_no("");//炼钢单元号
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		/* 获取输入参数 */
		if (bcls_rec->Tables[0].Columns.Contains("SM_UNIT_NO"))
			cs_sm_unit_no = bcls_rec->Tables[0].Rows[0]["SM_UNIT_NO"].ToString().Trim();

		Log::Info("", __FUNCTION__, "cs_sm_unit_no		= [{0}]", cs_sm_unit_no);



		CString  c_sql_where = "  WHERE  1 = 1 "; //查询条件。
		CString c_sql_condition = " SELECT * FROM TTMSM12  ";
		CString c_sql_orderBY = " ORDER BY IRON_LADLE_NO DESC ";

		if (cs_sm_unit_no.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND SM_UNIT_NO = @cs_sm_unit_no ";
		}

		//查询语句+ WHERE 语句。
		c_sql_condition = c_sql_condition + c_sql_where + c_sql_orderBY;

		Log::Trace("", __FUNCTION__, "c_sql_condition[{0}]  ", c_sql_condition);


		// 设置SQL中的变量
		cmd_inq.Parameters.Set("cs_sm_unit_no", cs_sm_unit_no);

		cmd_inq.SetCommandText(c_sql_condition);// 设置执行的SQL语句
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
