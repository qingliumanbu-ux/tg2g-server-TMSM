/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-10 19:30:47
Description: 工器具基本信息查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme91_inq1)


int f_tmsme91_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

		//获取库区授权
		//CString stock_no_auth = "' '";
		//EIClass *bcls_auth = new EIClass;
		//if (f_epes_get_auth_other(s.userid, 5, bcls_auth, conn) != 0)
		//{
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//for (int fetchRowCount = 0; fetchRowCount < bcls_auth->Tables[0].Rows.get_Count(); fetchRowCount++)
		//{
		//	stock_no_auth += ", '" + bcls_auth->Tables[0].Rows[fetchRowCount]["name"].ToString() + "' ";
		//}
		//delete bcls_auth;

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


		ttmsm91.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (ttmsm91["SM_UNIT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND SM_UNIT_NO = @sm_unti_no ";
		}
		if (ttmsm91["WORK_AREA"].ToString().Trim() != "" && ttmsm91["WORK_AREA"].ToString().Trim() != "Q")
		{
			sqlwhere += " AND WORK_AREA = @work_area ";
		}
		if (ttmsm91["DEV_NO"].ToString().Trim() != "")
		{
			sqlwhere += "  AND DEV_NAME = @dev_no ";
		}
		if (ttmsm91["DEV_TYPE"].ToString().Trim() != "")
		{
			sqlwhere += "  AND DEV_TYPE = @dev_type ";
		}
	
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT COUNT(1) FROM TTMSM91 WHERE 1=1 ";
			break;
		}
		//sqlwhere += " AND STOCK_NO IN"
		//	"( select STOCK_NO"
		//	" from   twm0a "
		//	" where   groupid in ( "
		//	" select groupid from TESGROUPMEMBER "
		//	"  where memberid in ( "
		//	" select id from tesuserinfo where @userid = @userid "
		//	" ) "
		//	"  ) "
		//	" union "
		//	" select STOCK_NO from twm01 where @userid = @userid "
		//	"  ) ";

		//sqlwhere +=
		//	" AND STOCK_NO IN (" + stock_no_auth + ")";

		//sqlwhere +=
		//	" AND EXISTS (SELECT NULL FROM TWM01"
		//	" WHERE TWMA7.STOCK_NO = TWM01.STOCK_NO"
		//	" AND TWM01.MAT_LINE_TYPE= 'SM'"
		//	" AND TWM01.MAT_KIND = 'SM')";


		sqlstr = sqlstr + sqlwhere;
		Log::Trace("", __FUNCTION__, "sqlstr_语句[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		/*cmd_inq.Parameters.Set("userid", s_userid);*/
		cmd_inq.Parameters.Set("sm_unti_no", ttmsm91["SM_UNIT_NO"].ToString());
		cmd_inq.Parameters.Set("work_area", ttmsm91["WORK_AREA"].ToString());
		cmd_inq.Parameters.Set("dev_no", ttmsm91["DEV_NO"].ToString());
		cmd_inq.Parameters.Set("dev_type", ttmsm91["DEV_TYPE"].ToString());
		rowCount = cmd_inq.ExecuteScalar();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			//sqlstr = "SELECT * FROM TTMSM91 WHERE 1=1 ";
			
				sqlstr = "SELECT TTMSM91.*,(select ttmsm61.dev_name from ttmsm61 where ttmsm61.dev_type = TTMSM91.dev_type) TYPE_NM FROM TTMSM91 WHERE 1=1";
			break;
		}

		if (ttmsm91["SM_UNIT_NO"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + "  AND TTMSM91.SM_UNIT_NO = @SM_UNIT_NO ";
		}
		if (ttmsm91["WORK_AREA"].ToString().Trim() != "" && ttmsm91["WORK_AREA"].ToString().Trim() != "Q")
		{
			sqlstr = sqlstr + "  AND TTMSM91.WORK_AREA = @WORK_AREA ";
		}
		if (ttmsm91["DEV_NO"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + "  AND TTMSM91.DEV_NAME = @DEV_NO ";
		}
		if (ttmsm91["DEV_TYPE"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + "  AND TTMSM91.DEV_TYPE = @DEV_TYPE ";
		}

		//sqlwhere += " ORDER BY DEV_NO ";
		sqlwhere = " ORDER BY DEV_NO ";
		sqlstr = sqlstr + sqlwhere;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("WORK_AREA", ttmsm91["WORK_AREA"].ToString());
		cmd_inq.Parameters.Set("SM_UNIT_NO", ttmsm91["SM_UNIT_NO"].ToString());
		cmd_inq.Parameters.Set("DEV_NO", ttmsm91["DEV_NO"].ToString());
		cmd_inq.Parameters.Set("DEV_TYPE", ttmsm91["DEV_TYPE"].ToString());
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

			//ttmsm91.Reset();
			//cmd_inq.Fetch(ttmsm91);
			//ttmsm91.MergeTo(bcls_ret->Tables[0], false);


			//if (bcls_ret->Tables[0].Columns.Contains("SG_SIGN") == false)
			//{
			//	bcls_ret->Tables[0].Columns.Add(DT_STRING, "SG_SIGN");
			//}
			//if (bcls_ret->Tables[0].Columns.Contains("ORDER_NO") == false)
			//{
			//	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ORDER_NO");
			//}

			//sqlstr1 = " SELECT  SG_SIGN, ORDER_NO      "
			//	" FROM TMMSM01 WHERE MAT_NO =@MAT_NO ";
			//cmd_con.Parameters.Clear();
			//cmd_con.Parameters.Set("MAT_NO", twma7["MAT_NO"].ToString().Trim());
			//cmd_con.SetCommandText(sqlstr1);
			//cmd_con.ExecuteReader();
			//if (cmd_con.Read())
			//{
			//	bcls_ret->Tables[0].Rows[fetchRowCount - 1]["SG_SIGN"] = cmd_con.GetString(1);
			//	bcls_ret->Tables[0].Rows[fetchRowCount - 1]["ORDER_NO"] = cmd_con.GetString(2);
			//}
			//cmd_con.Close();

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