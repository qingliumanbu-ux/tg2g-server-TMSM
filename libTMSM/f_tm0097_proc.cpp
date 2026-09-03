/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2111
Version:     1.0
Date:        2023-11-03 
Description: 工器具事件处理
**************************************************/

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <vector>

BM2_FUNCTION_EXPORT


//记录履历

int f_tm0097_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString userid = s.userid;
	/**事件调用类型 1-新增 2-修改 3-删除*/
	CString c_event_call_type("");
	/**事件号*/
	CString c_event_id("");
	/**设备类型*/
	CString c_station_id("");
	/**表名*/
	CString c_table_name("");
	/**主键*/
	CString c_pk("");
	/**sColName 配置事件字段列名*/
	CString sColName = "";
	/**配置事件字段类型            S字符型   D 数值型*/
	CString item_kind = "";
	/**配置事件字段修改类型        1 接口值  2 修改类 (基本都是2)*/
	CString item_upd_type = "";
	/**配置事件字段修改方式        0 不修改  1 接口值  2 固定值(为@则为空或0)  3表中字段  4 特殊处理*/
	CString item_upd_mode = "";
	/**配置事件字段是否允许为空    Y 可为空  N 不可为空*/
	CString item_para_allow_null = "";
	/**配置事件字段固定值          获取该字段的字段设置值*/
	CString item_proc_value = "";
	CString v_fieldstr = "";
	CString c_log_key_tab = "";

	

	/* 实体类定义 */
	CModel ttm0097("TTM0097");
	CModel ttm0099("TTM0099");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDataTable dt_temp;

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		
		c_event_id = bcls_rec->Tables["TM0096"].Rows[0]["EVENT_ID"].ToString();
		c_station_id = bcls_rec->Tables["TM0096"].Rows[0]["STATION_ID"].ToString();
		sqlstr = " select TM1.EVENT_CALL_TYPE_CODE, TM0.TABLE_NAME,TM0.LOG_KEY_VALUE1\
			from ttm0097 TM1,\
			TTM0000 TM0\
		where TM1.STATION_ID = TM0.STATION_ID\
		AND EVENT_ID = '"+ c_event_id +"'\
			AND TM1.STATION_ID = '"+ c_station_id +"'  ";
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read()) {
			c_event_call_type = cmd_inq.GetString(1);
			c_table_name = cmd_inq.GetString(2);
			c_log_key_tab= cmd_inq.GetString(3);
			Log::Trace("", __FUNCTION__, "c_event_call_type[{0}]c_table_name[{1}]c_log_key_tab[{2}]", c_event_call_type, c_table_name, c_log_key_tab);
		}
		else {
			sprintf(s.msg, "未配置事件，请联系系统人员，前往工器具跟踪事件管理[TM0097]配置！");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			sqlstr = "SELECT  LISTAGG(COLUMN_NAME,',') FROM "
				" (SELECT DISTINCT T2.COLNAME AS COLUMN_NAME FROM SYSCAT.TABCONST T1, SYSCAT.KEYCOLUSE T2 "
				"  WHERE T1.CONSTNAME = T2.CONSTNAME AND T1.TYPE = 'P' AND T1.TABNAME = '" + c_table_name + "')";
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = " SELECT LISTAGG(COLUMN_NAME, ',') "
				" FROM(SELECT DISTINCT T2.COLUMN_NAME AS COLUMN_NAME FROM USER_CONSTRAINTS T1, USER_CONS_COLUMNS T2 "
					" WHERE T1.CONSTRAINT_NAME = T2.CONSTRAINT_NAME AND T1.CONSTRAINT_TYPE = 'P' AND T1.TABLE_NAME = '" + c_table_name + "')";
		default:
			sqlstr = " SELECT LISTAGG(COLUMN_NAME, ',') "
				" FROM(SELECT DISTINCT T2.COLUMN_NAME AS COLUMN_NAME FROM USER_CONSTRAINTS T1, USER_CONS_COLUMNS T2 "
				" WHERE T1.CONSTRAINT_NAME = T2.CONSTRAINT_NAME AND T1.CONSTRAINT_TYPE = 'P' AND T1.TABLE_NAME = '" + c_table_name + "')";
			break;
		}
		Log::Trace("", "", "sqlstr ={0}", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		
		if (cmd_inq.Read())
		{
			c_pk = cmd_inq.GetString(1);
			Log::Trace("", __FUNCTION__, "v_pk = [{0}]", c_pk);
		}
		cmd_inq.Close();
		CModel ttm00aa(c_table_name);
		Log::Trace("", "", "c_table_name ={0}", c_table_name);
		ttm00aa.MergeFrom(bcls_rec->Tables["TM0096"].Rows[0]);
		
		if (c_event_call_type == "1")//新增事件
		{
			//新增事件-配置主键
			
				/*std::string col_name = cmd_inq.GetString(2);
				std::stringstream ss(col_name);
				std::string token;
				while (std::getline(ss, token, ',')) {
					Log::Trace("", __FUNCTION__, "token[{0}]", CString(token));
					if (!bcls_rec->Tables["TM0096"].Columns.Contains(CString(token))) {
						sprintf(s.msg, "未配置数据项[%s]，请联系系统人员，前往工器具跟踪事件管理[TM0097]配置！", (const char*)CString(token));
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}*/
				
				
				
				ttm00aa["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				ttm00aa["REC_CREATOR"] = s.userid;
				if (!ttm00aa.Insert()) {
					sprintf(s.msg, "新增失败！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			
		}
		else if (c_event_call_type == "2")//修改事件
		{
			//根据传入结构体(ttm00)主键值校验数据库表中是否有数据且是否唯一，有唯一数据则获取数据库表中数据并存入另一结构体(ttmsm)
			CModel ttm00aa_old(c_table_name);
			
			ttm00aa_old.Reset();
			ttm00aa_old.MergeFrom(bcls_rec->Tables["TM0096"].Rows[0]);
			
			if (ttm00aa_old.QueryCount(c_pk) == 0)
			{
				strcpy(s.msg, "根据主键查询没有数据无法更新");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else if (ttm00aa_old.QueryCount(c_pk) > 1)
			{
				strcpy(s.msg, "根据主键查询数据不唯一无法更新");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else
			{
				ttm00aa_old.Query(c_pk);
			}

			if (bcls_ret->Tables.Contains("TTM0099") == false)
			{
				bcls_ret->Tables.Add("TTM0099");
			}

			sqlstr = "SELECT * FROM TTM0099 WHERE 1=1 AND EVENT_ID ='" + c_event_id + "' AND STATION_ID ='" + c_station_id + "'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables["TTM0099"]);
			cmd_inq.Close();

			if (bcls_ret->Tables["TTM0099"].Rows.get_Count() == 0)
			{
				strcpy(s.msg, "没找到事件配置字段");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			

			for (int i = 0; i < bcls_ret->Tables["TTM0099"].Rows.get_Count(); i++)
			{
				sColName = bcls_ret->Tables["TTM0099"].Rows[i]["ITEM_ENAME"].ToString().Trim().ToUpper();
				item_kind = bcls_ret->Tables["TTM0099"].Rows[i]["ITEM_TYPE"].ToString().Trim();
				item_upd_type = bcls_ret->Tables["TTM0099"].Rows[i]["ITEM_UPD_TYPE"].ToString().Trim();
				item_para_allow_null = bcls_ret->Tables["TTM0099"].Rows[i]["ITEM_PARA_ALLOW_NULL"].ToString().Trim();
				item_upd_mode = bcls_ret->Tables["TTM0099"].Rows[i]["ITEM_UPD_MODE"].ToString().Trim();
				item_proc_value = bcls_ret->Tables["TTM0099"].Rows[i]["ITEM_PROC_VALUE"].ToString().Trim();

				//传入结构体含有事件中的字段名并且是修改类
				if (ttm00aa.GetFields().Contains(sColName) && item_upd_type == "2")
				{
					//配置事件字段不允许为空
					if (item_para_allow_null == "N")
					{
						//接口参数不可为空
						if (item_kind == "S")	//S-字符型
						{
							if (ttm00aa[sColName].ToString().Trim() == "")
							{
								strcpy(s.msg, "传入参数校验错误!字段[" + sColName + "]不允许为空!");
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
						else	//D-数值型
						{
							if (ttm00aa[sColName].ToDecimal() == 0)
							{
								strcpy(s.msg, "传入参数校验错误!字段[" + sColName + "]不允许为0!");
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}

						
					}
					if (true)
					{
						if (item_upd_mode == "0")//不修改
						{
							continue;
						}
						else if (item_upd_mode == "1")//接口值
						{
							if (item_kind == "S")	//S-字符型
							{
								if (ttm00aa[sColName].ToString().Trim() == ttm00aa_old[sColName].ToString().Trim())
								{
									Log::Trace("", __FUNCTION__, "字段[{0}]不修改", sColName);
									continue;
								}
								
							}
							else
							{
								if (ttm00aa[sColName].ToDecimal() == ttm00aa_old[sColName].ToDecimal())
								{
									Log::Trace("", __FUNCTION__, "字段[{0}]不修改", sColName);
									continue;
								}
								
							}
						}
						else if (item_upd_mode == "2")//固定值
						{
							if (item_proc_value == "")
							{
								strcpy(s.msg, "TTM0099表中字段[" + sColName + "]字段修改方式是固定值,字段设置值[" + item_proc_value + "]必须有值!");
								throw CApplicationException(-1, s.msg, log.Location);
							}

							if (item_proc_value == "@")		//固定值为空
							{
								if (item_kind == "S")	//S-字符型
								{
									ttm00aa[sColName] = "";
								}
								else
								{
									ttm00aa[sColName] = 0;
								}
							}
							else if (item_proc_value == "s.userid")		//固定值为登录者
							{
								ttm00aa[sColName] = s.userid;
							}
							else if (item_proc_value == "s.datetime")		//固定值为系统时刻 14位
							{
								ttm00aa[sColName] = CDateTime::Now().ToString("yyyyMMddHHmmss");
							}
							else
							{
								ttm00aa[sColName] = item_proc_value;
							}
						}
						else if (item_upd_mode == "3") //表中字段
						{
							if (item_proc_value == "")
							{
								strcpy(s.msg, "TTM0099表中字段[" + sColName + "]字段修改方式是表中字段,字段设置值[" + item_proc_value + "]必须有值!");
								throw CApplicationException(-1, s.msg, log.Location);
							}

							if (!ttm00aa.GetFields().Contains(item_proc_value))
							{
								strcpy(s.msg, "TTM0099表中字段[" + sColName + "]字段修改方式是表中字段,字段设置值[" + item_proc_value + "]必须为传入表结构中的字段!");
								throw CApplicationException(-1, s.msg, log.Location);
							}

							if (item_kind == "S")	//S-字符型
							{
								if (ttm00aa[sColName].ToString().Trim() == ttm00aa_old[item_proc_value].ToString().Trim())
								{
									Log::Trace("", __FUNCTION__, "字段[{0}]不修改", sColName);
									continue;
								}
								ttm00aa[sColName] = ttm00aa_old[item_proc_value].ToString().Trim();
							}
							else
							{
								if (ttm00aa[sColName].ToDecimal() == ttm00aa_old[item_proc_value].ToDecimal())
								{
									Log::Trace("", __FUNCTION__, "字段[{0}]不修改", sColName);
									continue;
								}
								ttm00aa[sColName] = ttm00aa_old[item_proc_value].ToDecimal();
							}
						}
						else if (item_upd_mode == "4") //特殊处理
						{

						}
					}

					v_fieldstr = v_fieldstr + sColName + ",";


				}

			}

			if (v_fieldstr.Trim().GetLength() > 1)
			{
				if (v_fieldstr.Find("REC_REVISE_TIME") == string::npos)v_fieldstr = v_fieldstr + "REC_REVISE_TIME,";
				if (v_fieldstr.Find("REC_REVISOR") == string::npos)v_fieldstr = v_fieldstr + "REC_REVISOR,";

				ttm00aa["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				ttm00aa["REC_REVISOR"] = s.userid;

				v_fieldstr = v_fieldstr.SubstringNE(0, v_fieldstr.GetLength() - 1);
				Log::Trace("", __FUNCTION__, "v_fieldstr = [{0}]", v_fieldstr);
				ttm00aa.Update(v_fieldstr, c_pk);
			}
		}
		else if (c_event_call_type=="3")
		{
		ttm00aa.Delete(c_pk);
		}

		//获取对应履历表结构体(htmsm) 
		CModel tm0a(c_log_key_tab);

		tm0a.Reset();
		tm0a.CopyFrom(ttm00aa);
		tm0a["FORE_IP"] = s.fore_ip;   //IP地址
		tm0a["FORM_NAME"] = s.formname;   //画面名
		tm0a["RESUME_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmssff6");   //履历
		tm0a["FUNC_ID"] = s.svc_name;   //服务名
		tm0a["FORE_MAC"] = s.fore_mac; //MAC地址
		tm0a["FORE_MACHINE"] = s.fore_machine; //电脑名称
		tm0a.Insert();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace((1,1, "[%s]", s.sysmsg);
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


