/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    KE2111
Version:    1.0
Date:     2024-02-16
Description:
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"

/******后台pc文件标准注释标记*****/

/* ***** 静态函数申明 ***** */


/*<remark>=========================================================

===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm121av_ins1)

int f_tmsm121av_ins1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义	

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int doflag = 0;
	int fetchRowCount = 0;
	int i;
	int ret;
	int RowCount = 0;
	int blkNum = 0;
	CDecimal i_count = 0;
	CDecimal i_status = 0;
	CString v_shift_no("");
	CString v_shift_group("");
	CString sqlstr("");
	CString sqlstr1("");

	CModel ttmsm12("TTMSM12");
	CModel tmmsm27("TMMSM27");
	CModel tmmsm21("TMMSM21");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass tmp;


	try
	{
		Log::Trace("", "", "条件查询sql {0}++[{1}],", bcls_rec->Tables.get_Count());
		Log::Trace("", "", "条件查询sql {0}++[{1}],", bcls_rec->Tables[0].Rows.get_Count());
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "熔炼号不能为空！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm12.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", ttmsm12["HEAT_NO"].ToString());
			if (ttmsm12.QueryCount("HEAT_NO") > 0)
			{
				sprintf(s.msg, "熔炼号[%s]已存在！", (const char*)ttmsm12["HEAT_NO"]);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm12["REC_CREATE_TIME"] = datetimeNow;
			ttmsm12["REC_CREATOR"] = s.userid;
			ttmsm12["C_DIV"] = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString();
			ttmsm12["FURNACE_NO"] = bcls_rec->Tables[0].Rows[0]["FURNACE_NO"].ToString();
			ttmsm12["DATE_C"] = ttmsm12["DATE_C"].ToString().SubstringNE(0, 8);
			

			tmmsm21["HEAT_NO"] = ttmsm12["HEAT_NO"];
			tmmsm27["HEAT_NO"] = ttmsm12["HEAT_NO"];
			if (ttmsm12["LADLE_NO"].ToString().Trim() != ""&& ttmsm12["LADLE_LIFE"].ToString().Trim() != "")
			{
				if (ttmsm12["LADLE_LIFE"].ToString().Trim() != "1")
				{
					CString v_pre_cast_end = Db::QueryCString("select CAST_END_TIME from ttmsm12 A where LADLE_NO = '" + ttmsm12["LADLE_NO"].ToString().Trim() + "' AND LD_TYPE = 'TS' and REC_CREATE_TIME = (select max(REC_CREATE_TIME) from ttmsm12 where LADLE_NO ='" + ttmsm12["LADLE_NO"].ToString().Trim() + "' AND LD_TYPE='TS' AND CAST_END_TIME!=' ') ");
					ttmsm12["PRE_HEAT_CAST_E_TIME"] = v_pre_cast_end.TrimOrBlank();
				}
			}
			
			if (ttmsm12["C_DIV"].ToString()=="A" &&tmmsm27.QueryCount("HEAT_NO")>0)
			{
				sqlstr = " SELECT * FROM TMMSM27 WHERE HEAT_NO='" + ttmsm12["HEAT_NO"].ToString() + "' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm27);
					ttmsm12["ST_NO"] = tmmsm27["ST_NO"];
					ttmsm12["OUT_STEEL_TIME"] = tmmsm27["TAP_START_TIME"];
					ttmsm12["LADLE_GROSS_WT"] = tmmsm27["LADLE_DEPART_WT"];
				}
			}
			if (ttmsm12["C_DIV"].ToString() == "B" && tmmsm21.QueryCount("HEAT_NO") > 0)
			{
				sqlstr = " SELECT * FROM TMMSM21 WHERE HEAT_NO='" + ttmsm12["HEAT_NO"].ToString() + "' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm21);
					ttmsm12["ST_NO"] = tmmsm21["ST_NO"];
					ttmsm12["OUT_STEEL_TIME"] = tmmsm21["TAP_START_TIME"];
					ttmsm12["LADLE_GROSS_WT"] = tmmsm21["LADLE_DEPART_WT"];
				}
			}
			if (ttmsm12["OUT_STEEL_TIME"].ToString().Trim() != "")
			{
				ttmsm12["FLAG_CAL_OUT_STL_T"] = "0";
			}
			else
			{
				if (ttmsm12["PRE_HEAT_CAST_E_TIME"].ToString().Trim() != "")
				{
					ttmsm12["OUT_STEEL_TIME"] = Db::QueryCString("SELECT TO_CHAR(TO_DATE('" + ttmsm12["PRE_HEAT_CAST_E_TIME"].ToString().Trim() + "', 'YYYY-MM-DD HH24:MI:SS') + 120 / 1440, 'yyyyMMddHH24miss') from dual");
					ttmsm12["FLAG_CAL_OUT_STL_T"] = "1";
				}
			}
			CString v_cast_end = Db::QueryCString(" SELECT END_TIME FROM TMMSM31 WHERE HEAT_NO='" + ttmsm12["HEAT_NO"].ToString() + "' ");
			ttmsm12["CAST_END_TIME"] = v_cast_end.TrimOrBlank();
			if (ttmsm12["LADLE_NO"].ToString().Trim()!=""&& ttmsm12["C_DIV"].ToString() == "A")
			{
				ttmsm12["BACK_C1"] = Db::QueryCString(" SELECT WORK_MAKER FROM TTMSM11 WHERE LADLE_NO='TS"+ ttmsm12["LADLE_NO"].ToString() +"' AND RESUME_SEQ_NO =(SELECT MAX(RESUME_SEQ_NO) FROM TTMSM11 WHERE LADLE_NO='TS"+ ttmsm12["LADLE_NO"].ToString() +"') ");
			}
			Log::Trace("", __FUNCTION__, "202508011= [{0}]", ttmsm12["PRE_HEAT_CAST_E_TIME"].ToString());
			Log::Trace("", __FUNCTION__, "202508012= [{0}]", ttmsm12["FLAG_CAL_OUT_STL_T"].ToString());
			Log::Trace("", __FUNCTION__, "202508013= [{0}]", ttmsm12["OUT_STEEL_TIME"].ToString());
			ttmsm12.Insert();
		}
		for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables[1].Rows[i]["HEAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "熔炼号不能为空！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm12.MergeFrom(bcls_rec->Tables[1].Rows[i]);
			Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", ttmsm12["HEAT_NO"].ToString());
			if (ttmsm12.QueryCount("HEAT_NO") > 0)
			{
				sprintf(s.msg, "熔炼号[%s]已存在！",(const char*)ttmsm12["HEAT_NO"]);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm12["REC_CREATE_TIME"] = datetimeNow;
			ttmsm12["REC_CREATOR"] = s.userid;
			ttmsm12["C_DIV"] = bcls_rec->Tables[1].Rows[0]["C_DIV"].ToString();
			ttmsm12["FURNACE_NO"] = bcls_rec->Tables[1].Rows[0]["FURNACE_NO"].ToString();
			ttmsm12["DATE_C"] = ttmsm12["DATE_C"].ToString().SubstringNE(0, 8);

			tmmsm21["HEAT_NO"] = ttmsm12["HEAT_NO"];
			tmmsm27["HEAT_NO"] = ttmsm12["HEAT_NO"];
			if (ttmsm12["LADLE_NO"].ToString().Trim() != ""&& ttmsm12["LADLE_LIFE"].ToString().Trim() != "")
			{ 
				if (ttmsm12["LADLE_LIFE"].ToString().Trim()!="1")
				{ 
					CString v_pre_cast_end = Db::QueryCString("select CAST_END_TIME from ttmsm12 A where LADLE_NO = '" + ttmsm12["LADLE_NO"].ToString().Trim() + "' AND LD_TYPE = 'TS' and REC_CREATE_TIME = (select max(REC_CREATE_TIME) from ttmsm12 where LADLE_NO ='" + ttmsm12["LADLE_NO"].ToString().Trim() + "' AND LD_TYPE='TS' AND CAST_END_TIME!=' ') ");
					ttmsm12["PRE_HEAT_CAST_E_TIME"] = v_pre_cast_end.TrimOrBlank();
				}
			}
			if (ttmsm12["C_DIV"].ToString() == "A" && tmmsm27.QueryCount("HEAT_NO") > 0)
			{
				sqlstr = " SELECT * FROM TMMSM27 WHERE HEAT_NO='" + ttmsm12["HEAT_NO"].ToString() + "' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm27);
					ttmsm12["ST_NO"] = tmmsm27["ST_NO"];
					ttmsm12["OUT_STEEL_TIME"] = tmmsm27["TAP_START_TIME"];
					ttmsm12["LADLE_GROSS_WT"] = tmmsm27["LADLE_DEPART_WT"];
				}
			}
			if (ttmsm12["C_DIV"].ToString() == "B" && tmmsm21.QueryCount("HEAT_NO") > 0)
			{
				sqlstr = " SELECT * FROM TMMSM21 WHERE HEAT_NO='" + ttmsm12["HEAT_NO"].ToString() + "' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm21);
					ttmsm12["ST_NO"] = tmmsm21["ST_NO"];
					ttmsm12["OUT_STEEL_TIME"] = tmmsm21["TAP_START_TIME"];
					ttmsm12["LADLE_GROSS_WT"] = tmmsm21["LADLE_DEPART_WT"];
				}
			}
			if (ttmsm12["OUT_STEEL_TIME"].ToString().Trim() != "")
			{
				ttmsm12["FLAG_CAL_OUT_STL_T"] = "0";
			}
			else
			{
				if (ttmsm12["PRE_HEAT_CAST_E_TIME"].ToString().Trim() != "")
				{
					ttmsm12["OUT_STEEL_TIME"] = Db::QueryCString("SELECT TO_CHAR(TO_DATE('" + ttmsm12["PRE_HEAT_CAST_E_TIME"].ToString().Trim() + "', 'YYYY-MM-DD HH24:MI:SS') + 120 / 1440, 'yyyyMMddHH24miss') from dual");
					ttmsm12["FLAG_CAL_OUT_STL_T"] = "1";
				}
			}
			CString v_cast_end = Db::QueryCString(" SELECT END_TIME FROM TMMSM31 WHERE HEAT_NO='" + ttmsm12["HEAT_NO"].ToString() + "' ");
			ttmsm12["CAST_END_TIME"] = v_cast_end.TrimOrBlank();
			if (ttmsm12["LADLE_NO"].ToString().Trim() != ""&& ttmsm12["C_DIV"].ToString() == "A")
			{
				ttmsm12["BACK_C1"] = Db::QueryCString(" SELECT WORK_MAKER FROM TTMSM11 WHERE LADLE_NO='TS" + ttmsm12["LADLE_NO"].ToString() + "' AND RESUME_SEQ_NO =(SELECT MAX(RESUME_SEQ_NO) FROM TTMSM11 WHERE LADLE_NO='TS" + ttmsm12["LADLE_NO"].ToString() + "') ");
			}
			Log::Trace("", __FUNCTION__, "202508014= [{0}]", ttmsm12["PRE_HEAT_CAST_E_TIME"].ToString());
			Log::Trace("", __FUNCTION__, "202508015= [{0}]", ttmsm12["FLAG_CAL_OUT_STL_T"].ToString());
			Log::Trace("", __FUNCTION__, "202508016= [{0}]", ttmsm12["OUT_STEEL_TIME"].ToString());
			ttmsm12.Insert();
		}
		for (int i = 0; i < bcls_rec->Tables[2].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables[2].Rows[i]["HEAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "熔炼号不能为空！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm12.MergeFrom(bcls_rec->Tables[2].Rows[i]);
			Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", ttmsm12["HEAT_NO"].ToString());
			if (ttmsm12.QueryCount("HEAT_NO") > 0)
			{
				sprintf(s.msg, "熔炼号[%s]已存在！", (const char*)ttmsm12["HEAT_NO"]);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm12["REC_CREATE_TIME"] = datetimeNow;
			ttmsm12["REC_CREATOR"] = s.userid;
			ttmsm12["C_DIV"] = bcls_rec->Tables[2].Rows[0]["C_DIV"].ToString();
			ttmsm12["FURNACE_NO"] = bcls_rec->Tables[2].Rows[0]["FURNACE_NO"].ToString();
			ttmsm12["DATE_C"] = ttmsm12["DATE_C"].ToString().SubstringNE(0, 8);

			tmmsm21["HEAT_NO"] = ttmsm12["HEAT_NO"];
			tmmsm27["HEAT_NO"] = ttmsm12["HEAT_NO"];
			if (ttmsm12["LADLE_NO"].ToString().Trim() != ""&& ttmsm12["LADLE_LIFE"].ToString().Trim() != "")
			{
				if (ttmsm12["LADLE_LIFE"].ToString().Trim() != "1")
				{
					CString v_pre_cast_end = Db::QueryCString("select CAST_END_TIME from ttmsm12 A where LADLE_NO = '" + ttmsm12["LADLE_NO"].ToString().Trim() + "' AND LD_TYPE = 'TS' and REC_CREATE_TIME = (select max(REC_CREATE_TIME) from ttmsm12 where LADLE_NO ='" + ttmsm12["LADLE_NO"].ToString().Trim() + "' AND LD_TYPE='TS' AND CAST_END_TIME!=' ') ");
					ttmsm12["PRE_HEAT_CAST_E_TIME"] = v_pre_cast_end.TrimOrBlank();
				}
			}
			if (ttmsm12["C_DIV"].ToString() == "A" && tmmsm27.QueryCount("HEAT_NO") > 0)
			{
				sqlstr = " SELECT * FROM TMMSM27 WHERE HEAT_NO='" + ttmsm12["HEAT_NO"].ToString() + "' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm27);
					ttmsm12["ST_NO"] = tmmsm27["ST_NO"];
					ttmsm12["OUT_STEEL_TIME"] = tmmsm27["TAP_START_TIME"];
					ttmsm12["LADLE_GROSS_WT"] = tmmsm27["LADLE_DEPART_WT"];
				}
			}
			if (ttmsm12["C_DIV"].ToString() == "B" && tmmsm21.QueryCount("HEAT_NO") > 0)
			{
				sqlstr = " SELECT * FROM TMMSM21 WHERE HEAT_NO='" + ttmsm12["HEAT_NO"].ToString() + "' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm21);
					ttmsm12["ST_NO"] = tmmsm21["ST_NO"];
					ttmsm12["OUT_STEEL_TIME"] = tmmsm21["TAP_START_TIME"];
					ttmsm12["LADLE_GROSS_WT"] = tmmsm21["LADLE_DEPART_WT"];
				}
			}
			if (ttmsm12["OUT_STEEL_TIME"].ToString().Trim() != "")
			{
				ttmsm12["FLAG_CAL_OUT_STL_T"] = "0";
			}
			else
			{
				if (ttmsm12["PRE_HEAT_CAST_E_TIME"].ToString().Trim() != "")
				{
					ttmsm12["OUT_STEEL_TIME"] = Db::QueryCString("SELECT TO_CHAR(TO_DATE('" + ttmsm12["PRE_HEAT_CAST_E_TIME"].ToString().Trim() + "', 'YYYY-MM-DD HH24:MI:SS') + 120 / 1440, 'yyyyMMddHH24miss') from dual");
					ttmsm12["FLAG_CAL_OUT_STL_T"] = "1";
				}
			}
			CString v_cast_end = Db::QueryCString(" SELECT END_TIME FROM TMMSM31 WHERE HEAT_NO='" + ttmsm12["HEAT_NO"].ToString() + "' ");
			ttmsm12["CAST_END_TIME"] = v_cast_end.TrimOrBlank();
			if (ttmsm12["LADLE_NO"].ToString().Trim() != ""&& ttmsm12["C_DIV"].ToString() == "A")
			{
				ttmsm12["BACK_C1"] = Db::QueryCString(" SELECT WORK_MAKER FROM TTMSM11 WHERE LADLE_NO='TS" + ttmsm12["LADLE_NO"].ToString() + "' AND RESUME_SEQ_NO =(SELECT MAX(RESUME_SEQ_NO) FROM TTMSM11 WHERE LADLE_NO='TS" + ttmsm12["LADLE_NO"].ToString() + "') ");
			}
			Log::Trace("", __FUNCTION__, "202508017= [{0}]", ttmsm12["PRE_HEAT_CAST_E_TIME"].ToString());
			Log::Trace("", __FUNCTION__, "202508018= [{0}]", ttmsm12["FLAG_CAL_OUT_STL_T"].ToString());
			Log::Trace("", __FUNCTION__, "202508019= [{0}]", ttmsm12["OUT_STEEL_TIME"].ToString());
			ttmsm12.Insert();
		}
		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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



	return doFlag;

}
