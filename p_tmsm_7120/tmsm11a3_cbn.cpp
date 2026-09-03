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
/// 根据传入的钢包号，炼钢计划号，完成配包操作，并新增数据进ttmsm12表。
/// </para>
/// <para>数据库表： ttmsm11   钢包基本信息表 </para>
/// <para>数据库表： ttmsm12   钢包配包信息表 </para>
/// <para>数据库表： tpssm11   炼钢计划信息表 </para>
/// </summary>
/// <param name="SM_PLAN_NO">炼钢计划号    </param>
/// <param name="IRON_LADLE_NO">钢包号       </param>
/// <returns>处理结果</returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(tmsm11a3_cbn)

int f_tmsm11a3_cbn(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	CTracer log(__FUNCTION__);  // 系统日志

    /* 程序内部变量 */
	CString sqlstr     = "" ;
	CString datetime   = "" ;
	CString datetemp   = "" ;
	CDecimal stop_use_time = 0;

	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);

	CModel ttmsm11("TTMSM11");
	CModel ttmsm12("TTMSM12");
	CModel tpssm11("TPSSM11");

	int doFlag = 0;

    try
    { 
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* ***** 获取输入参数 ***** */
		ttmsm11["IRON_LADLE_NO"]   = bcls_rec->Tables[1].Rows[0]["IRON_LADLE_NO"];
		ttmsm12["IRON_LADLE_NO"]   = bcls_rec->Tables[1].Rows[0]["IRON_LADLE_NO"];
		ttmsm12["SM_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["SM_PLAN_NO"];
		tpssm11["SM_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["SM_PLAN_NO"];

		ttmsm11["SM_UNIT_NO"] = "A";
		ttmsm12["SM_UNIT_NO"] = "A";


		//数据校核
		if(ttmsm12["IRON_LADLE_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg,_RES("铁包号不能为空。"));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if(ttmsm12["SM_PLAN_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg,_RES("炼钢计划号不能为空。"));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//if(ttmsm12.QueryCount("IRON_LADLE_NO")> 0)
		//{
		//	sprintf(s.msg,_RES("钢包号已被配包，无法再次配包。"));
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		if(ttmsm12.QueryCount("SM_PLAN_NO")> 0)
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
					     "        , b.IRON_LADLE_NO      "
					     "        , b.MIT_STATUS      , b.LD_STATUS                         "
					     " FROM     TPSSM11 a    , TTMSM11 b           "
						 " WHERE    a.SM_PLAN_NO = @tpssm11.SM_PLAN_NO "
						 " AND      b.IRON_LADLE_NO   = @ttmsm12.IRON_LADLE_NO   "
						 ;
				break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tpssm11.SM_PLAN_NO", tpssm11["SM_PLAN_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm12.IRON_LADLE_NO", ttmsm12["IRON_LADLE_NO"].ToString());
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			ttmsm12["SM_PLAN_NO"] = cmd_inq.GetString(1);
			ttmsm12["PONO"] = cmd_inq.GetString(2);
			ttmsm12["ST_NO"] = cmd_inq.GetString(3);
			ttmsm12["HEAT_NO"] = cmd_inq.GetString(4);
			ttmsm12["REC_CREATE_TIME"] = cmd_inq.GetString(5);
			ttmsm12["IRON_LADLE_NO"] = cmd_inq.GetString(6);
			//ttmsm11["LD_TYPE"]           = cmd_inq.GetString(7);
			//ttmsm11["LADLE_LEVEL"]       = cmd_inq.GetString(8);
			ttmsm11["MIT_STATUS"] = cmd_inq.GetString(7);
			ttmsm11["LD_STATUS"] = cmd_inq.GetString(8);
		}
		cmd_inq.Close();

		/*if(ttmsm11["LADLE_STATUS"].ToString().Trim() != "11")
		{
		sprintf(s.msg,_RES("钢包状态不为11（就绪），无法配包。"));
		throw CApplicationException(-1, s.msg, s.svc_name);
		}*/

		ttmsm11["IRON_LADLE_NO"] = ttmsm12["IRON_LADLE_NO"];
		ttmsm11.Query("IRON_LADLE_NO");
		//--------------钢包等级判定开始--------------//
		//if((ttmsm11["LADLE_LIFE"].ToDecimal() == 1)&&(ttmsm11["LADLE_LEVEL"].ToString().Trim() == ""))  //新包
		//{
		//	ttmsm11["LADLE_LEVEL"] = "BXXX";
		//	ttmsm11["LD_STATUS"]   = "3";
		//}
		//else
		//{			
		//	if(ttmsm11["LADLE_LEVEL"].ToString().Substring(0,1) == "D")
		//	{
		//		ttmsm11["LADLE_LEVEL"] = ttmsm11["LADLE_LEVEL"];
		//		ttmsm11["LD_STATUS"]   = ttmsm11["LD_STATUS"];
		//	}
		//	else if((ttmsm11["LADLE_LEVEL"].ToString().Substring(0,1) == "A")||
		//		    (ttmsm11["LADLE_LEVEL"].ToString().Substring(0,1) == "B")||
		//			(ttmsm11["LADLE_LEVEL"].ToString().Substring(0,1) == "C")  )
		//	{
		//		//获取钢包最近一次操作基准时间，若是 新包 或 小修中修后的钢包 烘烤后第一次使用，则取烘烤结束时间，否则取上次使用结束时间
		//		if(ttmsm11["LADLE_LEVEL"].ToString().Substring(0,1) == "C")   //烘烤后赋C,使用后赋A
		//		{
		//			datetemp = ttmsm11["DRYING_ET"];
		//		}
		//		else
		//		{
		//			datetemp = ttmsm11["USAGE_ET"];
		//		}

		//		CDateTime dt1 = CDateTime:: Parse(datetime);
		//		CDateTime dt3 = CDateTime:: Parse(datetemp);

		//		CTimeSpan ts1 = dt1 - dt3;
		//		stop_use_time = ts1.Days() * 24 * 60 + ts1.Hours() * 60 + ts1.Minutes();

		//		if((stop_use_time <= 45)&&(ttmsm11["LADLE_LEVEL"].ToString().Substring(0,1) == "A"))
		//		{
		//			ttmsm11["LADLE_LEVEL"] = "A";
		//			ttmsm11["LD_STATUS"]   = "0";
		//		}
		//		if((stop_use_time <= 45)&&(ttmsm11["LADLE_LEVEL"].ToString().Substring(0,1) == "C")) //维修或新包烤包第一次使用，C为烘烤时赋值
		//		{
		//			ttmsm11["LADLE_LEVEL"] = "C" + stop_use_time.ToString().Trim();
		//			ttmsm11["LD_STATUS"]   = "0";
		//		}
		//		if((stop_use_time > 45)&&(stop_use_time <= 70))
		//		{
		//			ttmsm11["LADLE_LEVEL"] = "B" + stop_use_time.ToString().Trim();
		//			ttmsm11["LD_STATUS"]   = "0";
		//		}
		//		if((stop_use_time > 70)&&(stop_use_time <= 150))
		//		{
		//			ttmsm11["LADLE_LEVEL"] = "B" + stop_use_time.ToString().Trim();
		//			ttmsm11["LD_STATUS"]   = "1";
		//		}
		//		if((stop_use_time > 150)&&(stop_use_time <= 999))
		//		{
		//			ttmsm11["LADLE_LEVEL"] = "B" + stop_use_time.ToString().Trim();
		//			ttmsm11["LD_STATUS"]   = "2";
		//		}
		//		if(stop_use_time > 999)
		//		{
		//			ttmsm11["LADLE_LEVEL"] = "BXXX";
		//			ttmsm11["LD_STATUS"]   = "2";
		//		}
		//	}
		//	else
		//	{
		//		/*sprintf(s.msg,"钢包等级[%s]错误!",(const char*)ttmsm11["LADLE_LEVEL"].ToString());
		//		throw CApplicationException(-1, s.msg, s.svc_name);*/

		//		ttmsm11["LADLE_LEVEL"] = "BXXX";
		//		ttmsm11["LD_STATUS"]   = "2";
		//	}
		//}
		//--------------钢包等级判定结束--------------//

		ttmsm12["DEV_CODE"] = "0";
		//ttmsm12["LADLE_LEVEL"]     = ttmsm11["LADLE_LEVEL"];
		ttmsm12["REC_CREATOR"]     = s.userid;
		ttmsm12["REC_CREATE_TIME"] = datetime;

		ttmsm12.Insert();

		//更新ttmsm11 钢包状态  21-配包
		ttmsm11["MIT_STATUS"] = "21";
		ttmsm11.Update("MIT_STATUS","IRON_LADLE_NO");

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
