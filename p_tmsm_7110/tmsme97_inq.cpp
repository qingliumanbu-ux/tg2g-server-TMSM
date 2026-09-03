/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-06-08 11:10:13
Description: 钢包烘烤履历查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme97_inq)

int f_tmsme97_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	CModel ttmsm97 = CModel("TTMSM97");

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


		ttmsm97.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Trace("", __FUNCTION__, "ttmsm97.MOLD_NO	= [{0}]", (const char*)ttmsm97["MOLD_NO"].ToString());
		Log::Trace("", __FUNCTION__, "ttmsm97.MOLD_TYPE	= [{0}]", (const char*)ttmsm97["MOLD_TYPE"].ToString());

		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_F"))
			ttmsm97["DRYING_ST"] = bcls_rec->Tables[0].Rows[0]["START_TIME_F"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_T"))
			ttmsm97["DRYING_ET"] = bcls_rec->Tables[0].Rows[0]["START_TIME_T"].ToString();
		Log::Trace("", __FUNCTION__, "ttmsm97.DRYING_ST	= [{0}]", (const char*)ttmsm97["DRYING_ST"].ToString());
		Log::Trace("", __FUNCTION__, "ttmsm97.DRYING_ET	= [{0}]", (const char*)ttmsm97["DRYING_ET"].ToString());

		if (ttmsm97["MOLD_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND MOLD_NO = @mold_no ";
		}
		if (ttmsm97["MOLD_TYPE"].ToString().Trim() != "")
		{
			sqlwhere += " AND MOLD_TYPE = @mold_type "; //MOLD_TYPE =2表示钢包
		}
		if (ttmsm97["DRYING_ST"].ToString().Trim() != "")
		{
			sqlwhere += "  AND REC_CREATE_TIME >= @s_datetime ";
		}
		if (ttmsm97["DRYING_ET"].ToString().Trim() != "")
		{
			sqlwhere += "  AND REC_CREATE_TIME <= @e_datetime ";
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT COUNT(1) FROM TTMSM97 WHERE 1 = 1";  //MOLD_TYPE =2表示钢包
			break;
		}

		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlstr_语句[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		/*cmd_inq.Parameters.Set("userid", s_userid);*/
		cmd_inq.Parameters.Set("mold_no", ttmsm97["MOLD_NO"].ToString());
		cmd_inq.Parameters.Set("mold_type", ttmsm97["MOLD_TYPE"].ToString());
		cmd_inq.Parameters.Set("s_datetime", ttmsm97["DRYING_ST"].ToString());
		cmd_inq.Parameters.Set("e_datetime", ttmsm97["DRYING_ET"].ToString());
		rowCount = cmd_inq.ExecuteScalar();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			//sqlstr = "SELECT * FROM TTMSM95 WHERE 1=1 ";

			sqlstr = "SELECT TTMSM97.* FROM TTMSM97 WHERE 1 = 1";  //MOLD_TYPE =2表示钢包
			break;
		}

		sqlstr = sqlstr + sqlwhere;
		sqlwhere = " ORDER BY REC_CREATE_TIME ";
		sqlstr = sqlstr + sqlwhere;
		cmd_inq.SetCommandText(sqlstr);

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


