/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-06-01 15:02:11
Description: 中间包结浇计算
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT

//记录履历
int f_tmsm67_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn); 
int f_tmsm21_60115(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString userid = s.userid;

	CString pono = " ";
	CString l_cs_no1 = " ";
	CString l_cs_no2 = " ";
	CDecimal l_tundish_total_age1 = 0;
	CDecimal l_tundish_total_age2 = 0;

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
		pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "需要更新状态的制造命令号 = [{0}]", pono);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "select MAX(TD_NO_1),MAX(TD_NO_2),MAX(TD_LIFE_1),MAX(TD_LIFE_2) FROM TMMSM31 WHERE PONO= @pono";
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("pono", pono);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			l_cs_no1 = cmd_inq.GetString(1);
			l_cs_no2 = cmd_inq.GetString(2);
			l_tundish_total_age1 = cmd_inq.GetDecimal(3);
			l_tundish_total_age2 = cmd_inq.GetDecimal(4);
		}
		else
		{
			l_cs_no1 = " ";
			l_cs_no2 = " ";
			l_tundish_total_age1 = 0;
			l_tundish_total_age2 = 0;
			//strcpy(s.msg, "1流结晶器号不能为空!");
			//throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cmd_inq.Close();


		if ((l_cs_no1.Trim().GetLength() == 0) && (l_cs_no2.Trim().GetLength() == 0))
		{
			strcpy(s.msg, "没有可用中间包!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

	
		sqlstr = "UPDATE TMMSM31 SET REC_REVISOR= '" + userid + "',"
			"REC_REVISE_TIME = '" + datetime + "',"
			"TD_LIFE_1= TD_LIFE_1 + 1 "  //中间包龄1
			" WHERE TD_NO_1 = '" + l_cs_no1.Trim() + "'";
		Log::Trace("", "", "sqlstr1：{0}", sqlstr);
		Db::Execute(sqlstr);

		sqlstr = "UPDATE TMMSM31 SET REC_REVISOR= '" + userid + "',"
			"REC_REVISE_TIME = '" + datetime + "',"
			"TD_LIFE_2= TD_LIFE_2 + 1 "  //中间包龄2
			" WHERE TD_NO_2 = '" + l_cs_no2.Trim() + "'";
		Log::Trace("", "", "sqlstr1：{0}", sqlstr);
		Db::Execute(sqlstr);

		//中间包基本信息表
		sqlstr = "UPDATE TTMSM21 SET REC_REVISOR= '" + userid + "',"
			"REC_REVISE_TIME = '" + datetime + "',"
			"LADLE_LIFE = LADLE_LIFE + 1 "  //中间包龄2
			" WHERE TD_NO = '" + l_cs_no1.Trim() + "' OR TD_NO = '" + l_cs_no2.Trim() + "' ";
		Log::Trace("", "", "sqlstr1：{0}", sqlstr);
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
		bcls_rec->Tables["TMSM67"].Rows[0]["REMARK_1"] = "l_cs_no中间包结浇计算";  //时间操作
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


