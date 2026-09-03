/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-19 14:24:28
Description: 工器具履历查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme95_inq1)

int f_tmsm11_60110(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tmsme95_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWMA7 twma7(conn);
	CModel ttmsm91 = CModel("TTMSM91");
	CModel ttmsm95 = CModel("TTMSM95");

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


		ttmsm95.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Trace("", __FUNCTION__, "ttmsm91.DEV_NAME	= [{0}]", (const char*)ttmsm95["DEV_NO"].ToString());

		if (ttmsm95["SM_UNIT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND SM_UNIT_NO = @sm_unti_no ";
		}
		if (ttmsm95["WORK_AREA"].ToString().Trim() != "" && ttmsm95["WORK_AREA"].ToString().Trim() != "Q")
		{
			sqlwhere += " AND WORK_AREA = @work_area ";
		}
		if (ttmsm95["DEV_NO"].ToString().Trim() != "")
		{
			sqlwhere += "  AND DEV_NAME = @dev_no ";
		}
		if (ttmsm95["DEV_TYPE"].ToString().Trim() != "")
		{
			sqlwhere += "  AND DEV_TYPE = @dev_type ";
		}
		if (ttmsm95["S_DATETIME"].ToString().Trim() != "")
		{
			sqlwhere += "  AND OPERATE_TIME >= @s_datetime ";
		}
		if (ttmsm95["E_DATETIME"].ToString().Trim() != "")
		{
			sqlwhere += "  AND OPERATE_TIME <= @e_datetime ";
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT COUNT(1) FROM TTMSM95 WHERE 1=1 ";
			break;
		}

		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlstr_语句[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		/*cmd_inq.Parameters.Set("userid", s_userid);*/
		cmd_inq.Parameters.Set("sm_unti_no", ttmsm95["SM_UNIT_NO"].ToString());
		cmd_inq.Parameters.Set("work_area", ttmsm95["WORK_AREA"].ToString());
		cmd_inq.Parameters.Set("dev_no", ttmsm95["DEV_NO"].ToString());
		cmd_inq.Parameters.Set("dev_type", ttmsm95["DEV_TYPE"].ToString());
		cmd_inq.Parameters.Set("s_datetime", ttmsm95["S_DATETIME"].ToString());
		cmd_inq.Parameters.Set("e_datetime", ttmsm95["E_DATETIME"].ToString());
		rowCount = cmd_inq.ExecuteScalar();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			//sqlstr = "SELECT * FROM TTMSM95 WHERE 1=1 ";

			sqlstr = "SELECT TTMSM95.*,(select ttmsm61.dev_name from ttmsm61 where ttmsm61.dev_type = TTMSM95.dev_type) TYPE_NM FROM TTMSM95 WHERE 1=1";
			break;
		}

		if (ttmsm95["SM_UNIT_NO"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + "  AND TTMSM95.SM_UNIT_NO = @SM_UNIT_NO ";
		}
		if (ttmsm95["WORK_AREA"].ToString().Trim() != "" && ttmsm95["WORK_AREA"].ToString().Trim() != "Q")
		{
			sqlstr = sqlstr + "  AND TTMSM95.WORK_AREA = @WORK_AREA ";
		}
		if (ttmsm95["DEV_NO"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + "  AND TTMSM95.DEV_NAME = @DEV_NO ";
		}
		if (ttmsm95["DEV_TYPE"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + "  AND TTMSM95.DEV_TYPE = @DEV_TYPE ";
		}
		if (ttmsm95["S_DATETIME"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + "  AND TTMSM95.OPERATE_TIME >= @S_DATETIME ";
		}
		if (ttmsm95["E_DATETIME"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + "  AND TTMSM95.OPERATE_TIME <= @E_DATETIME ";
		}

		sqlwhere = " ORDER BY DEV_NO ";
		sqlstr = sqlstr + sqlwhere;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("WORK_AREA", ttmsm95["WORK_AREA"].ToString());
		cmd_inq.Parameters.Set("SM_UNIT_NO", ttmsm95["SM_UNIT_NO"].ToString());
		cmd_inq.Parameters.Set("DEV_NO", ttmsm95["DEV_NO"].ToString());
		cmd_inq.Parameters.Set("DEV_TYPE", ttmsm95["DEV_TYPE"].ToString());
		cmd_inq.Parameters.Set("S_DATETIME", ttmsm95["S_DATETIME"].ToString());
		cmd_inq.Parameters.Set("E_DATETIME", ttmsm95["E_DATETIME"].ToString());
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

		//抛其它工器具跟踪履历

			bcls_rec->Tables.Add();
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "p_moltiron_tank_no");
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "p_case");


		bcls_rec->Tables[0].Rows.Clear();

		//其它工器具跟踪履历跟踪函数
		bcls_rec->Tables[0].Rows.Add();
		bcls_rec->Tables[0].Rows[0]["p_moltiron_tank_no"] = "111";				//用67表记录其它工器具履历
		bcls_rec->Tables[0].Rows[0]["p_case"] = "19";  //时间操作

		doFlag = f_tmsm11_60110(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
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


