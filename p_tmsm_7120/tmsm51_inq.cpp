/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     张利锋
Version:    1.0
Date:       2023-05-19
Description:行车作业码查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 结晶器信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

BM2F_ENTERACE(tmsm51_inq)

int f_tmsm51_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	long  v_total_count = 0;
	long  fetchRowCount = 0;
	/* 业务变量 */
	CString	datetime("");
	CString	code_class("");//代码类
	CString	type("");//是否在调用状态
	CString	oper_type("");//行车作业代码
	CString	operator_type("");//

	CDecimal cd_count = 0;
	CDecimal totalWt1 = 0;

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
		if (bcls_rec->Tables[0].Columns.Contains("CODE_CLASS"))
			code_class = bcls_rec->Tables[0].Rows[0]["CODE_CLASS"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("TYPE"))
			type = bcls_rec->Tables[0].Rows[0]["TYPE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("OPER_TYPE"))
			oper_type = bcls_rec->Tables[0].Rows[0]["OPER_TYPE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("OPERATOR_TYPE"))
			operator_type = bcls_rec->Tables[0].Rows[0]["OPERATOR_TYPE"].ToString().Trim();
		Log::Info("", __FUNCTION__, "code_class		= [{0}]", code_class);
		Log::Info("", __FUNCTION__, "oper_type	= [{0}]", oper_type);
		Log::Info("", __FUNCTION__, "operator_type	= [{0}]", operator_type);
		Log::Info("", __FUNCTION__, "type	= [{0}]", type);

		CString sql = "SELECT CODE,CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS='" + code_class + "' AND CODE " + ((type == "1") ? " " : "NOT ") + " IN (SELECT OPER_PLACE FROM TTMSM51 WHERE OPER_TYPE='" + oper_type + "' AND OPERATE_TYPE='" + operator_type + "')";

		//查询语句+ WHERE 语句。

		Log::Trace("", __FUNCTION__, "sql=[{0}]  ", sql);
		// 设置SQL中的变量
		cmd_sql.SetCommandText(sql);// 设置执行的SQL语句 
		//获取第一行+第一列的信息。
		cmd_sql.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_sql.Close(); //关闭游标  


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
