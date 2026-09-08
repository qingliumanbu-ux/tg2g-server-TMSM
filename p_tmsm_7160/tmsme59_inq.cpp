/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-30 16:40:45
Description: 行车作业率查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme59_inq)


int f_tmsme59_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal days_diffx = 0;
	CDecimal hours_diffx = 0;
	CDecimal minutes_diffx = 0;
	CDecimal seconds_diffx = 0;
	CDecimal time_diff = 0;

	CString b_time = " ";
	CString e_time = " ";
	CString pre_leave_time = " ";

	CDecimal all_time = 0.00;
	CDecimal use_time = 0.00;
	CDecimal prep_time = 0.00;
	CDecimal free_time = 0.00;
	CDecimal repair_time = 0.00;	
	CDecimal good_rate = 0.00;
	CDecimal good_rate1 = 0.00;

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWMA7 twma7(conn);
	CModel ttmsm66 = CModel("TTMSM66");
	CModel ttmsm67 = CModel("TTMSM66");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString s_userid("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDataTable dt_dev;
	CDataTable dt_temp;

	try
	{
		// 获取前台传入参数
		s_userid = s.userid;

		ttmsm66.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Trace("", __FUNCTION__, "ttmsm66.DEV_NO	= [{0}]", (const char*)ttmsm66["DEV_NO"].ToString());

		if (ttmsm66["DEV_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND DEV_NO = @dev_no ";
		}
		if (ttmsm66["REC_CREATE_TIME"].ToString().Trim() != "")
		{
			sqlwhere += "  AND CHANGE_TIME >= @begin_time ";
		}
		if (ttmsm66["CHANGE_TIME"].ToString().Trim() != "")
		{
			sqlwhere += "  AND CHANGE_TIME <= @end_time ";
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			// DM8 适配 CHANGE-01：按日历日期计算结束时间减开始时间的天数。
			// 改写原因：DAYS/SYSIBM.SYSDUMMY1 改为 DATEDIFF/DUAL；HR-001 确认全库时间格式为 YYYYMMDDHH24MISS,用 TO_TIMESTAMP 显式指定。
			// 本共用分支面向 DM8，其他 DB_KIND 标签也会执行此 SQL。
			// 原方案A（完整保留）：
			// sqlstr = "select DAYS(DATE(TIMESTAMP ('" + ttmsm66["CHANGE_TIME"].ToString() + "'))) - DAYS(DATE(TIMESTAMP ('" + ttmsm66["REC_CREATE_TIME"].ToString() + "'))) from SYSIBM.SYSDUMMY1";
			// 方案B（DM8）：
			sqlstr = "select DATEDIFF(DAY, TO_TIMESTAMP('" + ttmsm66["REC_CREATE_TIME"].ToString() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + ttmsm66["CHANGE_TIME"].ToString() + "','YYYYMMDDHH24MISS')) from DUAL";
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.SetCommandText(sqlstr);

		Log::Trace("", "", "时间差天数语句 sqlstr：{0}", sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			days_diffx = cmd_inq.GetDecimal(1);
		}
		else
		{
			days_diffx = 0;
		}
		cmd_inq.Close();
		Log::Trace("", "", "时间差天数计算值 days_diffx：{0}", days_diffx.ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			// DM8 适配 CHANGE-EXEC-097：小时差:从 REC_CREATE_TIME 到 CHANGE_TIME 实际经过的完整小时数。
			// 改写原因:DB2 两参数 TIMESTAMPDIFF(8=小时) 改为 DM8 标准写法(实际完整时长)。
			// 口径:HR-002 已确认选①实际完整时长——不满 1 个单位不算,与 DB2 原行为一致。
			// 改写公式:DB2 TIMESTAMPDIFF(8, TS1-TS2) → DM8 DATEDIFF(SECOND, 起点, 终点) / 3600。
			//   整数除法自动舍去不满整数的部分,与 DB2 TIMESTAMPDIFF 行为一致。
			// 时间格式:HR-001 确认全库时间为 YYYYMMDDHH24MISS(14位紧凑),用 TO_TIMESTAMP 显式指定。
			// 原 SQL(完整保留):
			// sqlstr = "select TIMESTAMPDIFF(8, CHAR(TIMESTAMP('" + ttmsm66["CHANGE_TIME"].ToString() + "')  - TIMESTAMP('" + ttmsm66["REC_CREATE_TIME"].ToString() + "'))) from SYSIBM.SYSDUMMY1";
			// DM8 SQL:
			sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + ttmsm66["REC_CREATE_TIME"].ToString() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + ttmsm66["CHANGE_TIME"].ToString() + "','YYYYMMDDHH24MISS')) / 3600 from DUAL";
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.SetCommandText(sqlstr);

		Log::Trace("", "", "时间差小时语句 sqlstr：{0}", sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			hours_diffx = cmd_inq.GetDecimal(1);
		}
		else
		{
			hours_diffx = 0;
		}
		cmd_inq.Close();
		Log::Trace("", "", "小时差计算值 days_diffx：{0}", hours_diffx.ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			// DM8 适配 CHANGE-EXEC-098：分钟差:从 REC_CREATE_TIME 到 CHANGE_TIME 实际经过的完整分钟数。
			// 改写原因:DB2 两参数 TIMESTAMPDIFF(4=分钟) 改为 DM8 标准写法(实际完整时长)。
			// 口径:HR-002 已确认选①实际完整时长——不满 1 个单位不算,与 DB2 原行为一致。
			// 改写公式:DB2 TIMESTAMPDIFF(4, TS1-TS2) → DM8 DATEDIFF(SECOND, 起点, 终点) / 60。
			//   整数除法自动舍去不满整数的部分,与 DB2 TIMESTAMPDIFF 行为一致。
			// 时间格式:HR-001 确认全库时间为 YYYYMMDDHH24MISS(14位紧凑),用 TO_TIMESTAMP 显式指定。
			// 原 SQL(完整保留):
			// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + ttmsm66["CHANGE_TIME"].ToString() + "')  - TIMESTAMP('" + ttmsm66["REC_CREATE_TIME"].ToString() + "'))) from SYSIBM.SYSDUMMY1";
			// DM8 SQL:
			sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + ttmsm66["REC_CREATE_TIME"].ToString() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + ttmsm66["CHANGE_TIME"].ToString() + "','YYYYMMDDHH24MISS')) / 60 from DUAL";
			//sqlstr = "select timestampdiff(8, char(timestamp(sysdate) - timestamp('2023-05-25 10:14:01'))) AS "间隔分钟" from ttmsm66
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.SetCommandText(sqlstr);

		Log::Trace("", "", "时间差分钟语句 sqlstr：{0}", sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			minutes_diffx = cmd_inq.GetDecimal(1);
		}
		else
		{
			minutes_diffx = 0;
		}
		cmd_inq.Close();
		Log::Trace("", "", "分钟差计算值 days_diffx：{0}", hours_diffx.ToString());

		all_time = all_time + minutes_diffx;

		//----------实际行车使用率查询-------------------------------
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "DEV_NO");		//行车		
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "REMARK_1");    //使用时间
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "DEV_NAME");     //到位时间
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "REMARK_2");    //空闲时间
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "REMARK_3");    //维修时间
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "REMARK_4");    //使用率
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REMARK_6");     //运行率（+到位时间）

		sqlstr = " SELECT * FROM TTMSM52 ORDER BY CRANENO ";

		//Log::Trace("", "", "sqlstr：{0}", sqlstr);
		Db::QueryTable(sqlstr, dt_dev);
		//Log::Trace("", "", "Count：{0}", dt_dev.Rows.get_Count());
		for (int i = 0; i < dt_dev.Rows.get_Count(); i++)
		{
			use_time = 0;
			prep_time = 0;
			free_time = 0;
			repair_time = 0;
			/* *****  ***** */

				sqlstr1 = " SELECT SUSPEND_TIME, D_TIME	FROM TTMSM54 WHERE CRANENO = '" + dt_dev.Rows[i]["CRANENO"].ToString() + "'"
					" AND SUSPEND_TIME BETWEEN '" + ttmsm66["REC_CREATE_TIME"].ToString() + "' AND '" + ttmsm66["CHANGE_TIME"].ToString().Trim() + "'";
				Db::QueryTable(sqlstr1, dt_temp);
				Log::Trace("", "", "使用语句 sqlstr：{0}", sqlstr1);
				for (int j = 0; j < dt_temp.Rows.get_Count(); j++)
				{
					//到位时间
					if (pre_leave_time.Trim().GetLength() > 0 && dt_temp.Rows[j]["SUSPEND_TIME"].ToString().Trim().GetLength() > 0)
					{
						// DM8 适配 CHANGE-EXEC-099:暂停时间差分钟数:从 DEV_NAME(存暂停开始时间)到 SUSPEND_TIME 实际经过的完整分钟数。
						// 改写原因:DB2 两参数 TIMESTAMPDIFF(4=分钟) 改为 DM8 标准写法(实际完整时长)。口径同 HR-002 选①。
						// 改写公式:DB2 TIMESTAMPDIFF(4, TS1-TS2) → DM8 DATEDIFF(SECOND, 起点, 终点) / 60。
						// 原 SQL(完整保留):
						// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + dt_temp.Rows[j]["SUSPEND_TIME"].ToString().Trim() + "')  - TIMESTAMP('" + dt_temp.Rows[j]["DEV_NAME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
						// DM8 SQL:
						sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + dt_temp.Rows[j]["DEV_NAME"].ToString().Trim() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + dt_temp.Rows[j]["SUSPEND_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS')) / 60 from DUAL";
						cmd_inq.Parameters.Clear();
						cmd_inq.SetCommandText(sqlstr);
						Log::Trace("", "", "到位时间差分钟语句 sqlstr：{0}", sqlstr);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							minutes_diffx = cmd_inq.GetDecimal(1);
							//use_time = use_time + days_diffx * 24 * 60 + hours_diffx * 60 + minutes_diffx;
							prep_time = prep_time + minutes_diffx;

							Log::Trace("", "", "到位时间差分钟值 prep_time：{0}", prep_time);
						}
						else
						{
							use_time = use_time;
						}
						cmd_inq.Close();
						//dt_temp.Clear();
					}
					//使用时间
					if (dt_temp.Rows[j]["SUSPEND_TIME"].ToString().Trim().GetLength() > 0 && dt_temp.Rows[j]["D_TIME"].ToString().Trim().GetLength() > 0)
					{
						// DM8 适配 CHANGE-EXEC-100:使用时间差分钟数:从 SUSPEND_TIME 到 D_TIME 实际经过的完整分钟数。
						// 改写原因:DB2 两参数 TIMESTAMPDIFF(4=分钟) 改为 DM8 标准写法(实际完整时长)。口径同 HR-002 选①。
						// 改写公式:DB2 TIMESTAMPDIFF(4, TS1-TS2) → DM8 DATEDIFF(SECOND, 起点, 终点) / 60。
						// 原 SQL(完整保留):
						// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + dt_temp.Rows[j]["D_TIME"].ToString().Trim() + "')  - TIMESTAMP('" + dt_temp.Rows[j]["SUSPEND_TIME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
						// DM8 SQL:
						sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + dt_temp.Rows[j]["SUSPEND_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + dt_temp.Rows[j]["D_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS')) / 60 from DUAL";
						cmd_inq.Parameters.Clear();
						cmd_inq.SetCommandText(sqlstr);
						Log::Trace("", "", "使用时间差分钟语句 sqlstr：{0}", sqlstr);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							minutes_diffx = cmd_inq.GetDecimal(1);
							//use_time = use_time + days_diffx * 24 * 60 + hours_diffx * 60 + minutes_diffx;
							use_time = use_time + minutes_diffx;

							Log::Trace("", "", "使用时间差分钟值 use_time：{0}", use_time);
						}
						else
						{
							use_time = use_time;
						}
						cmd_inq.Close();
						//dt_temp.Clear();
					}

					pre_leave_time = dt_temp.Rows[j]["D_TIME"].ToString();

				}
			

			//-----------------------------------------------------------
			/* ***** 维修 ***** */
				dt_temp.Clear();
				sqlstr1 = " SELECT REPAIR_START_TIME, REPAIR_END_TIME FROM TTMSM57 WHERE CRANENO = '" + dt_dev.Rows[i]["CRANENO"].ToString() + "'"
					" AND REPAIR_START_TIME BETWEEN '" + ttmsm66["REC_CREATE_TIME"].ToString() + "' AND '" + ttmsm66["CHANGE_TIME"].ToString().Trim() + "'";
				dt_temp.Clear();
				Db::QueryTable(sqlstr1, dt_temp);
				Log::Trace("", "", "维修语句 sqlstr：{0}", sqlstr1);
				for (int j = 0; j < dt_temp.Rows.get_Count(); j++)
				{
					if (dt_temp.Rows[j]["REPAIR_START_TIME"].ToString().Trim().GetLength() > 0 && dt_temp.Rows[j]["REPAIR_END_TIME"].ToString().Trim().GetLength() > 0)
					{
						// DM8 适配 CHANGE-EXEC-101:修理时间差分钟数:从 REPAIR_START_TIME 到 REPAIR_END_TIME 实际经过的完整分钟数。
						// 改写原因:DB2 两参数 TIMESTAMPDIFF(4=分钟) 改为 DM8 标准写法(实际完整时长)。口径同 HR-002 选①。
						// 改写公式:DB2 TIMESTAMPDIFF(4, TS1-TS2) → DM8 DATEDIFF(SECOND, 起点, 终点) / 60。
						// 原 SQL(完整保留):
						// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + dt_temp.Rows[j]["REPAIR_END_TIME"].ToString().Trim() + "')  - TIMESTAMP('" + dt_temp.Rows[j]["REPAIR_START_TIME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
						// DM8 SQL:
						sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + dt_temp.Rows[j]["REPAIR_START_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + dt_temp.Rows[j]["REPAIR_END_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS')) / 60 from DUAL";
						cmd_inq.Parameters.Clear();
						cmd_inq.SetCommandText(sqlstr);
						Log::Trace("", "", "维修时间差分钟语句 sqlstr：{0}", sqlstr);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							minutes_diffx = cmd_inq.GetDecimal(1);
							//use_time = use_time + days_diffx * 24 * 60 + hours_diffx * 60 + minutes_diffx;
							repair_time = repair_time + minutes_diffx;

							Log::Trace("", "", "维修时间差分钟值 repair_time：{0}", repair_time);
						}
						else
						{
							use_time = use_time;
						}
						cmd_inq.Close();

					}

				}
		
			//-----------------------------------------------------------

			//SELECT round(:use_time / 60, 2), round(:prep_time / 60, 2), round((:all_time - : use_time - : prep_time) / 60, 2), round(:repair_time / 60, 2), 
			//	round((:use_time / : all_time) * 100, 2), round(((:use_time + : prep_time) / :all_time) * 100, 2)
			//INTO :use_time, : prep_time, : free_time, : repair_time, : good_rate, : good_rate1
			//	  FROM sysibm.dual;
			// DM8 适配 CHANGE-110：对运行时分钟值做 ROUND 汇总计算;SYSIBM.SYSDUMMY1 辅助表改为 DUAL;计算口径与参数保持不变。
			// 改写原因：DM 支持 DUAL 辅助表(官方文档);纯算术表达式无方言差异;同函数 TIMESTAMPDIFF 语句属 HR-002,另行处理;DM8 尚未实测。
			// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
			// 原 SQL（完整保留）：
			// sqlstr = "SELECT ROUND(" + use_time.ToString() + "/60,2),ROUND(" + prep_time.ToString() + "/60,2),ROUND((" + all_time.ToString() + " - " + use_time.ToString() + " - " + prep_time.ToString() + ")/ 60, 2), ROUND("
				// "" + repair_time.ToString() + " / 60, 2),ROUND((" + use_time.ToString() + "/" + all_time.ToString() + ") * 100, 2),ROUND(((" + use_time.ToString() + " + " + prep_time.ToString() + ") /" + all_time.ToString() + ")* 100, 2) "
				// " FROM SYSIBM.SYSDUMMY1";
			// DM8 SQL：
			sqlstr = "SELECT ROUND(" + use_time.ToString() + "/60,2),ROUND(" + prep_time.ToString() + "/60,2),ROUND((" + all_time.ToString() + " - " + use_time.ToString() + " - " + prep_time.ToString() + ")/ 60, 2), ROUND("
				"" + repair_time.ToString() + " / 60, 2),ROUND((" + use_time.ToString() + "/" + all_time.ToString() + ") * 100, 2),ROUND(((" + use_time.ToString() + " + " + prep_time.ToString() + ") /" + all_time.ToString() + ")* 100, 2) "
				" FROM DUAL";
			cmd_inq.Parameters.Clear();
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", "", "总结时间差分钟语句 sqlstr：{0}", sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				use_time = cmd_inq.GetDecimal(1);
				prep_time = cmd_inq.GetDecimal(2);
				free_time = cmd_inq.GetDecimal(3);
				repair_time = cmd_inq.GetDecimal(4);
				good_rate = cmd_inq.GetDecimal(5);
				good_rate1 = cmd_inq.GetDecimal(6);

				Log::Trace("", "", "总结时间差分钟值 use_time：{0}", use_time);
				Log::Trace("", "", "总结时间差分钟值 prep_time：{0}", prep_time);
				Log::Trace("", "", "总结时间差分钟值 free_time：{0}", free_time);
				Log::Trace("", "", "总结时间差分钟值 repair_time：{0}", repair_time);
				Log::Trace("", "", "总结时间差分钟值 good_rate：{0}", good_rate);
				Log::Trace("", "", "总结时间差分钟值 good_rate1：{0}", good_rate1);
			}
			else
			{
				//use_time = use_time;
			}
			cmd_inq.Close();


			//Log::Trace("", "", "结果验证 i：{0}", i);
			//Log::Trace("", "", "结果验证 DEV_NO：{0}", dt_dev.Rows[i]["DEV_NO"].ToString());
			bcls_ret->Tables[0].Rows.Add();

			bcls_ret->Tables[0].Rows[i][0] = dt_dev.Rows[i]["CRANENO"].ToString();
			bcls_ret->Tables[0].Rows[i]["REMARK_1"] = use_time;
			bcls_ret->Tables[0].Rows[i]["DEV_NAME"] = prep_time;
			bcls_ret->Tables[0].Rows[i]["REMARK_2"] = free_time;
			bcls_ret->Tables[0].Rows[i]["REMARK_3"] = repair_time;
			bcls_ret->Tables[0].Rows[i]["REMARK_4"] = good_rate;
			bcls_ret->Tables[0].Rows[i]["REMARK_6"] = good_rate1;
			
			
		}

		/*-----------------------------------------------------------*/


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


