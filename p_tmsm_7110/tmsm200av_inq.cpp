/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      gongnn
Version:     1.0
Date:        2024-11-12
Description: 设备故障调度记录查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsm200av_inq)


int f_tmsm200av_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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

	CString c_orderid("");
	CString sap_erp_matnr("");
	CString sampl_entr_no("");
	CString table_name("");
	//CModel ttmsm200("TTMSM200");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		/*
		for (int i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
		{
		if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_STRING
		&& bcls_rec->Tables[0].Rows[0][i].ToString().Trim().IsEmpty())
		{
		continue;
		}
		if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_DECIMAL
		&& bcls_rec->Tables[0].Rows[0][i].ToDecimal() == 0)
		{
		continue;
		}

		if (bcls_rec->Tables[0].Columns[i].get_ColumnName() == "TABLE_NAME")
		{
		table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString();
		}


		else
		{
		sql += " AND " + bcls_rec->Tables[0].Columns[i].get_ColumnName() + " LIKE @" + bcls_rec->Tables[0].Columns[i].get_ColumnName() + "||'%' ";
		cmd_inq.Parameters.Set(bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
		Log::Trace("", __FUNCTION__, "b		= [{0}][{1}]", bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
		}
		}
		*/
		//查询条件改为日期范围
		for (int i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
		{
			get_columnname = bcls_rec->Tables[0].Columns[i].get_ColumnName();

			if (bcls_rec->Tables[0].Columns[i].get_ColumnName() == "TABLE_NAME")
			{
				table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString();
			}


			if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_STRING
				&& bcls_rec->Tables[0].Rows[0][i].ToString().Trim().IsEmpty())
			{
				continue;
			}

			if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_STRING
				&& get_columnname.Trim() == "TABLE_NAME")
			{
				continue;
			}
			if (get_columnname == "WORK_AREA")
			{
				continue;
			}

			if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_DECIMAL
				&& bcls_rec->Tables[0].Rows[0][i].ToDecimal() == 0)
			{
				continue;
			}

			if (bcls_rec->Tables[0].Columns[i].get_DataType() == DT_DATETIME
				&&  bcls_rec->Tables[0].Rows[0][i].ToString().Trim().IsEmpty())
			{
				continue;
			}



			Log::Trace("", __FUNCTION__, "ABC[{0}]", get_columnname.SubstringNE(get_columnname.GetLength() - 4, get_columnname.GetLength()));
			if (get_columnname.SubstringNE(get_columnname.GetLength() - 4, get_columnname.GetLength()) == "FROM")
			{

				sql += " AND " + bcls_rec->Tables[0].Columns[i].get_ColumnName().Replace("_FROM", "") + ">= @" + bcls_rec->Tables[0].Columns[i].get_ColumnName();
				cmd_inq.Parameters.Set(bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
				Log::Trace("", __FUNCTION__, "AAAAA[{0}]", get_columnname.SubstringNE(get_columnname.GetLength() - 4, get_columnname.GetLength()));
			}
			else if (get_columnname.SubstringNE(get_columnname.GetLength() - 2, get_columnname.GetLength()) == "TO")
			{
				sql += " AND " + bcls_rec->Tables[0].Columns[i].get_ColumnName().Replace("_TO", "") + "<= @" + bcls_rec->Tables[0].Columns[i].get_ColumnName();
				cmd_inq.Parameters.Set(bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
				Log::Trace("", __FUNCTION__, "BBBBB[{0}]", get_columnname.SubstringNE(get_columnname.GetLength() - 2, get_columnname.GetLength()));
			}
			else
			{


				Log::Trace("", __FUNCTION__, "ABC[{0}]", bcls_rec->Tables[0].Columns[i].get_ColumnName().Replace("_FROM", ""));

				Log::Trace("", __FUNCTION__, "b		= [{0}][{1}]", bcls_rec->Tables[0].Columns[i].get_ColumnName().SubstringNE(-1, -4), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
				sql += " AND " + bcls_rec->Tables[0].Columns[i].get_ColumnName() + " LIKE '%'||" + "@" + bcls_rec->Tables[0].Columns[i].get_ColumnName() + "||'%' ";
				cmd_inq.Parameters.Set(bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());

			}



		}
		bcls_rec->Tables[0].Rows[0]["WORK_AREA"] = bcls_rec->Tables[0].Rows[0]["WORK_AREA"].ToString().Replace(",", "','");
		if (bcls_rec->Tables[0].Rows[0]["WORK_AREA"].ToString().Trim() != "")
		{
			sql += " AND WORK_AREA IN ('" + bcls_rec->Tables[0].Rows[0]["WORK_AREA"].ToString() + "') ";
		}

		Log::Trace("", __FUNCTION__, table_name);
		sqlstr = " select * from " + table_name + " where 1 = 1";
		sqlstr += sql;
		sqlstr = sqlstr + " order by SEQ_NO desc";
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
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


