/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      杨扬
Version:     3.0
Date:        2014-07-07 09:39:03
Description: 钢包配包_强制替换
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件

  
  
  

//外部函数声明
int f_tmsm_trace(EIClass *bcls_rec,EIClass *bcls_ret,CDbConnection * conn);
//int f_hmzl06_snd(EIClass *bcls_rec,EIClass *bcls_ret,CDbConnection * conn);
//int f_hmjl05_snd(EIClass *bcls_rec,EIClass *bcls_ret,CDbConnection * conn);

/*<remark>=========================================================
/// <summary>
/// 钢包配包_强制替换
/// <para>
/// 根据传入的钢包号，炼钢计划号，完成强制替换操作。
///   1.更新 ttmsm02表 中钢包信息（换其他可用钢包）。
///   2.更新 ttmsm01表 中钢包信息（被替换钢包直接离线）。
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
BM2F_ENTERACE(tmsm01a3_upd)

int f_tmsm01a3_upd(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	CTracer log(__FUNCTION__);  // 系统日志

    /* 程序内部变量 */
	CString sqlstr        = "" ;
	CString ladle_no_on   = "" ;
	CString ladle_no_off  = "" ;
	CString datetime      = "" ;
	CString datetemp      = "" ;

	CDecimal stop_use_time = 0 ;

	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);

	CModel ttmsm01("TTMSM01");
	CModel ttmsm02("TTMSM02");
	CModel tpssm11("TPSSM11");
