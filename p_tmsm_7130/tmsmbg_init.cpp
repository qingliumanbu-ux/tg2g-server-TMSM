/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2111
Version:     1.0
Date:        2023-09-07
Description: 钢铁罐图形化初始化
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsmbg_init)


int f_tmsmbg_init(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString s_userid("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		sqlstr = " select LADLE_NO,\
			LOCA_NAME,\
			REXUNHUAN,\
			SHANGSHUIKOU,\
			HUANBAN,\
			DICHUI,\
			SHUIKOU,\
			XIAOXIU,\
			ZHONGXIU,\
			DAXIU,HEAT_NO,PONO,ST_NO, tmd.DEV_X,\
			tmd.DEV_Y\
			from TTMSM01 tm1\
			left join ttmsmdw tmd on tm1.LOCA_NAME = tmd.DEV_CODE\
			where 1 = 1 and DEV_Y is null ORDER BY LADLE_NO ";
		Log::Trace("", "", "sqlstr", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		bcls_ret->Tables.Add();
		sqlstr = " select LADLE_NO,\
			LOCA_NAME,\
			REXUNHUAN,\
			SHANGSHUIKOU,\
			HUANBAN,\
			DICHUI,\
			SHUIKOU,\
			XIAOXIU,\
			ZHONGXIU,\
			DAXIU,HEAT_NO,PONO,ST_NO, tmd.DEV_X,\
			tmd.DEV_Y\
			from TTMSM01 tm1\
			left join ttmsmdw tmd on tm1.LOCA_NAME = tmd.DEV_CODE\
			where 1 = 1 and DEV_Y is not null ORDER BY LADLE_NO ";
		Log::Trace("", "", "sqlstr", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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

	return doFlag;
}


