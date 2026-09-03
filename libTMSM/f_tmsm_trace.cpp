/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-11
Description: 钢包跟踪履历
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 钢包跟踪履历
/// <para>
/// * 记录钢包履历
/// <para>
/// </summary>
/// <param name="LADLE_NO">钢包号</param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  
 


int f_tmsm_trace(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");    
	CString	datetemp("");  
	CString	cs_event_desc("");    
	CString	cs_tm_seq_id("");

	CDecimal stop_use_time = 0;

	/* 实体类定义 */
	CModel ttmsm01("TTMSM01");
	CModel ttmsm96("TTMSM96");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("TM0099");
		if(blkNum < 0)
		{
			strcpy(s.msg,_RES("GCRSS0000007"));/*系统出现异常，请联系系统维护人员。*/
			strcpy(s.sysmsg,"传入数据块TM0099不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		for(int i = 0; i < bcls_rec->Tables["TM0099"].Rows.get_Count(); i++ ) 
		{
			/* 获取输入参数 */
			ttmsm96["LADLE_NO"]	= bcls_rec->Tables["TM0099"].Rows[i]["LADLE_NO"].ToString().Trim();
			
			if(bcls_rec->Tables["TM0099"].Columns.Contains("EVENT_ID") == true)
			{
				ttmsm96["EVENT_ID"]	= bcls_rec->Tables["TM0099"].Rows[i]["EVENT_ID"].ToString().Trim();
			}
			if(bcls_rec->Tables["TM0099"].Columns.Contains("EVENT_DESC") == true)
			{
				ttmsm96["EVENT_DESC"]	= bcls_rec->Tables["TM0099"].Rows[i]["EVENT_DESC"].ToString().Trim();
			}	
			if(bcls_rec->Tables["TM0099"].Columns.Contains("FUNC_ID") == true)
			{
				ttmsm96["FUNC_ID"]	= bcls_rec->Tables["TM0099"].Rows[i]["FUNC_ID"].ToString().Trim();
			}	
			if(bcls_rec->Tables["TM0099"].Columns.Contains("SYSTEM_ID") == true)
			{
				ttmsm96["SYSTEM_ID"]	= bcls_rec->Tables["TM0099"].Rows[i]["SYSTEM_ID"].ToString().Trim();
			}
			ttmsm96["FORM_NAME"]	= s.formname; 
			
			Log::Trace("",__FUNCTION__,"num = 1111111111111{0}",(const char*)ttmsm96["LADLE_NO"].ToString());
			
			/* 检查输入参数合法性 */
			if(ttmsm96["LADLE_NO"].ToString().Trim() ==	"")
			{
				strcpy(s.msg,"钢包号不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if(ttmsm96["EVENT_ID"].ToString().Trim() ==	"")
			{
				strcpy(s.msg,"事件号不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 查询材料主档信息 */
			ttmsm01["LADLE_NO"]	= ttmsm96["LADLE_NO"];
			//--------------钢包等级判定开始--------------//
			ttmsm01.Query("LADLE_NO");

			Log::Trace("",__FUNCTION__,"num = 1111111111111{0}",(const char*)ttmsm01["LADLE_NO"].ToString());

			if((ttmsm01["LADLE_LIFE"].ToDecimal() == 1)&&(ttmsm01["LADLE_LEVEL"].ToString().Trim() == ""))  //新包
			{
				ttmsm01["LADLE_LEVEL"] = "BXXX";
				ttmsm01["LD_STATUS"]   = "3";
			}
			else
			{	
				Log::Trace("",__FUNCTION__,"num = 1111111111111{0}",(const char*)ttmsm01["LADLE_NO"].ToString());
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
						if (ttmsm01["LADLE_LEVEL"].ToString().GetLength() >4)
						{
							ttmsm01["LADLE_LEVEL"] = "BXXX";
							ttmsm01["LD_STATUS"] = "2";
						}
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
			ttmsm01.TrimOrBlank();		 		

			/* 设置物料跟踪信息 */			
			ttmsm96.CopyFrom(ttmsm01);
			ttmsm96["REC_CREATOR"]		= s.userid;
			ttmsm96["REC_CREATE_TIME"]	= datetime;

			if(ttmsm96["FORM_NAME"].ToString().Trim()	==	"")
			{ 
				ttmsm96["FORM_NAME"]	    = s.formname;
			}
			if(ttmsm96["FUNC_ID"].ToString().Trim()	==	"")
			{ 
				ttmsm96["FUNC_ID"]	        = s.svc_name;
			}

			Log::Trace("",__FUNCTION__,"num = 1111111111111[{0}]",(const char*)ttmsm01["LADLE_NO"].ToString());

			//生成材料履历流水号
			cs_tm_seq_id = EPGetNextSeq("MM_SEQ_ID",conn); 
			if(cs_tm_seq_id.Trim() == "")
			{
				sprintf(s.msg,"获取 钢包履历流水号 失败!");/*系统出现异常，请联系系统维护人员。*/
				strcpy(s.sysmsg,"MM_SEQ_ID error!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm96["RESUME_SEQ_NO"]	=	CDateTime::Now().ToString("yyyyMMddHHmmssmsff"); 
			Log::Trace("",__FUNCTION__,"ttmsm96.RESUME_SEQ_NO.ToString().yyyyMMddHHmmssmsff = {0}",(const char*)ttmsm96["RESUME_SEQ_NO"].ToString());
			ttmsm96["RESUME_SEQ_NO"]	=	CDateTime::Now().ToString("yyyyMMddHHmmssmsff").Substring(0,18); 
			Log::Trace("",__FUNCTION__,"生成材料履历流水号 ttmsm96.RESUME_SEQ_NO.ToString().yyyyMMddHHmmssmsff.Substring(0,18) = {0}",(const char*)ttmsm96["RESUME_SEQ_NO"].ToString());
			ttmsm96["RESUME_SEQ_NO"]	=	CDateTime::Now().ToString("yyyyMMddHHmmssmsff").Substring(0,18) + cs_tm_seq_id; 
			Log::Trace("",__FUNCTION__,"生成材料履历流水号 ttmsm96.RESUME_SEQ_NO.ToString() + cs_tm_seq_id= {0}",(const char*)ttmsm96["RESUME_SEQ_NO"].ToString());

			ttmsm96.TrimOrBlank();
			ttmsm96.Insert();
 
		} 
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,"数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace((1,1, "[%s]", s.sysmsg);
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
