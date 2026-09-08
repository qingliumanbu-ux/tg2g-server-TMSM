/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-24 18:47:24
Description: 设备作业率查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme96_inq)


int f_tmsme96_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	CDecimal all_time = 0.00;
	CDecimal use_time = 0.00;
	CDecimal free_time = 0.00;
	CDecimal repair_time = 0.00;
	CDecimal good_rate = 0.00;

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWMA7 twma7(conn);
	CModel ttmsm66 = CModel("TTMSM66");
	CModel ttmsm67 = CModel("TTMSM67");

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

		if (ttmsm66["SM_UNIT_NO"].ToString().Trim() != "")
		{
			sqlwhere += " AND SM_UNIT_NO = @sm_unti_no ";
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
			// DM8 适配 CHANGE-108:查询设备状态记录创建(REC_CREATE_TIME)到变更(CHANGE_TIME)的日历天数差。
			// 改写原因：DB2 DAYS(日期) 差改为 DM8 DATEDIFF(DAY,·),按 HR-002 确认口径①(实际完整时长)
			//   改为 DATEDIFF(SECOND,起点,终点)/因子,整数除法与 DB2 截断行为一致。
			//   时间值为 14 位 YYYYMMDDHH24MISS(HR-001 已确认),用 TO_TIMESTAMP 显式指定;SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
			// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
			// 原 SQL（完整保留）：
			// sqlstr = "select DAYS(DATE(TIMESTAMP ('" + ttmsm66["CHANGE_TIME"].ToString() + "'))) - DAYS(DATE(TIMESTAMP ('" + ttmsm66["REC_CREATE_TIME"].ToString() + "'))) from SYSIBM.SYSDUMMY1";
			// DM8 SQL：
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
			// DM8 适配 CHANGE-346:查询设备状态记录持续小时数,写入 hours_diffx。
			// 改写原因：DB2 两参数 TIMESTAMPDIFF(8=小时) 在 DM8 无对应写法,按 HR-002 确认口径①(实际完整时长)
			//   改为 DATEDIFF(SECOND,起点,终点)/因子,整数除法与 DB2 截断行为一致。
			//   时间值为 14 位 YYYYMMDDHH24MISS(HR-001 已确认),用 TO_TIMESTAMP 显式指定;SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
			// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
			// 原 SQL（完整保留）：
			// sqlstr = "select TIMESTAMPDIFF(8, CHAR(TIMESTAMP('" + ttmsm66["CHANGE_TIME"].ToString() + "')  - TIMESTAMP('" + ttmsm66["REC_CREATE_TIME"].ToString() + "'))) from SYSIBM.SYSDUMMY1";
			// DM8 SQL：
			sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + ttmsm66["REC_CREATE_TIME"].ToString() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + ttmsm66["CHANGE_TIME"].ToString() + "','YYYYMMDDHH24MISS')) / 3600 from DUAL";
			//sqlstr = "select timestampdiff(8, char(timestamp(sysdate) - timestamp('2023-05-25 10:14:01'))) AS "间隔小时" from ttmsm66
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
			// DM8 适配 CHANGE-347:查询设备状态记录持续分钟数,写入 minutes_diffx 并累计到 all_time。
			// 改写原因：DB2 两参数 TIMESTAMPDIFF(4=分钟) 在 DM8 无对应写法,按 HR-002 确认口径①(实际完整时长)
			//   改为 DATEDIFF(SECOND,起点,终点)/因子,整数除法与 DB2 截断行为一致。
			//   时间值为 14 位 YYYYMMDDHH24MISS(HR-001 已确认),用 TO_TIMESTAMP 显式指定;SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
			// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
			// 原 SQL（完整保留）：
			// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + ttmsm66["CHANGE_TIME"].ToString() + "')  - TIMESTAMP('" + ttmsm66["REC_CREATE_TIME"].ToString() + "'))) from SYSIBM.SYSDUMMY1";
			// DM8 SQL：
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

		//----------实际设备使用率查询-------------------------------
		//bcls_ret->Tables.Add("TOTALINFO");	//增加块
		//bcls_ret->Tables["TOTALINFO"].Columns.Add(DT_STRING, "DEV_NO");    //总记录数
		//bcls_ret->Tables["TOTALINFO"].Columns.Add(DT_DECIMAL, "USE_TIME");    //总记录数
		//bcls_ret->Tables["TOTALINFO"].Columns.Add(DT_DECIMAL, "SPARE_TIME");    //总记录数
		//bcls_ret->Tables["TOTALINFO"].Columns.Add(DT_DECIMAL, "MAINTAIN_TIME");    //总记录数
		//bcls_ret->Tables["TOTALINFO"].Columns.Add(DT_DECIMAL, "USE_RATE");    //总记录数
		//bcls_ret->Tables["TOTALINFO"].Columns.Add(DT_STRING, "DEV_NAME");    //总记录数
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "DEV_NO");    //总记录数
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "DEV_NAME");    //总记录数
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "REMARK_1");    //总记录数
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "REMARK_2");    //总记录数
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "REMARK_3");    //总记录数
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "REMARK_4");    //总记录数
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REMARK_6");    //总记录数

		sqlstr = " SELECT *,STATION_ID||'0'||LPAD(STATION_NO,2,'0') DEV_NO FROM TPSSMD1 ORDER BY AREA_ID";

		//EXEC SQL DECLARE ttmsm96_q CURSOR FOR
		//	SELECT tpssmd1.*, station_id || '0' || lpad(station_no, 2, '0')
		//	FROM tpssmd1
		//	WHERE MAIN_BACKLOG_CODE = :c_sm_unit_no
		//	ORDER BY AREA_ID;
		//EXEC SQL OPEN ttmsm96_q;

		/*SELECT tpssmd1.*,station_id,station_no,station_name,station_id||'0'||lpad(station_no,2,'0')  FROM tpssmd1 */

		//Log::Trace("", "", "sqlstr：{0}", sqlstr);
		Db::QueryTable(sqlstr, dt_dev);
		//Log::Trace("", "", "Count：{0}", dt_dev.Rows.get_Count());
		for (int i = 0; i < dt_dev.Rows.get_Count(); i++)
		{
			use_time = 0;
			free_time = 0;
			repair_time = 0;
			/* ***** 倒罐 ***** */
			//if (strcmp(tpssmd1.station_id, "P") == 0)
			if (strcmp(dt_dev.Rows[i]["STATION_ID"].ToString().Trim(), "P") == 0)
			{
				sqlstr1 = " SELECT START_TIME, END_TIME	FROM TMMSM12 WHERE STATION_NO = '" + dt_dev.Rows[i]["STATION_NO"].ToString() + "'"
					" AND START_TIME BETWEEN '" + ttmsm66["REC_CREATE_TIME"].ToString() + "' AND '" + ttmsm66["CHANGE_TIME"].ToString().Trim() + "'";
				Db::QueryTable(sqlstr1, dt_temp);
				Log::Trace("", "", "倒罐语句 sqlstr：{0}", sqlstr1);
				for (int j = 0; j < dt_temp.Rows.get_Count(); j++)
				{
					if (dt_temp.Rows[j]["START_TIME"].ToString().Trim().GetLength() > 0 && dt_temp.Rows[j]["END_TIME"].ToString().Trim().GetLength() > 0)
					{
						//ret = EPTimeDiff(leave_time, arrive_time, &days_diffx, &hours_diffx, &minutes_diffx, &seconds_diffx, msg);
						//sqlstr = "select DAYS(DATE(TIMESTAMP ('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "'))) - DAYS(DATE(TIMESTAMP ('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
						//cmd_inq.Parameters.Clear();
						//cmd_inq.SetCommandText(sqlstr);
						//Log::Trace("", "", "倒灌时间差天数语句 sqlstr：{0}", sqlstr);
						//cmd_inq.ExecuteReader();
						//if (cmd_inq.Read())
						//{
						//	days_diffx = cmd_inq.GetDecimal(1);
						//	//use_time = use_time + days_diffx * 24 * 60 + hours_diffx * 60 + minutes_diffx;
						//	use_time = use_time + days_diffx * 24 * 60 ;

						//	Log::Trace("", "", "倒灌时间差天数值 use_time：{0}", use_time);
						//}
						//else
						//{
						//	use_time = use_time ;
						//}
						//cmd_inq.Close();

						//sqlstr = "select TIMESTAMPDIFF(8, CHAR(TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "')  - TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
						//cmd_inq.Parameters.Clear();
						//cmd_inq.SetCommandText(sqlstr);
						//Log::Trace("", "", "倒灌时间差小时语句 sqlstr：{0}", sqlstr);
						//cmd_inq.ExecuteReader();
						//if (cmd_inq.Read())
						//{
						//	hours_diffx = cmd_inq.GetDecimal(1);
						//	//use_time = use_time + days_diffx * 24 * 60 + hours_diffx * 60 + minutes_diffx;
						//	use_time = use_time + hours_diffx * 60;

						//	Log::Trace("", "", "倒灌时间差小时值 use_time：{0}", use_time);
						//}
						//else
						//{
						//	use_time = use_time;
						//}
						//cmd_inq.Close();

						// DM8 适配 CHANGE-348:查询倒罐作业(TMMSM12)开始到结束的分钟数,累计 use_time。
						// 改写原因：DB2 两参数 TIMESTAMPDIFF(4=分钟) 在 DM8 无对应写法,按 HR-002 确认口径①(实际完整时长)
						//   改为 DATEDIFF(SECOND,起点,终点)/因子,整数除法与 DB2 截断行为一致。
						//   时间值为 14 位 YYYYMMDDHH24MISS(HR-001 已确认),用 TO_TIMESTAMP 显式指定;SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
						// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
						// 原 SQL（完整保留）：
						// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "')  - TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
						// DM8 SQL：
						sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS')) / 60 from DUAL";
						cmd_inq.Parameters.Clear();
						cmd_inq.SetCommandText(sqlstr);
						Log::Trace("", "", "倒灌时间差分钟语句 sqlstr：{0}", sqlstr);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							minutes_diffx = cmd_inq.GetDecimal(1);
							//use_time = use_time + days_diffx * 24 * 60 + hours_diffx * 60 + minutes_diffx;
							use_time = use_time + minutes_diffx;

							Log::Trace("", "", "倒灌时间差分钟值 use_time：{0}", use_time);
						}
						else
						{
							use_time = use_time;
						}
						cmd_inq.Close();
						//dt_temp.Clear();
					}

				}
			}

			//-----------------------------------------------------------
			/* ***** 脱硫 ***** */
			//if (strcmp(tpssmd1.station_id, "S") == 0)
			dt_temp.Clear();
			if (strcmp(dt_dev.Rows[i]["STATION_ID"].ToString().Trim(), "S") == 0)
			{												//TMMSM14 炼钢脱硫实绩表
				sqlstr1 = " SELECT START_TIME, END_TIME	FROM TMMSM14 WHERE STATION_NO = '" + dt_dev.Rows[i]["STATION_NO"].ToString() + "'"
					" AND START_TIME BETWEEN '" + ttmsm66["REC_CREATE_TIME"].ToString() + "' AND '" + ttmsm66["CHANGE_TIME"].ToString().Trim() + "'";
				dt_temp.Clear();
				Db::QueryTable(sqlstr1, dt_temp);
				Log::Trace("", "", "脱硫语句 sqlstr：{0}", sqlstr1);
				for (int j = 0; j < dt_temp.Rows.get_Count(); j++)
				{
					if (dt_temp.Rows[j]["START_TIME"].ToString().Trim().GetLength() > 0 && dt_temp.Rows[j]["END_TIME"].ToString().Trim().GetLength() > 0)
					{
						// DM8 适配 CHANGE-349:查询脱硫作业(TMMSM14)开始到结束的分钟数,累计 use_time。
						// 改写原因：DB2 两参数 TIMESTAMPDIFF(4=分钟) 在 DM8 无对应写法,按 HR-002 确认口径①(实际完整时长)
						//   改为 DATEDIFF(SECOND,起点,终点)/因子,整数除法与 DB2 截断行为一致。
						//   时间值为 14 位 YYYYMMDDHH24MISS(HR-001 已确认),用 TO_TIMESTAMP 显式指定;SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
						// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
						// 原 SQL（完整保留）：
						// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "')  - TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
						// DM8 SQL：
						sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS')) / 60 from DUAL";
						cmd_inq.Parameters.Clear();
						cmd_inq.SetCommandText(sqlstr);
						Log::Trace("", "", "脱硫时间差分钟语句 sqlstr：{0}", sqlstr);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							minutes_diffx = cmd_inq.GetDecimal(1);
							//use_time = use_time + days_diffx * 24 * 60 + hours_diffx * 60 + minutes_diffx;
							use_time = use_time + minutes_diffx;

							Log::Trace("", "", "脱硫时间差分钟值 use_time：{0}", use_time);
						}
						else
						{
							use_time = use_time;
						}
						cmd_inq.Close();

					}

				}
			}
			//-----------------------------------------------------------

			//-----------------------------------------------------------
			/* ***** 转炉 ***** */
			//if (strcmp(tpssmd1.station_id,"B") == 0)
			dt_temp.Clear();
			if (strcmp(dt_dev.Rows[i]["STATION_ID"].ToString().Trim(), "B") == 0)
			{												//TMMSM21 炼钢转炉实绩表
				sqlstr1 = " SELECT START_TIME, END_TIME	FROM TMMSM21 WHERE STATION_NO = '" + dt_dev.Rows[i]["STATION_NO"].ToString() + "'"
					" AND START_TIME BETWEEN '" + ttmsm66["REC_CREATE_TIME"].ToString() + "' AND '" + ttmsm66["CHANGE_TIME"].ToString().Trim() + "'";
				dt_temp.Clear();
				Db::QueryTable(sqlstr1, dt_temp);
				Log::Trace("", "", "转炉语句 sqlstr：{0}", sqlstr1);
				for (int j = 0; j < dt_temp.Rows.get_Count(); j++)
				{
					if (dt_temp.Rows[j]["START_TIME"].ToString().Trim().GetLength() > 0 && dt_temp.Rows[j]["END_TIME"].ToString().Trim().GetLength() > 0)
					{
						// DM8 适配 CHANGE-350:查询转炉作业(TMMSM21)开始到结束的分钟数,累计 use_time。
						// 改写原因：DB2 两参数 TIMESTAMPDIFF(4=分钟) 在 DM8 无对应写法,按 HR-002 确认口径①(实际完整时长)
						//   改为 DATEDIFF(SECOND,起点,终点)/因子,整数除法与 DB2 截断行为一致。
						//   时间值为 14 位 YYYYMMDDHH24MISS(HR-001 已确认),用 TO_TIMESTAMP 显式指定;SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
						// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
						// 原 SQL（完整保留）：
						// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "')  - TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
						// DM8 SQL：
						sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS')) / 60 from DUAL";
						cmd_inq.Parameters.Clear();
						cmd_inq.SetCommandText(sqlstr);
						Log::Trace("", "", "转炉时间差分钟语句 sqlstr：{0}", sqlstr);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							minutes_diffx = cmd_inq.GetDecimal(1);
							//use_time = use_time + days_diffx * 24 * 60 + hours_diffx * 60 + minutes_diffx;
							use_time = use_time + minutes_diffx;

							Log::Trace("", "", "转炉时间差分钟值 use_time：{0}", use_time);
						}
						else
						{
							use_time = use_time;
						}
						cmd_inq.Close();

					}

				}
			}
			//-----------------------------------------------------------

			//-----------------------------------------------------------
			/* ***** 吹氩 ***** */
			//if (strcmp(tpssmd1.station_id, "A") == 0)
			dt_temp.Clear();
			if (strcmp(dt_dev.Rows[i]["STATION_ID"].ToString().Trim(), "A") == 0)
			{												//TMMSM22 炼钢吹氩作业实绩表
				sqlstr1 = " SELECT START_TIME, END_TIME	FROM TMMSM22 WHERE STATION_NO = '" + dt_dev.Rows[i]["STATION_NO"].ToString() + "'"
					" AND START_TIME BETWEEN '" + ttmsm66["REC_CREATE_TIME"].ToString() + "' AND '" + ttmsm66["CHANGE_TIME"].ToString().Trim() + "'";
				dt_temp.Clear();
				Db::QueryTable(sqlstr1, dt_temp);
				Log::Trace("", "", "吹氩语句 sqlstr：{0}", sqlstr1);
				for (int j = 0; j < dt_temp.Rows.get_Count(); j++)
				{
					if (dt_temp.Rows[j]["START_TIME"].ToString().Trim().GetLength() > 0 && dt_temp.Rows[j]["END_TIME"].ToString().Trim().GetLength() > 0)
					{
						// DM8 适配 CHANGE-351:查询吹氩作业(TMMSM22)开始到结束的分钟数,累计 use_time。
						// 改写原因：DB2 两参数 TIMESTAMPDIFF(4=分钟) 在 DM8 无对应写法,按 HR-002 确认口径①(实际完整时长)
						//   改为 DATEDIFF(SECOND,起点,终点)/因子,整数除法与 DB2 截断行为一致。
						//   时间值为 14 位 YYYYMMDDHH24MISS(HR-001 已确认),用 TO_TIMESTAMP 显式指定;SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
						// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
						// 原 SQL（完整保留）：
						// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "')  - TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
						// DM8 SQL：
						sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS')) / 60 from DUAL";
						cmd_inq.Parameters.Clear();
						cmd_inq.SetCommandText(sqlstr);
						Log::Trace("", "", "吹氩时间差分钟语句 sqlstr：{0}", sqlstr);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							minutes_diffx = cmd_inq.GetDecimal(1);
							//use_time = use_time + days_diffx * 24 * 60 + hours_diffx * 60 + minutes_diffx;
							use_time = use_time + minutes_diffx;

							Log::Trace("", "", "吹氩时间差分钟值 use_time：{0}", use_time);
						}
						else
						{
							use_time = use_time;
						}
						cmd_inq.Close();

					}

				}
			}
			//-----------------------------------------------------------

			//-----------------------------------------------------------
			/* ***** RH ***** */
			//if (strcmp(tpssmd1.station_id,"R") == 0)
			dt_temp.Clear();
			if (strcmp(dt_dev.Rows[i]["STATION_ID"].ToString().Trim(), "R") == 0)
			{												//TMMSM23 炼钢RH作业实绩表
				sqlstr1 = " SELECT START_TIME, END_TIME	FROM TMMSM23 WHERE STATION_NO = '" + dt_dev.Rows[i]["STATION_NO"].ToString() + "'"
					" AND START_TIME BETWEEN '" + ttmsm66["REC_CREATE_TIME"].ToString() + "' AND '" + ttmsm66["CHANGE_TIME"].ToString().Trim() + "'";
				dt_temp.Clear();
				Db::QueryTable(sqlstr1, dt_temp);
				Log::Trace("", "", "RH语句 sqlstr：{0}", sqlstr1);
				for (int j = 0; j < dt_temp.Rows.get_Count(); j++)
				{
					if (dt_temp.Rows[j]["START_TIME"].ToString().Trim().GetLength() > 0 && dt_temp.Rows[j]["END_TIME"].ToString().Trim().GetLength() > 0)
					{
						// DM8 适配 CHANGE-352:查询RH作业(TMMSM23)开始到结束的分钟数,累计 use_time。
						// 改写原因：DB2 两参数 TIMESTAMPDIFF(4=分钟) 在 DM8 无对应写法,按 HR-002 确认口径①(实际完整时长)
						//   改为 DATEDIFF(SECOND,起点,终点)/因子,整数除法与 DB2 截断行为一致。
						//   时间值为 14 位 YYYYMMDDHH24MISS(HR-001 已确认),用 TO_TIMESTAMP 显式指定;SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
						// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
						// 原 SQL（完整保留）：
						// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "')  - TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
						// DM8 SQL：
						sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS')) / 60 from DUAL";
						cmd_inq.Parameters.Clear();
						cmd_inq.SetCommandText(sqlstr);
						Log::Trace("", "", "RH时间差分钟语句 sqlstr：{0}", sqlstr);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							minutes_diffx = cmd_inq.GetDecimal(1);
							//use_time = use_time + days_diffx * 24 * 60 + hours_diffx * 60 + minutes_diffx;
							use_time = use_time + minutes_diffx;

							Log::Trace("", "", "RH时间差分钟值 use_time：{0}", use_time);
						}
						else
						{
							use_time = use_time;
						}
						cmd_inq.Close();

					}

				}
			}
			//-----------------------------------------------------------

			//-----------------------------------------------------------
			/* ***** LF ***** */
			//if (strcmp(tpssmd1.station_id,"L") == 0)
			dt_temp.Clear();
			if (strcmp(dt_dev.Rows[i]["STATION_ID"].ToString().Trim(), "A") == 0)
			{												//TMMSM24 炼钢LF作业实绩表
				sqlstr1 = " SELECT START_TIME, END_TIME	FROM TMMSM22 WHERE STATION_NO = '" + dt_dev.Rows[i]["STATION_NO"].ToString() + "'"
					" AND START_TIME BETWEEN '" + ttmsm66["REC_CREATE_TIME"].ToString() + "' AND '" + ttmsm66["CHANGE_TIME"].ToString().Trim() + "'";
				dt_temp.Clear();
				Db::QueryTable(sqlstr1, dt_temp);
				Log::Trace("", "", "LF语句 sqlstr：{0}", sqlstr1);
				for (int j = 0; j < dt_temp.Rows.get_Count(); j++)
				{
					if (dt_temp.Rows[j]["START_TIME"].ToString().Trim().GetLength() > 0 && dt_temp.Rows[j]["END_TIME"].ToString().Trim().GetLength() > 0)
					{
						// DM8 适配 CHANGE-353:查询LF作业(TMMSM22)开始到结束的分钟数,累计 use_time。
						// 改写原因：DB2 两参数 TIMESTAMPDIFF(4=分钟) 在 DM8 无对应写法,按 HR-002 确认口径①(实际完整时长)
						//   改为 DATEDIFF(SECOND,起点,终点)/因子,整数除法与 DB2 截断行为一致。
						//   时间值为 14 位 YYYYMMDDHH24MISS(HR-001 已确认),用 TO_TIMESTAMP 显式指定;SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
						// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
						// 原 SQL（完整保留）：
						// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "')  - TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
						// DM8 SQL：
						sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS')) / 60 from DUAL";
						cmd_inq.Parameters.Clear();
						cmd_inq.SetCommandText(sqlstr);
						Log::Trace("", "", "LF时间差分钟语句 sqlstr：{0}", sqlstr);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							minutes_diffx = cmd_inq.GetDecimal(1);
							//use_time = use_time + days_diffx * 24 * 60 + hours_diffx * 60 + minutes_diffx;
							use_time = use_time + minutes_diffx;

							Log::Trace("", "", "LF时间差分钟值 use_time：{0}", use_time);
						}
						else
						{
							use_time = use_time;
						}
						cmd_inq.Close();

					}

				}
			}
			//-----------------------------------------------------------

			//-----------------------------------------------------------
			/* ***** CC连铸 ***** */
			//if (strcmp(tpssmd1.station_id,"R") == 0)
			dt_temp.Clear();
			if (strcmp(dt_dev.Rows[i]["STATION_ID"].ToString().Trim(), "R") == 0)
			{												//TMMSM23 炼钢连铸作业实绩表
				sqlstr1 = " SELECT START_TIME, END_TIME	FROM TMMSM31 WHERE STATION_NO = '" + dt_dev.Rows[i]["STATION_NO"].ToString() + "'"
					" AND START_TIME BETWEEN '" + ttmsm66["REC_CREATE_TIME"].ToString() + "' AND '" + ttmsm66["CHANGE_TIME"].ToString().Trim() + "'";
				dt_temp.Clear();
				Db::QueryTable(sqlstr1, dt_temp);
				Log::Trace("", "", "RH语句 sqlstr：{0}", sqlstr1);
				for (int j = 0; j < dt_temp.Rows.get_Count(); j++)
				{
					if (dt_temp.Rows[j]["START_TIME"].ToString().Trim().GetLength() > 0 && dt_temp.Rows[j]["END_TIME"].ToString().Trim().GetLength() > 0)
					{
						// DM8 适配 CHANGE-354:查询连铸作业(TMMSM31)开始到结束的分钟数,累计 use_time。
						// 改写原因：DB2 两参数 TIMESTAMPDIFF(4=分钟) 在 DM8 无对应写法,按 HR-002 确认口径①(实际完整时长)
						//   改为 DATEDIFF(SECOND,起点,终点)/因子,整数除法与 DB2 截断行为一致。
						//   时间值为 14 位 YYYYMMDDHH24MISS(HR-001 已确认),用 TO_TIMESTAMP 显式指定;SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
						// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
						// 原 SQL（完整保留）：
						// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "')  - TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
						// DM8 SQL：
						sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + dt_temp.Rows[j]["START_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + dt_temp.Rows[j]["END_TIME"].ToString().Trim() + "','YYYYMMDDHH24MISS')) / 60 from DUAL";
						cmd_inq.Parameters.Clear();
						cmd_inq.SetCommandText(sqlstr);
						Log::Trace("", "", "连铸时间差分钟语句 sqlstr：{0}", sqlstr);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							minutes_diffx = cmd_inq.GetDecimal(1);
							//use_time = use_time + days_diffx * 24 * 60 + hours_diffx * 60 + minutes_diffx;
							use_time = use_time + minutes_diffx;

							Log::Trace("", "", "连铸时间差分钟值 use_time：{0}", use_time);
						}
						else
						{
							use_time = use_time;
						}
						cmd_inq.Close();

					}

				}
			}
			//-----------------------------------------------------------


			/* ***** 维修 ***** */                        //梅钢工器具历史信息表
			dt_temp.Clear();
			sqlstr1 = " SELECT S_DATETIME,E_DATETIME FROM TTMSM95 WHERE STATUS = '11'  AND DEV_NO = '" + dt_dev.Rows[i]["DEV_NO"].ToString() + "'"
				" AND S_DATETIME BETWEEN '" + ttmsm66["REC_CREATE_TIME"].ToString() + "' AND '" + ttmsm66["CHANGE_TIME"].ToString().Trim() + "'";
			dt_temp.Clear();
			Db::QueryTable(sqlstr1, dt_temp);
			Log::Trace("", "", "维修语句 sqlstr：{0}", sqlstr1);
			for (int j = 0; j < dt_temp.Rows.get_Count(); j++)
			{
				if (dt_temp.Rows[j]["S_DATETIME"].ToString().Trim().GetLength() > 0 && dt_temp.Rows[j]["E_DATETIME"].ToString().Trim().GetLength() > 0)
				{
					// DM8 适配 CHANGE-355:查询维修作业(TTMSM95,STATUS=11)开始到结束的分钟数,累计 repair_time。
					// 改写原因：DB2 两参数 TIMESTAMPDIFF(4=分钟) 在 DM8 无对应写法,按 HR-002 确认口径①(实际完整时长)
					//   改为 DATEDIFF(SECOND,起点,终点)/因子,整数除法与 DB2 截断行为一致。
					//   时间值为 14 位 YYYYMMDDHH24MISS(HR-001 已确认),用 TO_TIMESTAMP 显式指定;SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
					// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
					// 原 SQL（完整保留）：
					// sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + dt_temp.Rows[j]["E_DATETIME"].ToString().Trim() + "')  - TIMESTAMP('" + dt_temp.Rows[j]["S_DATETIME"].ToString().Trim() + "'))) from SYSIBM.SYSDUMMY1";
					// DM8 SQL：
					sqlstr = "select DATEDIFF(SECOND, TO_TIMESTAMP('" + dt_temp.Rows[j]["S_DATETIME"].ToString().Trim() + "','YYYYMMDDHH24MISS'), TO_TIMESTAMP('" + dt_temp.Rows[j]["E_DATETIME"].ToString().Trim() + "','YYYYMMDDHH24MISS')) / 60 from DUAL";
					cmd_inq.Parameters.Clear();
					cmd_inq.SetCommandText(sqlstr);
					Log::Trace("", "", "维修时间差分钟语句 sqlstr：{0}", sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						minutes_diffx = cmd_inq.GetDecimal(1);
						//repair_time = repair_time + days_diffx*24*60 + hours_diffx*60 + minutes_diffx;
						repair_time = repair_time + minutes_diffx;

						Log::Trace("", "", "连铸时间差分钟值 use_time：{0}", use_time);
					}
					else
					{
						use_time = use_time;
					}
					cmd_inq.Close();

				}
			}
			//-----------------------------------------------------------

			//SELECT round(:use_time / 60, 2), round((:all_time - : use_time) / 60, 2), round(:repair_time / 60, 2), round((:use_time / : all_time) * 100, 2)
			//INTO :use_time, : free_time, : repair_time, : good_rate
			//	  FROM sysibm.dual;
			// DM8 适配 CHANGE-109:汇总使用/空闲/维修时长(小时)与作业率(good_rate),纯算术表达式。
			// 改写原因：SYSIBM.SYSDUMMY1 辅助表改为 DUAL;计算口径与参数不变;按 HR-002 确认口径①(实际完整时长)
			//   改为 DATEDIFF(SECOND,起点,终点)/因子,整数除法与 DB2 截断行为一致。
			//   时间值为 14 位 YYYYMMDDHH24MISS(HR-001 已确认),用 TO_TIMESTAMP 显式指定;SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
			// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
			// 原 SQL（完整保留）：
			// sqlstr = "SELECT ROUND(" + use_time.ToString() + "/60,2),ROUND((" + all_time.ToString()+" - "+ use_time.ToString() + ")/ 60, 2), ROUND("
			// "" + repair_time.ToString() + " / 60, 2),ROUND((" + use_time.ToString() + "/" + all_time.ToString() + ") * 100, 2)"
			// " FROM SYSIBM.SYSDUMMY1";
			// DM8 SQL：
			sqlstr = "SELECT ROUND(" + use_time.ToString() + "/60,2),ROUND((" + all_time.ToString()+" - "+ use_time.ToString() + ")/ 60, 2), ROUND("
				"" + repair_time.ToString() + " / 60, 2),ROUND((" + use_time.ToString() + "/" + all_time.ToString() + ") * 100, 2)"
				 " FROM DUAL";
			cmd_inq.Parameters.Clear();
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", "", "总结时间差分钟语句 sqlstr：{0}", sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				use_time = cmd_inq.GetDecimal(1);
				free_time = cmd_inq.GetDecimal(2);
				repair_time = cmd_inq.GetDecimal(3); 
				good_rate = cmd_inq.GetDecimal(4);

				Log::Trace("", "", "总结时间差分钟值 use_time：{0}", use_time);
				Log::Trace("", "", "总结时间差分钟值 free_time：{0}", free_time);
				Log::Trace("", "", "总结时间差分钟值 repair_time：{0}", repair_time);
				Log::Trace("", "", "总结时间差分钟值 good_rate：{0}", good_rate);
			}
			else
			{
				//use_time = use_time;
			}
			cmd_inq.Close();


			////bcls_ret->Tables.Add("TOTALINFO");	//增加块
			////bcls_ret->Tables["TOTALINFO"].Columns.Add(DT_STRING, "DEV_NO");    //总记录数
			////bcls_ret->Tables["TOTALINFO"].Columns.Add(DT_DECIMAL, "USE_TIME");    //总记录数
			////bcls_ret->Tables["TOTALINFO"].Columns.Add(DT_DECIMAL, "SPARE_TIME");    //总记录数
			////bcls_ret->Tables["TOTALINFO"].Columns.Add(DT_DECIMAL, "MAINTAIN_TIME");    //总记录数
			////bcls_ret->Tables["TOTALINFO"].Columns.Add(DT_DECIMAL, "USE_RATE");    //总记录数
			////bcls_ret->Tables["TOTALINFO"].Columns.Add(DT_STRING, "DEV_NAME");    //总记录数
			//bcls_ret->Tables["TOTALINFO"].Rows.Add();
			//bcls_ret->Tables["TOTALINFO"].Rows[0][0] = dt_dev.Rows[i]["DEV_NO"].ToString();
			//bcls_ret->Tables["TOTALINFO"].Rows[0][1] = use_time;
			//bcls_ret->Tables["TOTALINFO"].Rows[0][2] = free_time;
			//bcls_ret->Tables["TOTALINFO"].Rows[0][3] = repair_time;
			//bcls_ret->Tables["TOTALINFO"].Rows[0][4] = good_rate;

			Log::Trace("", "", "结果验证 i：{0}", i);
			Log::Trace("", "", "结果验证 DEV_NO：{0}", dt_dev.Rows[i]["DEV_NO"].ToString());
			bcls_ret->Tables[0].Rows.Add();
			//bcls_ret->Tables[0].Columns.Add(DT_STRING, "FURNACE_NO_BOF");
			//bcls_ret->Tables[0].Columns.Add(DT_STRING, "FURNACE_NO_LF");

			bcls_ret->Tables[0].Rows[i][0] = dt_dev.Rows[i]["DEV_NO"].ToString();
			//bcls_ret->Tables[0].Rows[i]["REMARK_1"] = use_time;
			//bcls_ret->Tables[0].Rows[i]["REMARK_2"] = free_time;
			//bcls_ret->Tables[0].Rows[i]["REMARK_3"] = repair_time;
			//bcls_ret->Tables[0].Rows[i]["REMARK_4"] = good_rate;
			//bcls_ret->Tables[0].Rows[i]["REMARK_6"] = good_rate;


			//bcls_ret->Tables[0].Rows[i][1] = use_time;
			//bcls_ret->Tables[0].Rows[i][2] = use_time;
			//bcls_ret->Tables[0].Rows[i][3] = free_time;
			//bcls_ret->Tables[0].Rows[i][4] = repair_time;
			//bcls_ret->Tables[0].Rows[i][5] = good_rate;
			//bcls_ret->Tables[0].Rows[i][6] = "TEMP";
			bcls_ret->Tables[0].Rows[i]["REMARK_1"] = use_time;
			bcls_ret->Tables[0].Rows[i]["REMARK_6"] = "TEMP";
			bcls_ret->Tables[0].Rows[i]["REMARK_2"] = free_time;
			bcls_ret->Tables[0].Rows[i]["REMARK_3"] = repair_time;
			bcls_ret->Tables[0].Rows[i]["REMARK_4"] = good_rate;
			bcls_ret->Tables[0].Rows[i]["DEV_NAME"] = "DEV_NAME";

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


