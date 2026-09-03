/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-24 14:44:52
Description: 扇形段通钢量修改
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme68a1_upd)


int f_tmsme68a1_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	//CModel ttmsm66 = CModel("TTMSM66");
	CModel ttmsm68("TTMSM68");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 获得传入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm68.Reset();
			ttmsm68.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm68.TrimOrBlank();

			//Log::Trace("", __FUNCTION__, "ttmsm68.HEAT_NO		= [{0}]", (const char*)ttmsm69["HEAT_NO"].ToString());

			if (ttmsm68["SEQ_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "序号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm68["STRAND_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "流号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm68["CC_MACH_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "铸机号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 查询该序列号是否存在 */
			if (ttmsm68.QueryCount("SEQ_NO") <= 0)
			{
				sprintf(s.msg, "设熔炼号[%s]不存在!", (const char*)ttmsm68["SEQ_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//------------------------------------------------------------------
			//ttmsm91["SEQ_NO"] = atoi(EPGetNextSeq("TMSME_91", conn));
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "select CODE_DESC_1_CONTENT FROM TSI00FORMCODESETS WHERE FORM_NAME='TMSME68' AND CODE_CLASS='CC_MACH_NO' AND CODE = @mach_no";
				break;
			}
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("mach_no", ttmsm68["CC_MACH_NO"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();

			if (cmd_inq.Read())
			{
				ttmsm68["REMARK_3"] = cmd_inq.GetString(1);
			}
			else
			{
				ttmsm68["REMARK_3"] = " ";
			}
			cmd_inq.Close();

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "select CODE_DESC_1_CONTENT FROM TSI00FORMCODESETS WHERE FORM_NAME='TMSME68' AND CODE_CLASS='STRAND_NO' AND CODE = @strand_no";
				break;
			}
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("strand_no", ttmsm68["STRAND_NO"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();

			if (cmd_inq.Read())
			{
				ttmsm68["REMARK_4"] = cmd_inq.GetString(1);
			}
			else
			{
				ttmsm68["REMARK_4"] = " ";
			}
			cmd_inq.Close();

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "select CODE_DESC_1_CONTENT FROM TSI00FORMCODESETS WHERE FORM_NAME='TMSME68' AND CODE_CLASS='STATUS' AND CODE = @status_no";
				break;
			}
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("status_no", ttmsm68["STATUS"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();

			if (cmd_inq.Read())
			{
				ttmsm68["REMARK_5"] = cmd_inq.GetString(1);
			}
			else
			{
				ttmsm68["REMARK_5"] = " ";
			}
			cmd_inq.Close();

			/* 修改工器具RH真空槽信息表 */
			ttmsm68["REC_REVISOR"] = s.userid;   //记录修改责任者
			ttmsm68["REC_REVISE_TIME"] = datetime;   //记录修改时刻

			sqlstr = "UPDATE TTMSM68 SET REC_REVISOR= '" + ttmsm68["REC_REVISOR"].ToString() + "',"
				"REC_REVISE_TIME = '" + ttmsm68["REC_REVISE_TIME"].ToString() + "',"
				"CC_MACH_NO= '" + ttmsm68["CC_MACH_NO"].ToString() + "',"					/*铸机号*/
				"STRAND_NO= '" + ttmsm68["STRAND_NO"].ToString() + "',"						//流号
				"DEV_NO= '" + ttmsm68["DEV_NO"].ToString() + "',"							//设备编号
				"ON_LINE_TIME= '" + ttmsm68["ON_LINE_TIME"].ToString() + "',"				//上线时间
				"OFF_LINE_TIME= '" + ttmsm68["OFF_LINE_TIME"].ToString() + "',"				//下线时间
				"DAYS= " + ttmsm68["DAYS"].ToString() + ","									//计划时间
				"TOTAL_DAYS= " + ttmsm68["TOTAL_DAYS"].ToString() + ","						//使用天数
				"PLAN_RT = " + ttmsm68["PLAN_RT"].ToString() + ","							//计划过钢量
				"PLAN_HAPPEN_WGT = " + ttmsm68["PLAN_HAPPEN_WGT"].ToString() + ","			//过钢量
				"STATUS = '" + ttmsm68["STATUS"].ToString() + "',"							//状态
				"REMARK_3 = '" + ttmsm68["REMARK_3"].ToString() + "',"						//铸机号
				"REMARK_4 = '" + ttmsm68["REMARK_4"].ToString() + "',"						//流号
				"REMARK_5 = '" + ttmsm68["REMARK_5"].ToString() + "',"						//状态
				"REMARK_2 = '" + ttmsm68["REMARK_2"].ToString() + "'"						//备注
				" WHERE SEQ_NO = " + ttmsm68["SEQ_NO"].ToString() + "";

			Log::Trace("", "", "sqlstr：{0}", sqlstr);
			Db::Execute(sqlstr);



		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}


