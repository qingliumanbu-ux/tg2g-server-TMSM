/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-06-02 15:47:56
Description: 钢包开浇时间
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT


//记录履历
int f_tmsm67_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tmsm01_60108(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString userid = s.userid;

	CString cast_st  = " "; //浇注开始时刻
	CString htno	 = " "; //熔炼号
	CDecimal v_count = 0;
	CDecimal lf_time = 0;
	CString l_steel_grade = " "; //出钢记号
	CString l_ladle_no = " ";    //钢包号
	CString l_ccm_no = " ";
	CString l_cast_no = " ";
	CDecimal l_cast_div_no = 0;

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
		htno		= bcls_rec->Tables[0].Rows[0]["HTNO"].ToString().Trim();
		cast_st		= bcls_rec->Tables[0].Rows[0]["CAST_ST"].ToString().Trim(); //开浇时间

		Log::Trace("", __FUNCTION__, "需要更新状态的熔炼号 = [{0}]", htno);
		Log::Trace("", __FUNCTION__, "需要更新状态的开浇时间 = [{0}]", cast_st);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "SELECT LADLE_NO " // 钢包号, 
				//"USAGE_ST,"				//使用开始时间
				//"VIR_LADLE_AGE,"		// 虚拟钢包龄
				//"COUNT_FILL_TIME,"		// 盛钢时间累计
				//"CROSS_LF_COUNT,"		// 过LF炉次数累计
				//"CROSS_LF_HEAT_TIME LF "//炉加热时间累计
				"FROM TTMSM01 WHERE HEAT_NO = @htno ";
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("htno", htno);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			l_ladle_no = cmd_inq.GetString(1); // --钢包号
		}
		cmd_inq.Close();
			
		//   select steel_grade,ccm_no,cast_no,cast_div_no	from   lo_tap_plan	where  htno = P_htno  出钢记号
		//                                                     对应 TPSSM11
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			//炼钢计划运行监控表
			sqlstr = "SELECT MAX(ST_NO), " //出钢记号   20230602不知道LO_TAP_PLAN对应哪个产品化表，暂时
				"MAX(CC_MACH_NO), "//连铸机号
				"MAX(CAST_NO), "   //连连浇号(CAST号)
				"MAX(CAST_DIV_NO) " //浇次分割号
				"FROM TPSSM11 WHERE HEAT_NO = @htno "; //炼钢作业计划编制主表
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("htno", htno);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			l_steel_grade = cmd_inq.GetString(1);
			l_ccm_no = cmd_inq.GetString(2);
			l_cast_no = cmd_inq.GetString(3);
			l_cast_div_no = cmd_inq.GetDecimal(4);

		}
		cmd_inq.Close();

		//update  le_ld_usage_his
		//	set  cast_st = p_cast_st,
		//	steel_grade = l_steel_grade,
		//	ccm_no = l_ccm_no,
		//	cast_no = l_cast_no,
		//	cast_div_no = l_cast_div_no
		//where  ladle_no = l_ladle_no and cast_et is null
		//and  create_time + 7 > sysdate;  --wei.cx 2017 - 5 - 5 七天还没完成的计划，防止历史异常数据更新
			//czmes_m.trace(Imodule, czmes_constdef.entry_exit_lev, l_linenum, '3');
		sqlstr = "UPDATE HTMSM01 SET REC_REVISOR= '" + userid + "',"
			"REC_REVISE_TIME = '" + datetime + "',"
			"ST_NO = '" + l_steel_grade + "',"
			"ccm_no,cast_no,cast_div_no,cast_st在目前20230602都没有建履历表，也没有类似表字段，只存在于炼钢作业计划编制主表TPSSM11中"
			//"VIR_LADLE_AGE = " + vir_ladle_age.ToString() + ","
			//"COUNT_FILL_TIME = " + count_fill_time.ToString() + ","
			//"CROSS_LF_COUNT = " + cross_lf_count.ToString() + ","
			//"CROSS_LF_HEAT_TIME = '" + cross_lf_heat_time.ToString() + " "
			" WHERE LADLE_NO = '" + l_ladle_no + "'";

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
		bcls_rec->Tables["TMSM67"].Rows[0]["REMARK_1"] = "钢包开浇时间";  //时间操作
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


