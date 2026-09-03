/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-01-24
Description: 钢包实绩获取
**************************************************/

/*<remark>=========================================================
/// <summary>
/// 钢包实绩获取
/// <para>
/// 0.根据传入的
f_tmsm_ld：调用事件号（转炉实绩 LF实绩 炉次连铸浇铸实绩
转炉实绩：钢包号、钢包状态（热包、温包、凉包）、钢水净重
LF实绩：吹氩时间累计、钢包到达时刻、
炉次连铸浇铸实绩：这个到时候问问卡卡，接口没理解
/// 如果是LF实绩，判断更新TTMSM01
/// 如果是转炉实绩，判断是否要新增或更新TTMSM09
/// </para>
/// <para>数据库表：ttmsm01(钢包基本信息表)		</para>
/// <para>数据库表：ttmsm09(钢包使用信息表)		</para>
/// <para>主调用函数：电文 转炉实绩、LF实绩 炉次连铸浇铸实绩 调用。  </para>
/// </summary>
/// <param name="">调用事件号		</param>
/// <param name="heat_no">熔炼号		</param>
/// <param name="pono">制造命令号		</param>
/// <param name="equ_no">设备号				</param>
/// <param name="run_signal">运转信号		</param>
/// <param name="start_time_event"事件发生时刻    </param>
/// <param name="ladle_no"钢包号    </param>
/// <param name="dev_no"设备代码    </param>
/// <returns>配包信息</returns>
===========================================================</remark>*/

#include "stdafx.h"
#include "ttmsm01.h"
#include "ttmsm09.h"

//名称空间引用
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

int f_tmsm_ld(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);

	// 程序内部变量
	int doFlag = 0;
	int count = 0;

	// 实体类定义
	CTTMSM01 ttmsm01(conn);
	CTTMSM09 ttmsm09(conn);

	// 数据库SQL操作字符串
	CString sqlstr = "";
	CString sqlstr1 = "";

	// 数据库操作类定义
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	// 业务变量
	CString	event_id = ""; //跟踪事件号
	CString sm_unit_no = "";
	CDecimal steel_net_weight = 0; // 转炉用
	CString start_time = "";// LF用
	CString end_time = "";	// LF用
