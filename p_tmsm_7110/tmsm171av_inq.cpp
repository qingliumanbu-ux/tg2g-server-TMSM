/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      gongnn
Version:     1.0
Date:        2024-03-01
Description: 连铸机设备周期管理扇形段导入记录查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsm171av_inq)


int f_tmsm171av_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
		/*
		sqlstr = " select *\
		from TTMSM171 where 1=1";
		*/
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
		sqlstr = sqlstr + " ORDER BY DATE_C,SEQ_NO asc ";
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();




		//如果主记录表当天没有数据，找上一次最近的记录，如果都没有，再查扇形段初始化基表
		if (bcls_ret->Tables[0].Rows.get_Count() == 0) //主记录当天没有数据
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
					bcls_ret->Tables[0].Rows[i]["CCMACH_DESC"] = tmpbefore.Tables[0].Rows[i]["CCMACH_DESC"].ToString();  //铸机描述
					bcls_ret->Tables[0].Rows[i]["LOCATION"] = tmpbefore.Tables[0].Rows[i]["LOCATION"].ToString();   //位置
					bcls_ret->Tables[0].Rows[i]["SD_NO"] = tmpbefore.Tables[0].Rows[i]["SD_NO"].ToString();  //扇形段号
					bcls_ret->Tables[0].Rows[i]["ON_LINE_TIME"] = tmpbefore.Tables[0].Rows[i]["ON_LINE_TIME"].ToString();  //上线时间
					bcls_ret->Tables[0].Rows[i]["MON_NUM"] = tmpbefore.Tables[0].Rows[i]["MON_NUM"].ToString();   //周期月
					bcls_ret->Tables[0].Rows[i]["END_TIME"] = tmpbefore.Tables[0].Rows[i]["END_TIME"].ToString(); //到期时间
					bcls_ret->Tables[0].Rows[i]["DAYS"] = tmpbefore.Tables[0].Rows[i]["DAYS"].ToString(); //剩余天数
				}

			}
			else   
			{
				sqlstrinit = "select * from TTM0003 where 1=1 order by SEQ_NO asc"; //扇形段初始化基表

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
						bcls_ret->Tables[0].Rows[i]["CCMACH_DESC"] = tmp.Tables[0].Rows[i]["CCMACH_DESC"].ToString();
						bcls_ret->Tables[0].Rows[i]["SD_NO"] = tmp.Tables[0].Rows[i]["SD_NO"].ToString();
						bcls_ret->Tables[0].Rows[i]["LOCATION"] = tmp.Tables[0].Rows[i]["LOCATION"].ToString();
						bcls_ret->Tables[0].Rows[i]["MON_NUM"] = tmp.Tables[0].Rows[i]["MON_NUM"].ToString();
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


