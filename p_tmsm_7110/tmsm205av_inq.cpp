/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      wcm
Version:     1.0
Date:        2024-09-27
Description: 设备检修记录查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsm205av_inq)


int f_tmsm205av_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString date_c = CDateTime::Now().ToString("yyyyMMdd");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sql = "";
	CString sqlwhere = "";
	CString s_userid("");
	CString get_columnname = " ";
	CString begin_time = "";
	CString end_time = "";
	CString c_orderid("");
	CString sap_erp_matnr("");
	CString sampl_entr_no("");
	CString table_name("");
	//CModel ttmsm201("TTMSM201");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		begin_time = bcls_rec->Tables[0].Rows[0]["DATE_C_FROM"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["DATE_C_TO"].ToString();
		//查询条件改为日期范围
		
		sqlstr = " select * from TTMSM205 where flag1=0";
		if (end_time.Trim() != "")
		{
			
			sql += " and DATE_C <=@end_time";
		}
		if (begin_time.Trim() != "")
		{	
			sql += " and DATE_C >=@begin_time";
		}
		if (bcls_rec->Tables[0].Rows[0]["RESPONSIBILITY_PLANT_3T"].ToString().Trim() != "")
		{
			sql += " AND RESPONSIBILITY_PLANT_3T='" + bcls_rec->Tables[0].Rows[0]["RESPONSIBILITY_PLANT_3T"].ToString() + "' ";
		}
		if (bcls_rec->Tables[0].Rows[0]["LINE_DESC"].ToString().Trim() != "")
		{
			sql += " AND LINE_DESC='" + bcls_rec->Tables[0].Rows[0]["LINE_DESC"].ToString() + "' ";
		}
		if (bcls_rec->Tables[0].Rows[0]["DEV_NAME"].ToString().Trim() != "")
		{
			sql += " AND DEV_NAME='" + bcls_rec->Tables[0].Rows[0]["DEV_NAME"].ToString() + "' ";
		}
		if (bcls_rec->Tables[0].Rows[0]["ABNR_TYP"].ToString().Trim() != "")
		{
			sql += " AND ABNR_TYP='" + bcls_rec->Tables[0].Rows[0]["ABNR_TYP"].ToString() + "' ";
		}
		if (bcls_rec->Tables[0].Rows[0]["RESP"].ToString().Trim() != "")
		{
			sql += " AND RESP='" + bcls_rec->Tables[0].Rows[0]["RESP"].ToString() + "' ";
		}
		if (bcls_rec->Tables[0].Rows[0]["TYPE_CODE1"].ToString().Trim() != "")
		{
			sql += " AND TYPE_CODE1='" + bcls_rec->Tables[0].Rows[0]["TYPE_CODE1"].ToString() + "' ";
		}
		sqlstr += sql;
		sqlstr = sqlstr + " order by SEQ_NO desc";
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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

