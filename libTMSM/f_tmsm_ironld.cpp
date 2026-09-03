/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-01-26
Description: 铁水包实绩获取
**************************************************/

/*<remark>=========================================================
/// <summary>
/// 铁水包实绩获取
/// <para>
/// 0.根据传入的
来水：调用事件号、铁水包号、铁水罐重量
倒罐：调用事件号、铁水包号、倒罐处理号、是否标准罐
脱硫：调用事件号、铁水包号、倒罐处理号、铁水预处理号、是否脱硫
/// </para>
/// <para>数据库表：ttmsm11(铁水包基本信息表)		</para>
/// <para>数据库表：ttmsm19(铁水包使用信息表)		</para>
/// <para>主调用函数：电文 倒罐实绩、脱硫实绩 计划收到铁区实绩时 调用。  </para>
/// </summary>
/// <param name="">调用事件号		</param>
/// <param name="heat_no">熔炼号		</param>
/// <param name="pono">制造命令号		</param>
/// <param name="equ_no">设备号				</param>
/// <param name="run_signal">运转信号		</param>
/// <param name="start_time_event"事件发生时刻    </param>
/// <param name="ladle_no"铁水包号    </param>
/// <param name="dev_no"设备代码    </param>
/// <returns>配包信息</returns>
===========================================================</remark>*/

#include "stdafx.h"
#include "ttmsm11.h"
#include "ttmsm09.h"

//名称空间引用
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

