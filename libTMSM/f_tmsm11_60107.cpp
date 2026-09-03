/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-31 16:21:06
Description: 更新钢包状态跟踪位置
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT

//记录履历
int f_tmsm67_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_tmsm11_60107(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString userid = s.userid;

	CString p_htno = " ";
	CString p_case = " ";
	CString l_ladle_no = " ";

	/* 实体类定义 */
	//CModel ttmsm01("TTMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获得传入参数 */
		p_htno = bcls_rec->Tables[0].Rows[0]["P_HTNO"].ToString().Trim();
		p_case = bcls_rec->Tables[0].Rows[0]["P_CASE"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "需要更新状态的钢包号 = [{0}]", p_htno);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "select MAX(LADLE_NO) FROM TTMSM01 WHERE HEAT_NO= @htno";
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("htno", p_htno);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			l_ladle_no = cmd_inq.GetString(1);
		}
		else
		{
			l_ladle_no = " ";
			strcpy(s.msg, "钢包号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cmd_inq.Close();

		//--区域为1#转炉
		if (p_case == "1")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '1' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为2#转炉
		if (p_case == "2")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '2' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为1#氩站
		if (p_case == "3")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '3' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为2#氩站
		if (p_case == "4")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '4' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为1#LF东
		if (p_case == "5")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '5' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为1#RH东
		if (p_case == "6")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '6' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为1#CC
		if (p_case == "7")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '7' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//-- --区域为2#CC
		if (p_case == "8")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '8' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为3#转炉
		if (p_case == "11")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '11' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		// --3#吹氩站
		if (p_case == "12")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '12' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为1#LF西
		if (p_case == "13")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '13' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为1#RH西
		if (p_case == "14")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '14' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为2#LF东
		if (p_case == "15")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '15' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为2#LF西
		if (p_case == "16")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '16' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为2#RH东
		if (p_case == "17")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '17' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//--区域为2#RH西
		if (p_case == "18")
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"LD_POS= '18' "
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

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
		bcls_rec->Tables["TMSM67"].Rows[0]["REMARK_1"] = "更新钢包位置";  //时间操作
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


