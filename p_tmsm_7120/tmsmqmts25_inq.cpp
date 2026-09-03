/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     张利锋
Version:    1.0
Date:       2023-05-19
Description:化学成分查询
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

BM2F_ENTERACE(tmsmqmts25_inq)

int f_tmsmqmts25_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	long  v_total_count = 0;
	long  fetchRowCount = 0;
	/* 业务变量 */
	CString	datetime("");
	CString	heat_no("");//代码类
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
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		Log::Info("", __FUNCTION__, "heat_no		= [{0}]", heat_no);

		CString sql = "SELECT "
		"ELM_NAME  化学名称, ELM_ACT 重量"	
		" FROM TQMTS25 A WHERE WHOLE_BACKLOG_CODE = 'B' AND HEAT_NO = '"+heat_no+"'";

		//查询语句+ WHERE 语句。

		Log::Trace("", __FUNCTION__, "sql=[{0}]  ", sql);
		// 设置SQL中的变量
		if (heat_no.Trim()!="")
		{
			cmd_sql.SetCommandText(sql);// 设置执行的SQL语句 
			if (!bcls_ret->Tables.Contains("HXCF"))
			{
				bcls_ret->Tables.Add("HXCF");
			}
			cmd_sql.ExecuteQuery(bcls_ret->Tables["HXCF"]);
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
