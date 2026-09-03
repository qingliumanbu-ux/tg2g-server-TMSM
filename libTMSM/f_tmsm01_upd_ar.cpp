/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-29 15:59:40
Description: 修改氩站底吹情况--参考二炼钢tmsm01_upd_ar 7120
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT


int f_tmsm01_upd_ar(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString	datetemp("");
	CString	date_time = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal stop_use_time = 0;
	CString	factory_div =" ";

	/* 实体类定义 */
	CModel ttmsm01("TTMSM01");
	
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
			//factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
			if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			{
				factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
				ttmsm01["FACTORY_DIV"] = factory_div;
			}
			else
			{
				ttmsm01["FACTORY_DIV"] = "A20";
			}
				
			ttmsm01.Reset();
			ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm01.TrimOrBlank();

			Log::Trace("", __FUNCTION__, "ttmsm01.FACTORY_DIV		= [{0}]", (const char*)ttmsm01["FACTORY_DIV"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm01.LADLE_NO		= [{0}]", (const char*)ttmsm01["LADLE_NO"].ToString());

			if (ttmsm01["FACTORY_DIV"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "炼钢厂别不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm01["LADLE_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "钢包号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
	
			/* 查询该序列号是否存在 */
			if (ttmsm01.QueryCount("LADLE_NO") <= 0)
			{
				sprintf(s.msg, "钢包号[%s]不存在!", (const char*)ttmsm01["LADLE_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				//sqlstr = "select ladle_status,use_seq_no FROM TTMSM01 WHERE SM_UNIT_NO = @sm_unit_no  AND LADLE_NO = @ladle_no";
				sqlstr = "select ladle_status,use_seq_no FROM TTMSM01 WHERE FACTORY_DIV = '" + ttmsm01["FACTORY_DIV"].ToString() + "'  AND LADLE_NO = '" + ttmsm01["LADLE_NO"].ToString() + "'";
				break;
			}
			cmd_inq.Parameters.Clear();
			//cmd_inq.Parameters.Set("sm_unit_no", ttmsm01["SM_UNIT_NO"].ToString());
			//cmd_inq.Parameters.Set("ladle_no", ttmsm01["LADLE_NO"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", "", "sqlstr语句：{0}", sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				ttmsm01["LADLE_STATUS"] = cmd_inq.GetString(1);
				ttmsm01["USE_SEQ_NO"] = cmd_inq.GetDecimal(2);
			}
			else
			{
				ttmsm01["LADLE_STATUS"] = " ";
				ttmsm01["USE_SEQ_NO"] = 0;
			}
			cmd_inq.Close();
			Log::Trace("", __FUNCTION__, "ttmsm01.USE_SEQ_NO		= [{0}]", (const char*)ttmsm01["USE_SEQ_NO"].ToString());


			/* 修改钢包基本信息表*/
			ttmsm01["REC_REVISOR"] = s.userid;   //记录修改责任者
			ttmsm01["REC_REVISE_TIME"] = datetime;   //记录修改时刻

			//UPDATE TTMSM01 /*钢包基本信息表*/
			//	SET
			//	REC_REVISE_TIME = :ttmsm01.rec_revise_time  /*记录修改时刻*/,
			//	REC_REVISOR = : ttmsm01.rec_revisor  /*记录修改责任者*/,
			//	AR_BOTTOM_BLOW_INS = : ttmsm01.ar_bottom_blow_ins  /*氩站底吹情况*/
			//	WHERE SM_UNIT_NO = : ttmsm01.sm_unit_no /*炼钢单元号*/
			//	AND LADLE_NO = : ttmsm01.ladle_no /*钢包号*/;

			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + ttmsm01["REC_REVISOR"].ToString() + "',"
				"REC_REVISE_TIME = '" + ttmsm01["REC_REVISE_TIME"].ToString() + "',"
				//"AR_BLOW_TIME_TOTAL= " + ttmsm01["AR_BLOW_TIME_TOTAL"].ToString() + " "	
				"BLOW_AR_RESULT= '" + ttmsm01["BLOW_AR_RESULT"].ToString() + "' "	
				" WHERE FACTORY_DIV = '" + ttmsm01["FACTORY_DIV"].ToString() + "' AND LADLE_NO = '" + +ttmsm01["LADLE_NO"].ToString() + "'";

			Log::Trace("", "", "sqlstr：{0}", sqlstr);
			Db::Execute(sqlstr);

			/*修改钢包基本信息历史表*/
			//UPDATE TTMSM10 /*钢包基本信息历史表*/
			//	SET
			//	REC_REVISE_TIME = :ttmsm01.rec_revise_time  /*记录修改时刻*/,
			//	REC_REVISOR = : ttmsm01.rec_revisor  /*记录修改责任者*/,
			//	AR_BOTTOM_BLOW_INS = : ttmsm01.ar_bottom_blow_ins  /*氩站底吹情况*/
			//	WHERE SM_UNIT_NO = : ttmsm01.sm_unit_no /*炼钢单元号*/
			//	AND LADLE_NO = : ttmsm01.ladle_no /*钢包号*/
			//	AND USE_SEQ_NO = : ttmsm01.use_seq_no;

			//暂时没发现钢包基本信息历史表
			//sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + ttmsm01["REC_REVISOR"].ToString() + "',"
			//	"REC_REVISE_TIME = '" + ttmsm01["REC_REVISE_TIME"].ToString() + "',"
			//	"AR_BLOW_TIME_TOTAL= " + ttmsm01["AR_BLOW_TIME_TOTAL"].ToString() + " "
			//	" WHERE SM_UNIT_NO = '" + ttmsm01["SM_UNIT_NO"].ToString() + "' AND LADLE_NO = '" + +ttmsm01["LADLE_NO"].ToString() + "'";

			//Log::Trace("", "", "sqlstr：{0}", sqlstr);
			//Db::Execute(sqlstr);

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


