/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-06-02 09:57:09
Description: 钢包使用结束结浇
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT

//记录履历
int f_tmsm67_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tmsm01_60106(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString userid = s.userid;

	CString pono = " ";
	CString htno = " "; //熔炼号
	CDecimal v_count = 0;
	CDecimal lf_time = 0;
	CString l_steel_grade = " "; //出钢记号

	CString ladle_no = " ";
	CString usage_st = " ";
	CDecimal vir_ladle_age = 0;
	CDecimal count_fill_time = 0;
	CDecimal cross_lf_count = 0;
	CDecimal cross_lf_heat_time = 0; 
	CDecimal l_ladle_fill_time = 0;
	CDecimal l_vir_add_ladle_age = 0;

	/* 实体类定义 */
	//CModel ttmsm31("TTMSM31");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDataTable dt_temp;

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获得传入参数 */
		htno = bcls_rec->Tables[0].Rows[0]["HTNO"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "需要更新状态的熔炼号 = [{0}]", pono);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "SELECT LADLE_NO," // 钢包号, 
				"USAGE_ST,"				//使用开始时间
				"VIR_LADLE_AGE,"		// 虚拟钢包龄
				"COUNT_FILL_TIME,"		// 盛钢时间累计
				"CROSS_LF_COUNT,"		// 过LF炉次数累计
				"CROSS_LF_HEAT_TIME LF "//炉加热时间累计
				"FROM TTMSM01 WHERE HEAT_NO = @htno ";
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("htno", htno);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			ladle_no = cmd_inq.GetString(1);
			usage_st = cmd_inq.GetString(2);
			vir_ladle_age = cmd_inq.GetDecimal(3);
			count_fill_time = cmd_inq.GetDecimal(4);
			cross_lf_count = cmd_inq.GetDecimal(5);
			cross_lf_heat_time = cmd_inq.GetDecimal(16);
		}
		else
		{
			ladle_no =  " ";
			usage_st = " ";
			vir_ladle_age	= 0;
			count_fill_time = 0;
			cross_lf_count  = 0;
			cross_lf_heat_time = 0;
		}
		cmd_inq.Close();

		//   select lf_time	from   lt_cc_info	where  htno = P_htno;
		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//case DB_KIND_ORACLE:	    // Oracle 数据库
		//default:
		//	//炼钢计划运行监控表
		//	sqlstr = "SELECT LF_TIME " // 引流次数   20230602表中无此字段
		//		"FROM TPSSM33 WHERE HEAT_NO = @htno "; //炼钢计划运行监控表
		//	break;
		//}
		//cmd_inq.Parameters.Clear();
		//cmd_inq.Parameters.Set("htno", htno);
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.ExecuteReader();
		//if (cmd_inq.Read())
		//{
		//	lf_time = cmd_inq.GetDecimal(1);
	
		//}
		//else
		//{
		//	lf_time = 0;
		//}
		//cmd_inq.Close();

		//select steel_grade from lo_tap_plan where  htno = P_htno  出钢记号
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			//炼钢计划运行监控表
			sqlstr = "SELECT MAX(ST_NO) " //出钢记号   20230602不知道LO_TAP_PLAN对应哪个产品化表，暂时
				"FROM TPSSM11 WHERE HEAT_NO = @htno "; //炼钢作业计划编制主表
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("htno", htno);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			l_steel_grade = cmd_inq.GetString(1); // --取熔炼号对应的出钢记号

		}
		cmd_inq.Close();

		//l_ladle_fill_time  = (l_systime - l_usage_st) * 24 * 60;                          			//--计算钢包盛钢时间
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "select TIMESTAMPDIFF(4, CHAR(TIMESTAMP('" + datetime + "')  - TIMESTAMP('" + usage_st + "'))) from SYSIBM.SYSDUMMY1";
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.SetCommandText(sqlstr);

		Log::Trace("", "", "时间差分钟语句 sqlstr：{0}", sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			l_ladle_fill_time = cmd_inq.GetDecimal(1);
		}
		else
		{
			l_ladle_fill_time = 0;
		}
		cmd_inq.Close();
		Log::Trace("", "", "分钟差计算值 l_ladle_fill_time：{0}", l_ladle_fill_time.ToString());

	    l_vir_add_ladle_age  = 0;
		//--判断虚拟增加钢包龄
		if ((l_ladle_fill_time >= 200) && (l_ladle_fill_time < 300))
		{
			l_vir_add_ladle_age = 1;
		}
		if ((l_ladle_fill_time >= 300) && (l_ladle_fill_time < 400))
		{
			l_vir_add_ladle_age = 2;
		}
		if (l_ladle_fill_time >= 400)
		{
			l_vir_add_ladle_age = 3;
		}

		//update le_ld_usage_his
		//	set cast_et = l_systime, --钢包使用历史修改一条记录
		//	ladle_fill_time = (l_systime - usage_st) * 24 * 60, --计算钢包盛钢时间
		//	lf_time = l_lf_time, --插入引流次数
		//	steel_grade = l_steel_grade, --插入出钢记号
		//	vir_ladle_age = l_vir_ladle_age + l_vir_add_ladle_age + 1, --插入虚拟钢包龄
		//	count_fill_time = l_count_fill_time + l_ladle_fill_time,
		//	cross_lf_count = l_cross_lf_count,
		//	cross_lf_heat_time = l_cross_lf_heat_time
		//where ladle_no = l_ladle_no and cast_et is null and seq_bc = (select max(seq_bc)
		//	from le_ld_usage_his
		//where ladle_no = l_ladle_no);

		vir_ladle_age = vir_ladle_age + l_vir_add_ladle_age + 1;
		count_fill_time = count_fill_time + l_ladle_fill_time;
		sqlstr = "UPDATE HTMSM01 SET REC_REVISOR= '" + userid + "',"
			"REC_REVISE_TIME = '" + datetime + "',"
			"LADLE_FILL_TIME = " + l_ladle_fill_time.ToString() + ","
			"LF_TIME = " + lf_time.ToString() + ","
			"ST_NO = '" + l_steel_grade + "',"
			"VIR_LADLE_AGE = " + vir_ladle_age.ToString() + ","
			"COUNT_FILL_TIME = " + count_fill_time.ToString() + ","
			"CROSS_LF_COUNT = " + cross_lf_count.ToString() + ","
			"CROSS_LF_HEAT_TIME = '" + cross_lf_heat_time.ToString() + " "
			" WHERE LADLE_NO = '" + ladle_no + "'";

		Log::Trace("", "", "sqlstr：{0}", sqlstr);
		Db::Execute(sqlstr);

		//ttmsm92["REC_CREATOR"] = s.userid;			//记录创建责任者
		//ttmsm92["REC_CREATE_TIME"] = datetime;		//记录创建时刻
		//ttmsm92["REC_REVISOR"] = s.userid;			//记录修改责任者
		//ttmsm92["REC_REVISE_TIME"] = datetime;		//记录修改时刻
		//抛其它工器具跟踪履历
		if (!bcls_rec->Tables.Contains("TMSM67"))
		{
			bcls_rec->Tables.Add("TMSM67");
			bcls_rec->Tables["TMSM67"].Columns.Add(DT_STRING, "SM_UNIT_NO");
			bcls_rec->Tables["TMSM67"].Columns.Add(DT_STRING, "REMARK_1");
			bcls_rec->Tables["TMSM67"].Columns.Add(DT_STRING, "REMARK");
		}
		bcls_rec->Tables["TMSM67"].Rows.Clear();

		//其它工器具跟踪履历跟踪函数
		bcls_rec->Tables["TMSM67"].Rows.Add();
		bcls_rec->Tables["TMSM67"].Rows[0]["SM_UNIT_NO"] = "Z";				//用67表记录其它工器具履历
		bcls_rec->Tables["TMSM67"].Rows[0]["REMARK_1"] = "钢包使用结束结浇";  //时间操作
		bcls_rec->Tables["TMSM67"].Rows[0]["REMARK"] = sqlstr;				 //事件
		doFlag = f_tmsm67_trace(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

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