int f_tmsm_ironld(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);

	// 程序内部变量
	int doFlag = 0;
	int count = 0;

	// 实体类定义
	CTTMSM11 ttmsm11(conn);
	CTTMSM11 tTMSM11(conn);
	CTTMSM09 ttmsm09(conn);

	// 数据库SQL操作字符串
	CString sqlstr = "";

	// 数据库操作类定义
	CDbCommand cmd_inq(conn);

	// 业务变量
	CString	event_id = ""; //跟踪事件号
	CString sm_unit_no = "";

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

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
		
		ttmsm11.Reset();
		ttmsm11.SM_UNIT_NO = sm_unit_no;
		
		if(bcls_rec->Tables.Contains("TMSM11") == false)
		{
			 strcpy(s.msg,_RES("GCRSS0000011")/*系统出现异常，数据块有误，请联系系统维护人员。*/);
			 strcpy(s.sysmsg, "块TMSM11不存在。");
			 throw CApplicationException(-1, s.msg, log.Location); 
		}

		event_id = bcls_rec->Tables["TMSM11"].Rows[0]["EVENT_ID"].ToString().Trim();
		Log::Trace("","","event_id = {0}", event_id);
			
		if (event_id.Trim() == "")
		{
			sprintf(s.msg,"event_id can not be empty.");
			sprintf(s.sysmsg,"event_id can not be empty.");
			throw CApplicationException(-1, s.msg, log.Location); 
		}

		// 铁水包来水 计划接收铁区的实绩 然后调用
		if(event_id.Trim() == "TMLS")
		{
		 	//来水：调用事件号、铁水包号、铁水罐重量
		 	ttmsm11.IRON_LADLE_NO	= (CString)bcls_rec->Tables["TMSM11"].Rows[0]["IRON_LADLE_NO"].ToString().Trim();
		 	ttmsm11.MOLTIRON_WT = (CDecimal)bcls_rec->Tables["TMSM11"].Rows[0]["MOLTIRON_WT"];
		 	Log::Trace(" ", "f_tmsm_ironld", "ttmsm11.MOLTIRON_WT[{0}]"		,ttmsm11.MOLTIRON_WT);
		 		
		 	if (ttmsm11.IRON_LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的铁水包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "f_tmsm_ironld");
			}
			
			sqlstr = " SELECT COUNT(1) FROM TTMSM11 "
			" WHERE IRON_LADLE_NO = @ttmsm11.IRON_LADLE_NO "
			" AND SM_UNIT_NO = @ttmsm11.SM_UNIT_NO "
			;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
			cmd_inq.Parameters.Set( "ttmsm11.SM_UNIT_NO"		, ttmsm11.SM_UNIT_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if(cmd_inq.Read())
			{
				count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
			}
			cmd_inq.Close();
			Log::Trace("","f_tmsm_ironld"," ttmsm11 count [{0}]",count );
			
			if	(count == 0)
			{
				sprintf	(s.msg , "传入的铁水包号不存在") ;
				throw	CApplicationException(-1 , s.msg , "f_tmsm_ironld");
			}
			else // 修改ttmsm11表中该铁水包的铁水罐重量
			{
				// ttmsm11.REC_REVISOR = s.userid;   //记录修改责任者
				// ttmsm11.REC_REVISE_TIME =datetimeNow; //记录修改时刻
				ttmsm11.Update(" REC_REVISOR "
					" ,REC_REVISE_TIME "
					" , MOLTIRON_WT ",
					" SM_UNIT_NO, LADLE_NO ");
			}
		}
		
		// 倒罐：调用事件号、铁水包号、倒罐处理号、是否标准罐
		if(event_id.Trim() == "TMDG")
		{
		 	// 倒罐：调用事件号、铁水包号、倒罐处理号、是否标准罐
		 	ttmsm11.IRON_LADLE_NO	= (CString)bcls_rec->Tables["TMSM11"].Rows[0]["IRON_LADLE_NO"].ToString().Trim();
		 	ttmsm11.TPD_NO	= (CString)bcls_rec->Tables["TMSM11"].Rows[0]["TPD_NO"].ToString().Trim();
		 	ttmsm11.IS_STANDARD_JAR	= (CString)bcls_rec->Tables["TMSM11"].Rows[0]["IS_STANDARD_JAR"].ToString().Trim();
		 	Log::Trace(" ", "f_tmsm_ironld", "ttmsm11.IS_STANDARD_JAR[{0}]"		,(const char*)ttmsm11.IS_STANDARD_JAR);
		 		
		 	if (ttmsm11.IRON_LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的铁水包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "f_tmsm_ironld");
			}
			
			sqlstr = " SELECT COUNT(1) FROM TTMSM11 "
			" WHERE IRON_LADLE_NO = @ttmsm11.IRON_LADLE_NO "
			" AND SM_UNIT_NO = @ttmsm11.SM_UNIT_NO "
			;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
			cmd_inq.Parameters.Set( "ttmsm11.SM_UNIT_NO"		, ttmsm11.SM_UNIT_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if(cmd_inq.Read())
			{
				count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
			}
			cmd_inq.Close();
			Log::Trace("","f_tmsm_ironld"," ttmsm11 count [{0}]",count );
			
			if	(count == 0)
			{
				sprintf	(s.msg , "传入的铁水包号不存在") ;
				throw	CApplicationException(-1 , s.msg , "f_tmsm_ironld");
			}
			else // 修改ttmsm11表
			{
				// 计算包龄先
				
					// ttmsm11.REC_REVISOR = s.userid;   //记录修改责任者
					// ttmsm11.REC_REVISE_TIME =datetimeNow; //记录修改时刻
					ttmsm11.Update(" REC_REVISOR "
						" ,REC_REVISE_TIME "
						" , TPD_NO "
						" , IS_STANDARD_JAR ",							
						" SM_UNIT_NO, LADLE_NO ");
			}
		}
		
		// 脱硫：调用事件号、铁水包号、倒罐处理号、铁水预处理号、是否脱硫
		if(event_id.Trim() == "TMTL")
		{
		 	// 脱硫：调用事件号、铁水包号、倒罐处理号、铁水预处理号、是否脱硫
		 	ttmsm11.IRON_LADLE_NO	= (CString)bcls_rec->Tables["TMSM11"].Rows[0]["IRON_LADLE_NO"].ToString().Trim();
		 	ttmsm11.TPD_NO	= (CString)bcls_rec->Tables["TMSM11"].Rows[0]["TPD_NO"].ToString().Trim();
		 	ttmsm11.IRON_LTREAT_NO	= (CString)bcls_rec->Tables["TMSM11"].Rows[0]["IRON_LTREAT_NO"].ToString().Trim();
		 	ttmsm11.IS_IRON_LTREAT	= (CString)bcls_rec->Tables["TMSM11"].Rows[0]["IS_IRON_LTREAT"].ToString().Trim();
		 	Log::Trace(" ", "f_tmsm_ironld", "ttmsm11.TPD_NO[{0}]"		,(const char*)ttmsm11.TPD_NO);
		 	Log::Trace(" ", "f_tmsm_ironld", "ttmsm11.IS_IRON_LTREAT[{0}]"		,(const char*)ttmsm11.IS_IRON_LTREAT);
		 		
		 	if (ttmsm11.IRON_LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的铁水包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "f_tmsm_ironld");
			}
			
			sqlstr = " SELECT COUNT(1) FROM TTMSM11 "
			" WHERE IRON_LADLE_NO = @ttmsm11.IRON_LADLE_NO "
			" AND SM_UNIT_NO = @ttmsm11.SM_UNIT_NO "
			;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
			cmd_inq.Parameters.Set( "ttmsm11.SM_UNIT_NO"		, ttmsm11.SM_UNIT_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if(cmd_inq.Read())
			{
				count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
			}
			cmd_inq.Close();
			Log::Trace("","f_tmsm_ironld"," ttmsm11 count [{0}]",count );
			
			if	(count == 0)
			{
				sprintf	(s.msg , "传入的铁水包号不存在") ;
				throw	CApplicationException(-1 , s.msg , "f_tmsm_ironld");
			}
			else // 修改ttmsm11表
			{
				// 计算包龄先
				
					// ttmsm11.REC_REVISOR = s.userid;   //记录修改责任者
					// ttmsm11.REC_REVISE_TIME =datetimeNow; //记录修改时刻
					ttmsm11.Update(" REC_REVISOR "
						" ,REC_REVISE_TIME "
						" , TPD_NO "
						" , IRON_LTREAT_NO "
						" , IS_IRON_LTREAT ",							
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