//	CModel tpssm11("TPSSM11");

	int doFlag = 0;

    try
    { 
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		int blkNum = bcls_rec->Tables.IndexOf("TM0099"); 
		if (blkNum <= 0)
		{ 
			bcls_rec->Tables.Add("TM0099");		
			bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING,"LADLE_NO");
			bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING,"EVENT_ID");
			bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING,"EVENT_DESC");
			bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING,"FUNC_ID");
			bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING,"SYSTEM_ID");
		}

		blkNum = bcls_rec->Tables.IndexOf("TMSM01A3"); 
		if (blkNum <= 0)
		{ 
			bcls_rec->Tables.Add("TMSM01A3");		
			bcls_rec->Tables["TMSM01A3"].Columns.Add(DT_STRING,"SM_PLAN_NO");
			bcls_rec->Tables["TMSM01A3"].Columns.Add(DT_STRING,"HEAT_NO");
			bcls_rec->Tables["TMSM01A3"].Columns.Add(DT_STRING,"LADLE_NO");
			bcls_rec->Tables["TMSM01A3"].Columns.Add(DT_STRING,"LD_STATUS");
			bcls_rec->Tables["TMSM01A3"].Columns.Add(DT_STRING,"LADLE_LEVEL");
		}

		/* ***** 获取输入参数 ***** */
		//若已确认 配包计划 对应钢包出现问题，强制用就绪状态钢包对其进行替换
		ladle_no_on   = bcls_rec->Tables[1].Rows[0]["LADLE_NO"];    //将被换上的钢包
		ladle_no_off  = bcls_rec->Tables[0].Rows[0]["LADLE_NO"];    //将被换下的钢包

		ttmsm02["SM_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["SM_PLAN_NO"];
		tpssm11["SM_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["SM_PLAN_NO"];
		ttmsm01["SM_UNIT_NO"] = "A";
		ttmsm02["SM_UNIT_NO"] = "A";

		//数据校核
		if(ladle_no_on.Trim() == "")
		{
			sprintf(s.msg,_RES("用于替换的可用钢包 的钢包号不能为空。"));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if(ladle_no_off.Trim() == "")
		{
			sprintf(s.msg,_RES("被替换的钢包 的钢包号不能为空。"));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if(ttmsm02["SM_PLAN_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg,_RES("炼钢计划号不能为空。"));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		ttmsm02["LADLE_NO"] = ladle_no_off;
		ttmsm02.Query("SM_PLAN_NO, LADLE_NO");
		if(ttmsm02["SM_PLAN_NO"].ToString().Trim() != tpssm11["SM_PLAN_NO"].ToString().Trim())
		{
			sprintf(s.msg,_RES("被换下钢包对应的配包信息不存在，无法进行强制替换。"));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		ttmsm02["LADLE_NO"] = ladle_no_on;   //被换上钢包数据校核
		//if(ttmsm02.QueryCount("LADLE_NO")> 0)
		//{
		//	sprintf(s.msg,_RES("用于替换的钢包号已被配包，无法强制替换。"));
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

		//修改被换下钢包信息ladle_no_off
		ttmsm01["LADLE_NO"]     = ladle_no_off; //被换下的钢包
		ttmsm01["LADLE_STATUS"] = "31";         //31-维修
		ttmsm01.Update("LADLE_STATUS","LADLE_NO");


		//修改被换上钢包信息ladle_no_on，此处ttmsm02结构体钢包号已被赋值为ladle_no_on，见程序约110行
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

		//if(ttmsm01["LADLE_STATUS"].ToString().Trim() != "11")
		//{
		//	sprintf(s.msg,_RES("被换上的钢包状态不为11（就绪），无法进行强制替换。"));
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

		ttmsm02.Update("LADLE_NO","SM_PLAN_NO");

		ttmsm01["LADLE_NO"]     = ladle_no_on;
		//--------------钢包等级判定开始--------------//
		ttmsm01.Query("LADLE_NO");

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
		ttmsm01["LADLE_STATUS"] = "22";
		ttmsm01.Update("LADLE_STATUS,LD_STATUS","LADLE_NO");
		//ttmsm01.Update("LADLE_STATUS,LADLE_LEVEL,LD_STATUS","LADLE_NO");

		tpssm11["LADLE_NO"] = ttmsm02["LADLE_NO"];
		tpssm11.Update("LADLE_NO","SM_PLAN_NO");

		tpssm11["SM_PLAN_NO"] = tpssm11["SM_PLAN_NO"];
		tpssm11["LADLE_NO"] = ttmsm02["LADLE_NO"];
		tpssm11.Update("LADLE_NO", "SM_PLAN_NO");

		//发送钢包配包信息
		bcls_rec->Tables["TMSM01A3"].Rows.Clear();
		bcls_rec->Tables["TMSM01A3"].Rows.Add();
		bcls_rec->Tables["TMSM01A3"].Rows[0]["LADLE_NO"]	 = ladle_no_on;
		bcls_rec->Tables["TMSM01A3"].Rows[0]["LD_STATUS"]	 = ttmsm01["LD_STATUS"];
		bcls_rec->Tables["TMSM01A3"].Rows[0]["LADLE_LEVEL"]  = ttmsm02["LADLE_LEVEL"];
		bcls_rec->Tables["TMSM01A3"].Rows[0]["HEAT_NO"]	     = ttmsm02["HEAT_NO"];
		bcls_rec->Tables["TMSM01A3"].Rows[0]["SM_PLAN_NO"]   = ttmsm02["SM_PLAN_NO"];
		//doFlag = f_hmzl06_snd(bcls_rec, bcls_ret,conn);
		////转炉L2
		//if(doFlag < 0)
		//{
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		////精炼L2
		//doFlag = f_hmjl05_snd(bcls_rec, bcls_ret,conn);
		//if(doFlag < 0)
		//{
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

		//记录钢包履历
		bcls_rec->Tables["TM0099"].Rows.Clear();
		bcls_rec->Tables["TM0099"].Rows.Add();
		bcls_rec->Tables["TM0099"].Rows[0]["LADLE_NO"]	 = ladle_no_off;
		bcls_rec->Tables["TM0099"].Rows[0]["EVENT_ID"]	 = "TMA5";
		bcls_rec->Tables["TM0099"].Rows[0]["EVENT_DESC"] = "钢包强制替换 LADLE_OFF";
		bcls_rec->Tables["TM0099"].Rows[0]["FUNC_ID"]	 = "tmsm01a3_upd";
		bcls_rec->Tables["TM0099"].Rows[0]["SYSTEM_ID"]  = "TMSM";
		doFlag = f_tmsm_trace(bcls_rec, bcls_ret,conn);
		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}	

		//记录钢包履历
		bcls_rec->Tables["TM0099"].Rows.Clear();
		bcls_rec->Tables["TM0099"].Rows.Add();
		bcls_rec->Tables["TM0099"].Rows[0]["LADLE_NO"]	 = ladle_no_on;
		bcls_rec->Tables["TM0099"].Rows[0]["EVENT_ID"]	 = "TMA4";
		bcls_rec->Tables["TM0099"].Rows[0]["EVENT_DESC"] = "钢包强制替换 LADLE_ON";
		bcls_rec->Tables["TM0099"].Rows[0]["FUNC_ID"]	 = "tmsm01a3_upd";
		bcls_rec->Tables["TM0099"].Rows[0]["SYSTEM_ID"]  = "TMSM";
		doFlag = f_tmsm_trace(bcls_rec, bcls_ret,conn);
		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}	
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
