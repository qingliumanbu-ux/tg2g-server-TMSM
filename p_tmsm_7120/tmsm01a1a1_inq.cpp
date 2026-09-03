/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-01
Description: 钢包信息详细信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 钢包信息详细信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明

BM2F_ENTERACE(tmsm01a1a1_inq)   

int f_tmsm01a1a1_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CString	datetemp("");
	CDecimal stop_use_time = 0;

	/* 实体类定义 */ 
	CModel ttmsm01("TTMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		
		/* 获取输入参数 */
		ttmsm01["LADLE_NO"] = bcls_rec->Tables[0].Rows[0]["LADLE_NO"].ToString().Trim();
	
		Log::Trace("",__FUNCTION__,"ttmsm01.LADLE_NO		= [{0}]",(const char*)ttmsm01["LADLE_NO"].ToString());
		
		/* 检查输入参数合法性 */
		if(ttmsm01["LADLE_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,"钢包号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}	  

		/* 查询钢包信息表 */
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
				ttmsm01["LADLE_LEVEL"] = "BXXX";
				ttmsm01["LD_STATUS"]   = "2";
				/*sprintf(s.msg,"钢包等级[%s]错误!",(const char*)ttmsm01["LADLE_LEVEL"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);*/
			}
		}
		//--------------钢包等级判定结束--------------//

		/* 设置返回块的值 */
		ttmsm01.MergeTo(bcls_ret->Tables[0],false);

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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}



