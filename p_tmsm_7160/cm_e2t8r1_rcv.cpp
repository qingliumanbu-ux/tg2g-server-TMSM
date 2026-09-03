/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      KE2111
Version:     1.0
Date:        2023-10-23
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"

/*<remark>=========================================================
/// <summary>
///
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
int f_tmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2F_ENTERACE_TELE(cm_e2t8r1_rcv)

int f_cm_e2t8r1_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	/* 业务变量 */

	/* 实体类定义 */
	CModel ttmsm01("TTMSM01");
	CModel ttmsm0a("TTMSM0A");
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);

	CDecimal devo_wt = 0;

	CString c_loca_name = "";
	CString c_ladle_no = "";

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	//工器具事件
	if (!bcls_rec->Tables.Contains("TM0096")) {
		bcls_rec->Tables.Add("TM0096");
	}
	bcls_rec->Tables["TM0096"].Columns.Add(DT_STRING, "EVENT_ID");
	bcls_rec->Tables["TM0096"].Columns.Add(DT_STRING, "STATION_ID");
	bcls_rec->Tables["TM0096"].Columns.Add(ttmsm01);



	try
	{
		Log::Trace("", "", "{0}", "JIN");
		bcls_rec->Tables[0].Columns["AGGREGATE_NAME"].set_ColumnName("LOCA_NAME");
		bcls_rec->Tables[0].Columns["LADLE_NUMBER"].set_ColumnName("LADLE_NO");

		ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (ttmsm01["LOCA_NAME"].ToString().Trim() == "") {
			sprintf(s.msg, "工位不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (ttmsm01["LADLE_NO"].ToString().Trim() == "") {
			sprintf(s.msg, "钢包号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		ttmsm0a.CopyFrom(ttmsm01);
		ttmsm0a["EVENT_ID"] = "TM06";
		ttmsm0a["STATION_ID"] = "0";
		ttmsm0a.MergeTo(bcls_rec->Tables["TM0096"], false);

		/*doFlag = f_tmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0) {
			s.flag = 0;
			doFlag = 0;
		}*/
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


