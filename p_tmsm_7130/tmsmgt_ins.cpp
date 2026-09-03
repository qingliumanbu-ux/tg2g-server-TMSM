/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2111
Version:     1.0
Date:        2023-09-07
Description: 钢铁罐图形化初始化
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsmgt_ins)


int f_tmsmgt_ins(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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

	CString ladle_no("");
	CString new_place("");
	CModel ttmsm01("TTMSM01");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{

		Log::Trace(" ", " ", "jin");
		ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		ttmsm01["LOCA_NAME"] = "YSQ";
		ttmsm01["REXUNHUAN"] = "0";
		ttmsm01["SHANGSHUIKOU"] = "0";
		ttmsm01["HUANBAN"] = "0";
		ttmsm01["DICHUI"] = "0";
		ttmsm01["SHUIKOU"] = "0";
		ttmsm01["XIAOXIU"] = "0";
		ttmsm01["ZHONGXIU"] = "0";
		ttmsm01["DAXIU"] = "0";
		ttmsm01.Print();
		ttmsm01.Insert();

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