//	CString heat_no = "";
//	CString pono = "";
//	//CString equ_no = "";
//	CString run_signal = "";
//	CString start_time_event = "";
//	CString ladle_no = "";
//	CString ladle_status = "";
//	CDecimal ladle_w_l = 0;
//	CString sm_plan_no = "";
//	CString dev_code = "";

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	//CString delTime = CDateTime::Now().AddDays(-90).ToString("yyyyMMddHHmmss");

	try
	{
		#if defined(_P1)
			sm_unit_no = "L1";
		#endif

		#if defined(_P2)
			sm_unit_no = "L2";
		#endif

		#if defined(_P3)
			sm_unit_no = "L1";
		#endif

		if(sm_unit_no.Trim() == "")
			sm_unit_no = "L1";
		Log::Trace("","","sm_unit_no = {0}", sm_unit_no);
		
		ttmsm01.Reset();
		ttmsm01.SM_UNIT_NO = sm_unit_no;
		
		// f_tmsm_plan用了TMSM02块 所以LADLE的函数都用TMSM02块好了
		if(bcls_rec->Tables.Contains("TMSM02") == false)
		{
			 strcpy(s.msg,_RES("GCRSS0000011")/*系统出现异常，数据块有误，请联系系统维护人员。*/);
			 strcpy(s.sysmsg, "块TMSM02不存在。");
			 throw CApplicationException(-1, s.msg, log.Location); 
		}

		event_id = bcls_rec->Tables["TMSM02"].Rows[0]["EVENT_ID"].ToString().Trim();
		Log::Trace("","","event_id = {0}", event_id);
			
		if (event_id.Trim() == "")
		{
			sprintf(s.msg,"event_id can not be empty.");
			sprintf(s.sysmsg,"event_id can not be empty.");
			throw CApplicationException(-1, s.msg, log.Location); 
		}

		// 转炉实绩
		if(event_id.Trim() == "TMZL")
		{
		 	// 转炉实绩：调用事件号、钢包号、钢包状态（热包、温包、凉包）、钢水净重、吹氩时间累计、
		 	ttmsm01.LADLE_NO	= (CString)bcls_rec->Tables["TMSM02"].Rows[0]["LADLE_NO"].ToString().Trim();
		 	//电文里的钢包状态1:热包 2:温包 3:黑包 4:凉包
		 	//钢包状态00-新包 01-可用 02-凉包 03-黑包 11-烘烤 21-使用 31-维修 32-粘钢 99-报废
		 	ttmsm01.LADLE_STATUS	= (CString)bcls_rec->Tables["TMSM02"].Rows[0]["LADLE_STATUS"].ToString().Trim();
		 	steel_net_weight		= (CDecimal)bcls_rec->Tables["TMSM02"].Rows[0]["STEEL_NET_WEIGHT"];
			ttmsm01.LADLE_W_L		= steel_net_weight.Round(3);
			//ttmsm01.LOAD_STEEL_WT	= (CDecimal)bcls_rec->Tables["TMSM02"].Rows[0]["STEEL_NET_WEIGHT"];
			ttmsm01.AR_BLOW_TIME_TOTAL = (CDecimal)bcls_rec->Tables["TMSM02"].Rows[0]["AR_DURATION"];
			
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LADLE_NO[{0}]"		,(const char*)ttmsm01.LADLE_NO);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LADLE_STATUS[{0}]"	,(const char*)ttmsm01.LADLE_STATUS);
			Log::Trace(" ", "f_tmsm_ld", "steel_net_weight[{0}]"		,steel_net_weight);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LADLE_W_L[{0}]"		,ttmsm01.LADLE_W_L);
			//Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LOAD_STEEL_WT[{0}]"	,ttmsm01.LOAD_STEEL_WT);
			Log::Trace(" ", "f_tmsm_ld", "ttmsm01.AR_BLOW_TIME_TOTAL[{0}]",ttmsm01.AR_BLOW_TIME_TOTAL);
		 		
		 	if (ttmsm01.LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的钢包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "f_tmsm_ld");
			}

			// 转换钢包状态
			if (ttmsm01.LADLE_STATUS.Trim() == "3")
			{
				ttmsm01.LADLE_STATUS = "03";
			}
			else if (ttmsm01.LADLE_STATUS.Trim() == "4")
			{
				ttmsm01.LADLE_STATUS = "02";
			}
			else
			{
				ttmsm01.LADLE_STATUS = "21";
			}
			Log::Trace(" ", "f_tmsm_ld", "转换后的LADLE_STATUS[{0}]",(const char*)ttmsm01.LADLE_STATUS);

			// 转换装钢量
			ttmsm01.LOAD_STEEL_WT = ttmsm01.LADLE_W_L *1000;
			Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LOAD_STEEL_WT[{0}]"	,ttmsm01.LOAD_STEEL_WT);

		 	sqlstr = " SELECT COUNT(1) FROM TTMSM01 "
			" WHERE LADLE_NO = @ttmsm01.LADLE_NO "
			" AND SM_UNIT_NO = @ttmsm01.SM_UNIT_NO "
			;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm01.LADLE_NO"	, ttmsm01.LADLE_NO );
			cmd_inq.Parameters.Set( "ttmsm01.SM_UNIT_NO", ttmsm01.SM_UNIT_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if(cmd_inq.Read())
			{
				count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
			}
			cmd_inq.Close();
			Log::Trace("","f_tmsm_ld"," ttmsm01 count [{0}]",count );
			
			if	(count == 0)
			{
				sprintf	(s.msg , "传入的钢包号不存在") ;
				throw	CApplicationException(-1 , s.msg , "f_tmsm_ld");
			}
			else // 修改TTMSM01表中该钢包的包状态与钢水净重信息
			{
				// 计算累计装钢量
				sqlstr1 = " SELECT LOAD_STEEL_WT_TOTAL FROM TTMSM01 "
				" WHERE LADLE_NO = @ttmsm01.LADLE_NO "
				" AND SM_UNIT_NO = @ttmsm01.SM_UNIT_NO "
				;
				cmd_inq1.SetCommandText( sqlstr );
				cmd_inq1.Parameters.Set( "ttmsm01.LADLE_NO"	, ttmsm01.LADLE_NO );
				cmd_inq1.Parameters.Set( "ttmsm01.SM_UNIT_NO", ttmsm01.SM_UNIT_NO );
				cmd_inq1.ExecuteReader();
				if(cmd_inq1.Read())
				{
					ttmsm01.LOAD_STEEL_WT_TOTAL = cmd_inq1.GetDecimal(1); //将数据获取到实体对象中				
				}
				cmd_inq1.Close();
				Log::Trace("","f_tmsm_ld"," ttmsm01.LOAD_STEEL_WT_TOTAL[{0}]",ttmsm01.LOAD_STEEL_WT_TOTAL );
				ttmsm01.LOAD_STEEL_WT_TOTAL = ttmsm01.LOAD_STEEL_WT_TOTAL + ttmsm01.LOAD_STEEL_WT;
				Log::Trace("","f_tmsm_ld"," new ttmsm01.LOAD_STEEL_WT_TOTAL[{0}]",ttmsm01.LOAD_STEEL_WT_TOTAL );
				
				ttmsm01.REC_REVISOR = s.userid;   //记录修改责任者
				ttmsm01.REC_REVISE_TIME =datetimeNow; //记录修改时刻
				ttmsm01.Update(" REC_REVISOR "
					" ,REC_REVISE_TIME "
					" , LADLE_STATUS "
					" , LADLE_W_L "
					" , AR_BLOW_TIME_TOTAL "
					" , LOAD_STEEL_WT "
					" , LOAD_STEEL_WT_TOTAL ",							
					" SM_UNIT_NO, LADLE_NO ");
			}
		}
		
		// LF实绩
		if(event_id.Trim() == "TMLF")
		{
		 	// LF实绩：调用事件号、钢包号、钢包状况、钢包使用次数、吹氩时间累计、钢包到达时刻
		 	ttmsm01.LADLE_NO	= (CString)bcls_rec->Tables["TMSM02"].Rows[0]["LADLE_NO"].ToString().Trim();
		 	ttmsm01.LD_STATUS = (CString)bcls_rec->Tables["TMSM02"].Rows[0]["LADLE_STATUS"].ToString().Trim();
		 	ttmsm01.LADLE_LIFE = (CDecimal)bcls_rec->Tables["TMSM02"].Rows[0]["LADLE_USE_SUM"];
		 	ttmsm01.AR_BLOW_TIME_TOTAL = (CDecimal)bcls_rec->Tables["TMSM02"].Rows[0]["AR_DURATION"];
		 	ttmsm01.LADLE_ARRIVE_TIME = (CString)bcls_rec->Tables["TMSM02"].Rows[0]["LADLE_ARRIVE_TIME"].ToString().Trim();
			start_time = (CString)bcls_rec->Tables["TMSM02"].Rows[0]["START_TIME"].ToString().Trim();
		 	end_time = (CString)bcls_rec->Tables["TMSM02"].Rows[0]["END_TIME"].ToString().Trim();
			ttmsm01.GRP_NO = (CString)bcls_rec->Tables["TMSM02"].Rows[0]["PROD_SHIFT_GROUP"].ToString().Trim();
		 	ttmsm01.SHIFT_NO = (CString)bcls_rec->Tables["TMSM02"].Rows[0]["PROD_SHIFT_NO"].ToString().Trim();
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LADLE_NO[{0}]"		,(const char*)ttmsm01.LADLE_NO);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LADLE_STATUS[{0}]"	,(const char*)ttmsm01.LADLE_STATUS);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LADLE_LIFE[{0}]"		,ttmsm01.LADLE_LIFE);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.AR_BLOW_TIME_TOTAL[{0}]",ttmsm01.AR_BLOW_TIME_TOTAL);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LADLE_ARRIVE_TIME[{0}]",(const char*)ttmsm01.LADLE_ARRIVE_TIME);			
		 	Log::Trace(" ", "f_tmsm_ld", "start_time[{0}]"			,(const char*)start_time);
			Log::Trace(" ", "f_tmsm_ld", "end_time[{0}]"			,(const char*)end_time);
			Log::Trace(" ", "f_tmsm_ld", "ttmsm01.GRP_NO[{0}]"			,(const char*)ttmsm01.GRP_NO);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.SHIFT_NO[{0}]"		,(const char*)ttmsm01.SHIFT_NO);
		 	
		 	if (ttmsm01.LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的钢包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "f_tmsm_ld");
			}	

			// 计算时间	精炼时间累计		//刘丹英2014.4.29注释
			/*CDateTime CTIME_START = CDateTime::Parse(start_time); 
			CDateTime CTIME_END = CDateTime::Parse(end_time);
			CTimeSpan ts = CTIME_END - CTIME_START;
			ttmsm01.SRP_TIME_TOTAL = ts.TotalMinutes;*/


			//int days = ts.Days();
			//int Minutes = ts.Minutes();//只对分钟计算
			//int Minutes_total = ts.TotalMinutes();//总的换算成分钟
			Log::Trace(" ", "f_tmsm_ld", "ttmsm01.SRP_TIME_TOTAL[{0}]"	,ttmsm01.SRP_TIME_TOTAL);
		 	
		 	sqlstr = " SELECT COUNT(1) FROM TTMSM01 "
			" WHERE LADLE_NO = @ttmsm01.LADLE_NO "
			" AND SM_UNIT_NO = @ttmsm01.SM_UNIT_NO "
			;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm01.LADLE_NO"	, ttmsm01.LADLE_NO );
			cmd_inq.Parameters.Set( "ttmsm01.SM_UNIT_NO", ttmsm01.SM_UNIT_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if(cmd_inq.Read())
			{
				count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
			}
			cmd_inq.Close();
			Log::Trace("","f_tmsm_ld"," ttmsm01 count [{0}]",count );
			
			if	(count == 0)
			{
				sprintf	(s.msg , "传入的钢包号不存在") ;
				throw	CApplicationException(-1 , s.msg , "f_tmsm_ld");
			}
			else // 修改TTMSM01表中该钢包的包状态与钢水净重信息
			{
				// ttmsm01.REC_REVISOR = s.userid;   //记录修改责任者
				// ttmsm01.REC_REVISE_TIME =datetimeNow; //记录修改时刻
				ttmsm01.Update(" REC_REVISOR "
					" ,REC_REVISE_TIME "
					" , LD_STATUS "
					" , LADLE_LIFE "
					" , AR_BLOW_TIME_TOTAL "
					" , LADLE_ARRIVE_TIME "
					" , SRP_TIME_TOTAL "
					" , GRP_NO "
					" , SHIFT_NO ",
					" SM_UNIT_NO, LADLE_NO ");
			}
		}

		// 炉次连铸浇铸实绩
		if(event_id.Trim() == "TMLZ")
		{
		 	// 炉次连铸浇铸实绩：调用事件号、钢包号、钢包状况、钢包使用次数、吹氩时间累计、钢包到达时刻、
		 	ttmsm01.LADLE_NO	= (CString)bcls_rec->Tables["TMSM02"].Rows[0]["LADLE_NO"].ToString().Trim();
		 	ttmsm01.LD_STATUS = (CString)bcls_rec->Tables["TMSM02"].Rows[0]["LADLE_STATUS"].ToString().Trim();
		 	ttmsm01.LADLE_LIFE = (CDecimal)bcls_rec->Tables["TMSM02"].Rows[0]["LADLE_USE_SUM"];
		 	ttmsm01.AR_BLOW_TIME_TOTAL = (CDecimal)bcls_rec->Tables["TMSM02"].Rows[0]["AR_DURATION"];
		 	ttmsm01.LADLE_ARRIVE_TIME = (CString)bcls_rec->Tables["TMSM02"].Rows[0]["LADLE_ARRIVE_TIME"].ToString().Trim();
		 	ttmsm01.GRP_NO = (CString)bcls_rec->Tables["TMSM02"].Rows[0]["PROD_SHIFT_GROUP"].ToString().Trim();
		 	ttmsm01.SHIFT_NO = (CString)bcls_rec->Tables["TMSM02"].Rows[0]["PROD_SHIFT_NO"].ToString().Trim();
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LADLE_NO[{0}]"		,(const char*)ttmsm01.LADLE_NO);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LADLE_STATUS[{0}]"	,(const char*)ttmsm01.LADLE_STATUS);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LADLE_LIFE[{0}]"		,ttmsm01.LADLE_LIFE);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.AR_BLOW_TIME_TOTAL[{0}]",ttmsm01.AR_BLOW_TIME_TOTAL);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.LADLE_ARRIVE_TIME[{0}]",(const char*)ttmsm01.LADLE_ARRIVE_TIME);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.GRP_NO[{0}]"			,(const char*)ttmsm01.GRP_NO);
		 	Log::Trace(" ", "f_tmsm_ld", "ttmsm01.SHIFT_NO[{0}]"		,(const char*)ttmsm01.SHIFT_NO);
		 		
		 	if (ttmsm01.LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的钢包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "f_tmsm_ld");
			}				
				
		 	sqlstr = " SELECT COUNT(1) FROM TTMSM01 "
			" WHERE LADLE_NO = @ttmsm01.LADLE_NO "
			" AND SM_UNIT_NO = @ttmsm01.SM_UNIT_NO "
			;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm01.LADLE_NO"	, ttmsm01.LADLE_NO );
			cmd_inq.Parameters.Set( "ttmsm01.SM_UNIT_NO", ttmsm01.SM_UNIT_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if(cmd_inq.Read())
			{
				count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
			}
			cmd_inq.Close();
			Log::Trace("","f_tmsm_ld"," ttmsm01 count [{0}]",count );
			
			if	(count == 0)
			{
				sprintf	(s.msg , "传入的钢包号不存在") ;
				throw	CApplicationException(-1 , s.msg , "f_tmsm_ld");
			}
			else // 修改TTMSM01表中该钢包的包状态与钢水净重信息
			{
				// ttmsm01.REC_REVISOR = s.userid;   //记录修改责任者
				// ttmsm01.REC_REVISE_TIME =datetimeNow; //记录修改时刻
				ttmsm01.Update(" REC_REVISOR "
					" ,REC_REVISE_TIME "
					" , LD_STATUS "
					" , LADLE_LIFE "
					" , AR_BLOW_TIME_TOTAL "
					" , LADLE_ARRIVE_TIME "
					" , GRP_NO "
					" , SHIFT_NO ",
					" SM_UNIT_NO, LADLE_NO ");
			}
		}
		strcpy(s.msg, _RES("GCRSS0000002")/*处理成功。*/);
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (const CApplicationException& ex)
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