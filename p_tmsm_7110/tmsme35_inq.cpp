/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-06-14 14:47:19
Description: 包盖基本信息查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme35_inq)


int f_tmsme35_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWMA7 twma7(conn);
	CModel ttmsm35 = CModel("TTMSM35");

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
		// 获取前台传入参数
		s_userid = s.userid;

		//分页信息
		CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "recordsum");

		//2)获取分页信息
		if (bcls_rec->Tables.Contains("PageInfo"))
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		else
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = -1;  //每页记录数量
		}

		Log::Trace("", __FUNCTION__, "pageInfo.RecordFrom[{0}]pageInfo.PageSize[{1}]", pageInfo.RecordFrom, pageInfo.PageSize);


		ttmsm35.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Trace("", __FUNCTION__, "ttmsm35.HEAT_NO	= [{0}]", (const char*)ttmsm35["HEAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "ttmsm35.CLAD_NO	= [{0}]", (const char*)ttmsm35["CLAD_NO"].ToString());

		if (ttmsm35["CLAD_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND CLAD_NO = @clad_no ";
		}
		if (ttmsm35["HEAT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND HEAT_NO = @heat_no ";
		}
		/*	if (ttmsm35["REC_CREATE_TIME"].ToString().Trim() != "")
		{
		sqlwhere += "  AND CHANGE_TIME >= @begin_time ";
		}
		if (ttmsm35["CHANGE_TIME"].ToString().Trim() != "")
		{
		sqlwhere += "  AND CHANGE_TIME <= @end_time ";
		}*/

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT COUNT(1) FROM TTMSM35 WHERE 1=1 ";
			break;
		}

		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlstr_语句[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", ttmsm35["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("clad_no", ttmsm35["CLAD_NO"].ToString());
		//cmd_inq.Parameters.Set("begin_time", ttmsm66["REC_CREATE_TIME"].ToString());
		//cmd_inq.Parameters.Set("end_time", ttmsm66["CHANGE_TIME"].ToString());
		rowCount = cmd_inq.ExecuteScalar();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			//sqlstr = "SELECT * FROM TTMSM95 WHERE 1=1 ";

			sqlstr = "SELECT * FROM TTMSM35 WHERE 1=1";
			break;
		}

		sqlstr = sqlstr + sqlwhere;
		sqlstr = sqlstr + " ORDER BY CLAD_NO ";
		Log::Trace("", __FUNCTION__, "sqlstr_语句[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", ttmsm35["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("clad_no", ttmsm35["CLAD_NO"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			fetchRowCount++;

			if (fetchRowCount >    (pageInfo.RecordFrom + pageInfo.PageSize))
			{
				Log::Trace("", __FUNCTION__, "超页面上限值，break");
				break;
			}
			if (!((fetchRowCount > pageInfo.RecordFrom) && (fetchRowCount <= (pageInfo.RecordFrom + pageInfo.PageSize))))
			{
				continue;
			}



		}

		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "sqlstr_2语句[{0}]", sqlstr);

		//返回记录总数
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["recordsum"] = rowCount.ToInt32();
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


