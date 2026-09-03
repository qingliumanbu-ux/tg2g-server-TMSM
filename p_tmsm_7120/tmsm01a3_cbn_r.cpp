/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      杨扬
Version:     3.0
Date:        2014-07-07 09:39:03
Description: 钢包配包_回退
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件

  
  

//外部函数声明

/*<remark>=========================================================
/// <summary>
/// 钢包配包_回退
/// <para>
/// 根据传入的钢包号，炼钢计划号，完成配包回退操作，并删除ttmsm02表对应信息。
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
BM2F_ENTERACE(tmsm01a3_cbn_r)

int f_tmsm01a3_cbn_r(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	CTracer log(__FUNCTION__);  // 系统日志

    /* 程序内部变量 */
	CString sqlstr     = "" ;
	CString datetime   = "" ;

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
		ttmsm01["LADLE_NO"]   = bcls_rec->Tables[0].Rows[0]["LADLE_NO"];
		ttmsm02["LADLE_NO"]   = bcls_rec->Tables[0].Rows[0]["LADLE_NO"];
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
		ttmsm02.Query("SM_PLAN_NO, LADLE_NO");
		if(ttmsm02["SM_PLAN_NO"].ToString().Trim() != tpssm11["SM_PLAN_NO"].ToString().Trim())
		{
			sprintf(s.msg,_RES("对应配包信息不存在，无法进行配包确认。"));
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

		if(ttmsm01["LADLE_STATUS"].ToString().Trim() == "22")
		{
			sprintf(s.msg,_RES("配包计划已确认，无法进行配包回退。"));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		ttmsm02.Delete("SM_PLAN_NO, LADLE_NO");

		//更新ttmsm01 钢包状态  11-就绪
		ttmsm01["LADLE_STATUS"] = "11";
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
