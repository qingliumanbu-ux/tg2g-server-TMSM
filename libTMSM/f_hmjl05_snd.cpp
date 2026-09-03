/* ****************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************	
*  程序名称			: f_hmjl05_snd
*  程序描述			: 配包计划信息（精炼L2）
*  备注说明			: 
*  编制人			：杨扬
*  修改历史			: 2014-06-10 	BM2IDE			(ADD)程序建立
*			... ...
* **************************************************************************** */
//框架公用头文件，勿删
#include "stdafx.h"




//程序用头文件
#include "epex.h"


//#include "xhmjl05.h"

int f_hmjl05_snd(EIClass *bcls_rec,EIClass *bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	CString	lpsz_tc_no = " ";	

	EPEX epex;
	CModel ttmsm02("TTMSM02");
	CModel ttmsm01("TTMSM01");
	//CHMJL05  xhmjl05(conn);

	CDbCommand cmd_inq(conn);
	CString sqlstr;

	try
	{
		/* ***** 获取输入参数 ***** */
		ttmsm02["SM_PLAN_NO"]  = bcls_rec->Tables["TMSM01A3"].Rows[0]["SM_PLAN_NO"].ToString().Trim();
		ttmsm02["HEAT_NO"]     = bcls_rec->Tables["TMSM01A3"].Rows[0]["HEAT_NO"].ToString().Trim();
		ttmsm02["LADLE_NO"]    = bcls_rec->Tables["TMSM01A3"].Rows[0]["LADLE_NO"].ToString().Trim();
		ttmsm01["LD_STATUS"]   = bcls_rec->Tables["TMSM01A3"].Rows[0]["LD_STATUS"].ToString().Trim();
		ttmsm02["LADLE_LEVEL"] = bcls_rec->Tables["TMSM01A3"].Rows[0]["LADLE_LEVEL"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "=== ttmsm02.SM_PLAN_NO = [{0}]",ttmsm02["SM_PLAN_NO"].ToString());

		lpsz_tc_no = "HMJL05";
		/* *****	发送电文开始 ***** */
		// 初始化
		if(epex.Initialize(lpsz_tc_no) < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if(epex.SetValue("sm_plan_no", 0, ttmsm02["SM_PLAN_NO"].ToString())<0)
		{
			strcpy(s.msg, _RES("GCRSS0000015")/*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(epex.SetValue("heat_no", 0, ttmsm02["HEAT_NO"].ToString())<0)
		{
			strcpy(s.msg, _RES("GCRSS0000015")/*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(epex.SetValue("ladle_no", 0, ttmsm02["LADLE_NO"].ToString())<0)
		{
			strcpy(s.msg, _RES("GCRSS0000015")/*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(epex.SetValue("ladle_status", 0, ttmsm01["LD_STATUS"].ToString())<0)   //电文配置为LADLE_STATUS,实际传入为LD_STATUS包况
		{
			strcpy(s.msg, _RES("GCRSS0000015")/*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(epex.SetValue("ladle_level", 0, ttmsm02["LADLE_LEVEL"].ToString())<0)
		{
			strcpy(s.msg, _RES("GCRSS0000015")/*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		// 发送电文
		if(epex.SendTele()<0)
		{
			sprintf(s.sysmsg,_RES("GCRSS0000032")/*电文发送失败。*/);
			strcpy(s.msg,  _RES("GCRSS0000032")/*电文发送失败。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		// 释放
		epex.Uninitialize();

		/* *****	发送电文end ***** */
		doFlag = 0;
		s.sqlcode = 0;
		strcpy(s.msg ,_RES("GCRSS0000002"));/*操作成功。*/
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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
