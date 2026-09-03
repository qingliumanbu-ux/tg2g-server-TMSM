/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      gongnn
Version:     1.0
Date:        2024-03-26
Description: 连铸机设备周期管理弯曲段初始化查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsm191av_inq)


int f_tmsm191av_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString sqlstrinit = "";
	CString sqlstrbefore = "";
	CString sqlstr1 = "";
	CString sqlstrmaxdate = "";
	CString sqlwhere = "";
	CString s_userid("");
	CModel ttmsm191("TTMSM191");

	CString c_orderid("");
	CString sql("");
	CString sap_erp_matnr("");
	CString sampl_entr_no("");
	CString table_name("");
	CString	datetime("");
	CDateTime date_c_cur;
	CString	date_c_maxdate("");
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inqbefore(conn);
	CDbCommand cmd_inqmaxdate(conn);


	EIClass tmp;
	EIClass tmpbefore;
	EIClass tmpmaxdate;
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		//根据查询条件查主记录表
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
				sql += " AND " + bcls_rec->Tables[0].Columns[i].get_ColumnName() + " LIKE @" + bcls_rec->Tables[0].Columns[i].get_ColumnName() + "||'%'";
				cmd_inq.Parameters.Set(bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
				Log::Trace("", __FUNCTION__, "b		= [{0}][{1}]", bcls_rec->Tables[0].Columns[i].get_ColumnName(), bcls_rec->Tables[0].Rows[0][i].ToString().Trim());

			}
		}
		Log::Trace("", __FUNCTION__, table_name);
		sqlstr = " select * from " + table_name + " where 1 = 1";
		sqlstr += sql;
		sqlstr = sqlstr + " order by DATE_C,SEQ_NO asc";
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);

		cmd_inq.Close();
		//如果主记录表当天没有数据，找上一次最近的记录，如果都没有，再查弯曲段初始化基表
		if (bcls_ret->Tables[0].Rows.get_Count() == 0)
		{

			sqlstrmaxdate = " select max(date_c) date_c from " + table_name + " where 1=1";
			cmd_inqmaxdate.SetCommandText(sqlstrmaxdate);
			cmd_inqmaxdate.ExecuteQuery(tmpmaxdate.Tables[0]);

			cmd_inqmaxdate.Close();
			if (tmpmaxdate.Tables[0].Rows.get_Count() > 0)//所有记录中最大日期
			{
				date_c_maxdate = tmpmaxdate.Tables[0].Rows[0]["DATE_C"].ToString().Trim();
				if (date_c_maxdate != "")
				{
					sqlstrbefore = " select * from " + table_name + " where date_c= " + date_c_maxdate + " order by SEQ_NO asc";
				}
				else
				{
					sqlstrbefore = " select * from " + table_name + " where 1 = 1 order by SEQ_NO asc";
				}

			}


			Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstrbefore);
			cmd_inqbefore.SetCommandText(sqlstrbefore);
			cmd_inqbefore.ExecuteQuery(tmpbefore.Tables[0]);

			cmd_inqbefore.Close();
			if (tmpbefore.Tables[0].Rows.get_Count() > 0) //上一次最近记录
			{
				for (int i = 0; i < tmpbefore.Tables[0].Rows.get_Count(); i++)
				{
					bcls_ret->Tables[0].Rows.Add();
					bcls_ret->Tables[0].Rows[i]["SEQ_NO"] = tmpbefore.Tables[0].Rows[i]["SEQ_NO"].ToString();  //序号
					bcls_ret->Tables[0].Rows[i]["SERIES_DESC"] = tmpbefore.Tables[0].Rows[i]["SERIES_DESC"].ToString();  //系列
					bcls_ret->Tables[0].Rows[i]["BEND_NO"] = tmpbefore.Tables[0].Rows[i]["BEND_NO"].ToString();  //弯曲段号
					bcls_ret->Tables[0].Rows[i]["CURRENT_STATUS"] = tmpbefore.Tables[0].Rows[i]["CURRENT_STATUS"].ToString();   //当前状态
					bcls_ret->Tables[0].Rows[i]["LOCATION"] = tmpbefore.Tables[0].Rows[i]["LOCATION"].ToString();  //位置
					bcls_ret->Tables[0].Rows[i]["TOTAL_FURNACE_COUNT"] = tmpbefore.Tables[0].Rows[i]["TOTAL_FURNACE_COUNT"].ToString();   //累计炉数
					bcls_ret->Tables[0].Rows[i]["NOTE"] = tmpbefore.Tables[0].Rows[i]["NOTE"].ToString(); //备注说明

				}

		}
		else
		{
			sqlstrinit = " select * from TTM0002 where 1=1 order by SEQ_NO asc";

			Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstrinit);
			cmd_inq.SetCommandText(sqlstrinit);
			cmd_inq.ExecuteQuery(tmp.Tables[0]);

			cmd_inq.Close();
		
			if (tmp.Tables[0].Rows.get_Count() > 0)
			{
				for (int i = 0; i < tmp.Tables[0].Rows.get_Count(); i++)
				{
					bcls_ret->Tables[0].Rows.Add();
					bcls_ret->Tables[0].Rows[i]["SEQ_NO"] = tmp.Tables[0].Rows[i]["SEQ_NO"].ToString();
					bcls_ret->Tables[0].Rows[i]["SERIES_DESC"] = tmp.Tables[0].Rows[i]["SERIES_DESC"].ToString();
					bcls_ret->Tables[0].Rows[i]["BEND_NO"] = tmp.Tables[0].Rows[i]["BEND_NO"];
				}


			}
		}


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


