/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      杨扬
Version:     3.0
Date:        2014-07-07 09:39:03
Description: 钢包配包
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件

  
  

//外部函数声明

/*<remark>=========================================================
/// <summary>
/// 钢包配包
/// <para>
/// 根据传入的钢包号，炼钢计划号，完成配包操作，并新增数据进ttmsm02表。
/// </para>
/// <para>数据库表： ttmsm01   钢包基本信息表 </para>
/// <para>数据库表： ttmsm02   钢包配包信息表 </para>
/// <para>数据库表： tpssm11   炼钢计划信息表 </para>
/// </summary>
/// <param name="SM_PLAN_NO">炼钢计划号    </param>
/// <param name="LADLE_NO">钢包号       </param>
/// <returns>处理结果</returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(tmsm01a3_cbn)

int f_tmsm01a3_cbn(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	CTracer log(__FUNCTION__);  // 系统日志

    /* 程序内部变量 */
	CString sqlstr     = "" ;
	CString datetime   = "" ;
	CString datetemp   = "" ;
	CDecimal stop_use_time = 0;

	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);

	CModel ttmsm01("TTMSM01");
	CModel ttmsm02("TTMSM02");
	CModel tpssm11("TPSSM11");

	int doFlag = 0;

    try
    { 
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* ***** 获取输入参数 ***** */
		ttmsm01["LADLE_NO"]   = bcls_rec->Tables[1].Rows[0]["LADLE_NO"];
		ttmsm02["LADLE_NO"]   = bcls_rec->Tables[1].Rows[0]["LADLE_NO"];
		ttmsm02["SM_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["SM_PLAN_NO"];
		tpssm11["SM_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["SM_PLAN_NO"];

		ttmsm01["SM_UNIT_NO"] = "A";
		ttmsm02["SM_UNIT_NO"] = "A";


		//数据校核
		if(ttmsm02["LADLE_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg,_RES("钢包号不能为空。"));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if(ttmsm02["SM_PLAN_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg,_RES("炼钢计划号不能为空。"));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//if(ttmsm02.QueryCount("LADLE_NO")> 0)
		//{
		//	sprintf(s.msg,_RES("钢包号已被配包，无法再次配包。"));
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		if(ttmsm02.QueryCount("SM_PLAN_NO")> 0)
		{
			sprintf(s.msg,_RES("炼钢计划已配过包，无需再次配包。"));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
	
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = " SELECT   a.SM_PLAN_NO        , a.PONO             , a.ST_NO        "
					     "        , a.HEAT_NO           , a.REC_CREATE_TIME                   " 
					     "        , b.LADLE_NO          , b.LD_TYPE          , b.LADLE_LEVEL  "
					     "        , b.LADLE_STATUS      , b.LD_STATUS                         "
					     " FROM     TPSSM11 a    , TTMSM01 b           "
						 " WHERE    a.SM_PLAN_NO = @tpssm11.SM_PLAN_NO "
						 " AND      b.LADLE_NO   = @ttmsm02.LADLE_NO   "
						 ;
				break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tpssm11.SM_PLAN_NO",tpssm11["SM_PLAN_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm02.LADLE_NO",ttmsm02["LADLE_NO"].ToString());
		cmd_inq.SetCommandText(sqlstr);		
		cmd_inq.ExecuteReader();
	
		if(cmd_inq.Read()) 
		{
			ttmsm02["SM_PLAN_NO"]        = cmd_inq.GetString(1);
			ttmsm02["PONO"]              = cmd_inq.GetString(2);
			ttmsm02["ST_NO"]             = cmd_inq.GetString(3);
			ttmsm02["HEAT_NO"]           = cmd_inq.GetString(4);
			ttmsm02["REC_CREATE_TIME"]   = cmd_inq.GetString(5);
			ttmsm02["LADLE_NO"]          = cmd_inq.GetString(6);
			ttmsm01["LD_TYPE"]           = cmd_inq.GetString(7);
			ttmsm01["LADLE_LEVEL"]       = cmd_inq.GetString(8);
			ttmsm01["LADLE_STATUS"]      = cmd_inq.GetString(9);
			ttmsm01["LD_STATUS"]         = cmd_inq.GetString(10);
		}
		cmd_inq.Close();

		/*if(ttmsm01["LADLE_STATUS"].ToString().Trim() != "11")
		{
			sprintf(s.msg,_RES("钢包状态不为11（就绪），无法配包。"));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/

		ttmsm01["LADLE_NO"] = ttmsm02["LADLE_NO"];
		ttmsm01.Query("LADLE_NO");
		//--------------钢包等级判定开始--------------//
		if((ttmsm01["LADLE_LIFE"].ToDecimal() == 1)&&(ttmsm01["LADLE_LEVEL"].ToString().Trim() == ""))  //新包
		{
			ttmsm01["LADLE_LEVEL"] = "BXXX";
			ttmsm01["LD_STATUS"]   = "3";
		}
		else
		{			
			if(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "D")
			{
				ttmsm01["LADLE_LEVEL"] = ttmsm01["LADLE_LEVEL"];
				ttmsm01["LD_STATUS"]   = ttmsm01["LD_STATUS"];
			}
			else if((ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "A")||
				    (ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "B")||
					(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "C")  )
			{
				//获取钢包最近一次操作基准时间，若是 新包 或 小修中修后的钢包 烘烤后第一次使用，则取烘烤结束时间，否则取上次使用结束时间
				if(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "C")   //烘烤后赋C,使用后赋A
				{
					datetemp = ttmsm01["DRYING_ET"];
				}
				else
				{
					datetemp = ttmsm01["USAGE_ET"];
				}

				CDateTime dt1 = CDateTime:: Parse(datetime);
				CDateTime dt3 = CDateTime:: Parse(datetemp);

				CTimeSpan ts1 = dt1 - dt3;
				stop_use_time = ts1.Days() * 24 * 60 + ts1.Hours() * 60 + ts1.Minutes();

				if((stop_use_time <= 45)&&(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "A"))
				{
					ttmsm01["LADLE_LEVEL"] = "A";
					ttmsm01["LD_STATUS"]   = "0";
				}
				if((stop_use_time <= 45)&&(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "C")) //维修或新包烤包第一次使用，C为烘烤时赋值
				{
					ttmsm01["LADLE_LEVEL"] = "C" + stop_use_time.ToString().Trim();
					ttmsm01["LD_STATUS"]   = "0";
				}
				if((stop_use_time > 45)&&(stop_use_time <= 70))
				{
					ttmsm01["LADLE_LEVEL"] = "B" + stop_use_time.ToString().Trim();
					ttmsm01["LD_STATUS"]   = "0";
				}
				if((stop_use_time > 70)&&(stop_use_time <= 150))
				{
					ttmsm01["LADLE_LEVEL"] = "B" + stop_use_time.ToString().Trim();
					ttmsm01["LD_STATUS"]   = "1";
				}
				if((stop_use_time > 150)&&(stop_use_time <= 999))
				{
					ttmsm01["LADLE_LEVEL"] = "B" + stop_use_time.ToString().Trim();
					ttmsm01["LD_STATUS"]   = "2";
				}
				if(stop_use_time > 999)
				{
					ttmsm01["LADLE_LEVEL"] = "BXXX";
					ttmsm01["LD_STATUS"]   = "2";
				}
			}
			else
			{
				/*sprintf(s.msg,"钢包等级[%s]错误!",(const char*)ttmsm01["LADLE_LEVEL"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);*/

				ttmsm01["LADLE_LEVEL"] = "BXXX";
				ttmsm01["LD_STATUS"]   = "2";
			}
		}
		//--------------钢包等级判定结束--------------//

		ttmsm02["DEV_CODE"]        = "0";
		ttmsm02["LADLE_LEVEL"]     = ttmsm01["LADLE_LEVEL"];
		ttmsm02["REC_CREATOR"]     = s.userid;
		ttmsm02["REC_CREATE_TIME"] = datetime;

		ttmsm02.Insert();

		//更新ttmsm01 钢包状态  21-配包
		ttmsm01["LADLE_STATUS"] = "21";
		ttmsm01.Update("LADLE_STATUS","LADLE_NO");

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,"数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace(1,1, "[%s]", s.sysmsg);
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
