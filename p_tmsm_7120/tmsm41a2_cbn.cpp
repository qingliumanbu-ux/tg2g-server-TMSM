/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      杨扬
Version:     3.0
Date:        2014-07-07 09:39:03
Description: 铜板装配
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件

  

//外部函数声明

/*<remark>=========================================================
/// <summary>
/// 铜板配包
/// <para>
/// 根据传入的铜板号，炼钢计划号，完成装配操作，并新增数据进ttmsm42表。
/// </para>
/// <para>数据库表： ttmsm41   铜板基本信息表 </para>
/// <para>数据库表： ttmsm42   铜板装配信息表 </para>
/// </summary>
/// <param name="SM_PLAN_NO">炼钢计划号    </param>
/// <param name="LADLE_NO">铜板号       </param>
/// <returns>处理结果</returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(tmsm41a2_cbn)

int f_tmsm41a2_cbn(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	CTracer log(__FUNCTION__);  // 系统日志

	/* 程序内部变量 */
	CString sqlstr = "";
	CString datetime = "";
	CString datetemp = "";
	CDecimal stop_use_time = 0;

	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);

	CModel ttmsm41("TTMSM41");
	CModel ttmsm42("TTMSM42");

	int doFlag = 0;
	CString TM_COPPER_NO = "";
	CString TM_COPPER_NO1 = "";
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* ***** 获取输入参数 ***** */
		//东侧或固定侧数据获取
		ttmsm41["BACK_C1"] = bcls_rec->Tables[1].Rows[0]["BACK_C1"];//获取安装侧
		TM_COPPER_NO = bcls_rec->Tables[1].Rows[0]["TM_COPPER_NO"];
		TM_COPPER_NO1 = bcls_rec->Tables[2].Rows[0]["TM_COPPER_NO"];
		Log::Trace("", __FUNCTION__, "ttmsm41.BACK_C1		= [{0}]", ttmsm41["BACK_C1"].ToString());
		if (ttmsm41["BACK_C1"] = "01")
		{
			ttmsm42["TM_FIXED_SIDE_NUMBER"] = bcls_rec->Tables[1].Rows[0]["TM_COPPER_NO"];
			Log::Trace("", __FUNCTION__, "ttmsm42.TM_FIXED_SIDE_NUMBER		= [{0}]", ttmsm42["TM_FIXED_SIDE_NUMBER"].ToString());
		}
		else if (ttmsm41["BACK_C1"] = "03")
		{
			ttmsm42["TM_EAST_NUMBER"] = bcls_rec->Tables[1].Rows[0]["TM_COPPER_NO"];
			Log::Trace("", __FUNCTION__, "ttmsm42.TM_EAST_NUMBER		= [{0}]", ttmsm42["TM_EAST_NUMBER"].ToString());
		}
		ttmsm42["TM_MILLING_TIME"] = bcls_rec->Tables[1].Rows[0]["TM_MILLING_TIME"];
		ttmsm42["TM_MILLING_AMOUNT"] = bcls_rec->Tables[1].Rows[0]["TM_MILLING_AMOUNT"];
		ttmsm42["TM_ACTUAL_THICKNESS"] = bcls_rec->Tables[1].Rows[0]["TM_ACTUAL_THICKNESS"];
		ttmsm42["MD_COPPER_PLATE_NO"] = bcls_rec->Tables[1].Rows[0]["MD_COPPER_PLATE_NO"];
		//西侧侧或活动侧数据获取
		ttmsm41["BACK_C1"] = bcls_rec->Tables[2].Rows[0]["BACK_C1"];//获取安装侧
		Log::Trace("", __FUNCTION__, "ttmsm41.BACK_C1活动西		= [{0}]", ttmsm41["BACK_C1"].ToString());
		if (ttmsm41["BACK_C1"] = "02")
		{
			ttmsm42["TM_ACTIVITY_SIDE_NUMBER"] = bcls_rec->Tables[2].Rows[0]["TM_COPPER_NO"];
			Log::Trace("", __FUNCTION__, "ttmsm42.TM_ACTIVITY_SIDE_NUMBER活动西		= [{0}]", ttmsm42["TM_ACTIVITY_SIDE_NUMBER"].ToString());
		}
		else if (ttmsm41["BACK_C1"] = "04")
		{
			ttmsm42["TM_WEST_NUMBER"] = bcls_rec->Tables[2].Rows[0]["TM_COPPER_NO"];
			Log::Trace("", __FUNCTION__, "ttmsm42.TM_WEST_NUMBER活动西		= [{0}]", ttmsm42["TM_WEST_NUMBER"].ToString());
		}
		ttmsm42["TM_MILLING_TIME_1"] = bcls_rec->Tables[2].Rows[0]["TM_MILLING_TIME"];
		ttmsm42["TM_MILLING_AMOUNT_1"] = bcls_rec->Tables[2].Rows[0]["TM_MILLING_AMOUNT"];
		ttmsm42["TM_ACTUAL_THICKNESS_1"] = bcls_rec->Tables[2].Rows[0]["TM_ACTUAL_THICKNESS"];

		ttmsm42["OFF_LINE_TIME"] = bcls_rec->Tables[0].Rows[0]["OFF_LINE_TIME"];
		ttmsm42["TM_CASTING_MACHINE"] = bcls_rec->Tables[0].Rows[0]["TM_CASTING_MACHINE"];
		ttmsm42["TM_CRACK_CONDITION"] = bcls_rec->Tables[0].Rows[0]["TM_CRACK_CONDITION"];
		ttmsm42["TM_INSTALLATION_LOCATION"] = bcls_rec->Tables[0].Rows[0]["TM_INSTALLATION_LOCATION"];
		ttmsm42["TM_NORMAL_MILLING"] = bcls_rec->Tables[0].Rows[0]["TM_NORMAL_MILLING"];
		ttmsm42["TM_NUMBERFURNACE"] = bcls_rec->Tables[0].Rows[0]["TM_NUMBERFURNACE"];
		ttmsm42["TOTAL_CHARGE_NUM"] = bcls_rec->Tables[0].Rows[0]["TOTAL_CHARGE_NUM"];
		ttmsm42["REMARK"] = bcls_rec->Tables[0].Rows[0]["REMARK"];
		ttmsm42["TM_COPPER_TYPE"] = bcls_rec->Tables[1].Rows[0]["TM_COPPER_TYPE"];
		//数据校核
		if (TM_COPPER_NO.Trim() == "" || TM_COPPER_NO1.Trim() == "")
		{
			sprintf(s.msg, _RES("铜板号不能为空。"));
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		

		ttmsm42["REC_CREATOR"] = s.userid;
		ttmsm42["REC_CREATE_TIME"] = datetime;

		ttmsm42.Insert();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace(1,1, "[%s]", s.sysmsg);
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
