/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-01-24
Description: 工器具实绩获取
**************************************************/

/*<remark>=========================================================
/// <summary>
/// 工器具实绩获取
/// <para>
/// 0.根据传入的
调用事件号（转炉实绩 LF实绩 炉次连铸浇铸实绩 来水 倒罐 脱硫

钢包：调用事件号（转炉实绩 LF实绩 炉次连铸浇铸实绩
转炉实绩：钢包号、钢包状态（热包、温包、凉包）、钢水净重
LF实绩：吹氩时间累计、钢包到达时刻、
炉次连铸浇铸实绩：

铁水包：调用事件号（来水 倒罐 脱硫
来水：调用事件号、铁水包号、铁水罐重量
倒罐：调用事件号、铁水包号、倒罐处理号、是否标准罐
脱硫：调用事件号、铁水包号、倒罐处理号、铁水预处理号、是否脱硫

/// </para>
/// 如果是LF实绩，判断更新TTMSM01
/// 如果是转炉实绩，判断是否要新增或更新TTMSM09
/// </para>
/// <para>数据库表：ttmsm01(钢包基本信息表)		</para>
/// <para>数据库表：ttmsm09(钢包使用信息表)		</para>
/// <para>数据库表：ttmsm11(铁水包基本信息表)		</para>
/// <para>数据库表：ttmsm19(铁水包使用信息表)		</para>
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
#include "ttmsm11.h"
#include "ttmsm19.h"
#include "ttmsm99.h"
#include "ttmsm21.h"
#include "ttmsm29.h"
#include "ttmsm31.h"
#include "ttmsm39.h"
#include "tmmsm31.h" // 炼钢连铸炉次实绩表

//名称空间引用
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

int f_tmsm_mm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);

	// 程序内部变量
	int doFlag = 0;
	int count = 0;
	int i = 0;

	// 实体类定义
	CTTMSM01 ttmsm01(conn);
	CTTMSM09 ttmsm09(conn);
	CTTMSM11 ttmsm11(conn);
	CTTMSM19 ttmsm19(conn);
	CTTMSM21 ttmsm21(conn);
	CTTMSM29 ttmsm29(conn);
	CTTMSM31 ttmsm31(conn);
	CTTMSM39 ttmsm39(conn);
	CTTMSM99 ttmsm99(conn);
	CTMMSM31 tmmsm31(conn); // 炼钢连铸炉次实绩表

	// 数据库SQL操作字符串
	CString sqlstr = "";
	CString sqlstr1 = "";

	// 数据库操作类定义
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	// 业务变量
	CString	event_id = "";	// 跟踪事件号
	CString dev_code = "";	// 处理设备号
	CString sm_unit_no = "";
	CDecimal steel_net_weight = 0; // 转炉用
	CString start_time = "";// LF用
	CString end_time = "";	// LF用

	EIClass tmbw_bcls_rec;

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
		Log::Trace("","","sm_unit_no = [{0}]", sm_unit_no);
		
		// f_tmsm_plan用了TMSM99块 所以LADLE的函数都用TMSM99块好了
		if(bcls_rec->Tables.Contains("TMSM99") == false)
		{
			 strcpy(s.msg,_RES("GCRSS0000011")/*系统出现异常，数据块有误，请联系系统维护人员。*/);
			 strcpy(s.sysmsg, "块TMSM99不存在。");
			 throw CApplicationException(-1, s.msg, log.Location); 
		}

		//event_id = bcls_rec->Tables["TMSM99"].Rows[0]["EVENT_ID"].ToString().Trim();		
		//Log::Trace("","","调用事件号event_id = [{0}]", event_id);
		dev_code = bcls_rec->Tables["TMSM99"].Rows[0]["DEV_CODE"].ToString().Trim();
		Log::Trace("","","调用事件号 dev_code = [{0}]", dev_code);
		
		// 设备代码
		// BOF炉 A~F
		// P1:A-1#BOF B-2#BOF C-3#BOF;
		// P2:E-5#BOF F-6#BOF;
		// P3:D-4#BOF
		// LF K~N
		// P1:K-1#LF;
		// P2:M-3#LF N-4#LF;
		// P3:L-2#LF
		// 连铸 1~8
		// P1:1-1#连铸 2-2#连铸 4-4#连铸;
		// P2:6-6#连铸 7-7#连铸 8-8#连铸;
		// P3:3-3#连铸 5-5#连铸

		if (dev_code.Trim() == "")
		{
			sprintf(s.msg,"dev_code can not be empty.");
			sprintf(s.sysmsg,"dev_code can not be empty.");
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		//if (event_id.Trim() == "")
		//{
		//	sprintf(s.msg,"event_id can not be empty.");
		//	sprintf(s.sysmsg,"event_id can not be empty.");
		//	throw CApplicationException(-1, s.msg, log.Location); 
		//}

		// 铁水包来水 炼钢计划接收铁区的实绩 然后调用
		if(dev_code.Trim() == "TMLS")//if(event_id.Trim() == "TMLS")
		{
			Log::Trace(" ", "f_tmsm_mm", "*******铁水包来水*******");
		 	
			ttmsm11.Reset();
			ttmsm11.SM_UNIT_NO = sm_unit_no;
		 	//来水：调用事件号、铁水包号、铁水罐重量
		 	ttmsm11.IRON_LADLE_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["IRON_LADLE_NO"].ToString().Trim();
		 	ttmsm11.MOLTIRON_WT	= (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["MOLTIRON_WT"];
			
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.IRON_LADLE_NO[{0}]"	,(const char*)ttmsm11.IRON_LADLE_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.SM_UNIT_NO[{0}]"		,(const char*)ttmsm11.SM_UNIT_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.MOLTIRON_WT[{0}]"		,ttmsm11.MOLTIRON_WT);
		 		
		 	if (ttmsm11.IRON_LADLE_NO.Trim() == "")
			{
				Log::Trace(" ", "f_tmsm_mm", "*******铁水包为空*******");
				//sprintf	(s.msg , "传入的铁水包号不能为空") ;
				//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
			}
			else
			{
				sqlstr = " SELECT COUNT(1) FROM TTMSM11 "
					" WHERE IRON_LADLE_NO	= @ttmsm11.IRON_LADLE_NO "
					" AND SM_UNIT_NO		= @ttmsm11.SM_UNIT_NO "
					;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
				cmd_inq.Parameters.Set( "ttmsm11.SM_UNIT_NO"	, ttmsm11.SM_UNIT_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if(cmd_inq.Read())
				{
					count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
				}
				cmd_inq.Close();
				Log::Trace("","f_tmsm_mm"," ttmsm11 count [{0}]",count );
				
				if	(count == 0)
				{
					Log::Trace(" ", "f_tmsm_mm", "*******铁水包为空*******");
					//sprintf	(s.msg , "传入的铁水包号不存在") ;
					//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
				}
				else // 修改ttmsm11表中该铁水包的铁水罐重量、包龄
				{
					// 计算包龄先 读取99表公式先
					sqlstr1 = " SELECT MODEL_ID,IS_STANDARD_JAR,TPD_NO,IS_IRON_LTREAT,IRON_LTREAT_NO,MOLTIRON_WT "
						" FROM TTMSM11 "
						" WHERE IRON_LADLE_NO	= @ttmsm11.IRON_LADLE_NO "
						" AND SM_UNIT_NO		= @ttmsm11.SM_UNIT_NO "
						;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
					cmd_inq1.Parameters.Set( "ttmsm11.SM_UNIT_NO"		, ttmsm11.SM_UNIT_NO );
					cmd_inq1.ExecuteReader();
					if(cmd_inq1.Read())
					{
						ttmsm11.MODEL_ID		= cmd_inq1.GetString(1);
						//ttmsm11.IS_STANDARD_JAR = cmd_inq1.GetString(2);
						//ttmsm11.TPD_NO			= cmd_inq1.GetString(3);
						//ttmsm11.IS_IRON_LTREAT	= cmd_inq1.GetString(4);
						//ttmsm11.IRON_LTREAT_NO	= cmd_inq1.GetString(5);
						//ttmsm11.MOLTIRON_WT	= cmd_inq1.GetDecimal(6);//传入值
					}
					cmd_inq1.Close();

					sqlstr1 = " SELECT * "
						" FROM   TTMSM99 "
						" WHERE  MODEL_ID = @ttmsm11.MODEL_ID ";
					;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set( "ttmsm11.MODEL_ID"	, ttmsm11.MODEL_ID );
					cmd_inq1.ExecuteReader();
					ttmsm99.Reset();
					if(cmd_inq1.Read())
					{
						cmd_inq1.Fetch(ttmsm99);
					}
					cmd_inq1.Close();

					// 此处是来水实绩 把上次倒罐和脱硫的值、包龄都清掉
					ttmsm11.IS_STANDARD_JAR = "0";	// 是否标准罐
					ttmsm11.TPD_NO			= " ";	// 倒罐处理号
					ttmsm11.IS_IRON_LTREAT	= "0";	// 是否脱硫
					ttmsm11.IRON_LTREAT_NO	= " ";	// 铁水预处理号
					ttmsm11.IRON_LADLE_LIFE = 0;

					// 倒罐
					if(ttmsm11.IS_STANDARD_JAR.Trim() == "0" || ttmsm11.IS_STANDARD_JAR.Trim() == "")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.TPD_STD_PARA;
					}
					else if(ttmsm11.IS_STANDARD_JAR.Trim() == "1")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.TPD_NSTD_PARA;
					}
					// 来水
					if(ttmsm11.MOLTIRON_WT <= ttmsm99.MOLTIRON_WT_A)
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.MOLTIRON_PARA_X;
					}
					else if(ttmsm99.MOLTIRON_WT_A < ttmsm11.MOLTIRON_WT < ttmsm99.MOLTIRON_WT_B)
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.MOLTIRON_PARA_Y;
					}
					else if(ttmsm11.MOLTIRON_WT >= ttmsm99.MOLTIRON_WT_B)
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.MOLTIRON_PARA_Z;
					}
					// 脱硫
					if(ttmsm11.IS_IRON_LTREAT.Trim() == "0" || ttmsm11.IS_IRON_LTREAT.Trim() == "")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.NS_PARA;
					}
					else if(ttmsm11.IS_IRON_LTREAT.Trim() == "1")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.S_PARA;
					}

					ttmsm11.REC_REVISOR = s.userid;   //记录修改责任者
					ttmsm11.REC_REVISE_TIME =datetimeNow; //记录修改时刻
					ttmsm11.Update(" REC_REVISOR "
						" , REC_REVISE_TIME "
						" , MOLTIRON_WT "
						" , IS_STANDARD_JAR "
						" , TPD_NO "
						" , IS_IRON_LTREAT "
						" , IRON_LTREAT_NO "
						" , IRON_LADLE_LIFE ",							
						" SM_UNIT_NO, IRON_LADLE_NO ");
				}
			}
		}
		
		// 倒罐：调用事件号、倒入的铁水包号、倒罐处理号、是否标准罐
		if(dev_code.Trim() == "TMDG")
		{
			Log::Trace(" ", "f_tmsm_mm", "*******倒罐*******");

			ttmsm11.Reset();
			ttmsm11.SM_UNIT_NO = sm_unit_no;
		 	// 倒罐：调用事件号、铁水包号、铁水重量、倒罐处理号、是否标准罐、
		 	ttmsm11.IRON_LADLE_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["POTID_INT"].ToString().Trim();
		 	ttmsm11.MOLTIRON_WT		= (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["MOLTIRON_WT"];
			ttmsm11.TPD_NO			= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["TPD_NO"].ToString().Trim();
		 	ttmsm11.IS_STANDARD_JAR	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["IS_STANDARD_JAR"].ToString().Trim();
		 	
			Log::Trace(" ", "f_tmsm_mm", "ttmsm11.SM_UNIT_NO[{0}]"		,(const char*)ttmsm11.SM_UNIT_NO);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm11.IRON_LADLE_NO[{0}]"	,(const char*)ttmsm11.IRON_LADLE_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.TPD_NO[{0}]"			,(const char*)ttmsm11.TPD_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.IS_STANDARD_JAR[{0}]"	,(const char*)ttmsm11.IS_STANDARD_JAR);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.MOLTIRON_WT[{0}]"		,ttmsm11.MOLTIRON_WT);

		 	if (ttmsm11.IRON_LADLE_NO.Trim() == "")
			{
				Log::Trace(" ", "f_tmsm_mm", "*******铁水包为空*******");
				//sprintf	(s.msg , "传入的铁水包号不能为空") ;
				//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
			}			
			else
			{
				sqlstr = " SELECT COUNT(1) FROM TTMSM11 "
					" WHERE IRON_LADLE_NO	= @ttmsm11.IRON_LADLE_NO "
					" AND SM_UNIT_NO		= @ttmsm11.SM_UNIT_NO "
					;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
				cmd_inq.Parameters.Set( "ttmsm11.SM_UNIT_NO"	, ttmsm11.SM_UNIT_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if(cmd_inq.Read())
				{
					count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
				}
				cmd_inq.Close();
				Log::Trace("","f_tmsm_mm"," ttmsm11 count [{0}]",count );
				
				if	(count == 0)
				{
					Log::Trace(" ", "f_tmsm_mm", "*******铁水包不存在*******");
					//sprintf	(s.msg , "传入的铁水包号不存在") ;
					//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
				}
				else // 修改ttmsm11表
				{
					// 计算包龄先
					sqlstr1 = " SELECT MODEL_ID,IS_STANDARD_JAR,TPD_NO,IS_IRON_LTREAT,IRON_LTREAT_NO,MOLTIRON_WT "
						" FROM TTMSM11 "
						" WHERE IRON_LADLE_NO	= @ttmsm11.IRON_LADLE_NO "
						" AND SM_UNIT_NO		= @ttmsm11.SM_UNIT_NO "
					;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
					cmd_inq1.Parameters.Set( "ttmsm11.SM_UNIT_NO"		, ttmsm11.SM_UNIT_NO );
					cmd_inq1.ExecuteReader();
					if(cmd_inq1.Read())
					{
						ttmsm11.MODEL_ID		= cmd_inq1.GetString(1);
						//ttmsm11.IS_STANDARD_JAR = cmd_inq1.GetString(2);//传入值
						//ttmsm11.TPD_NO			= cmd_inq1.GetString(3);//传入值
						//ttmsm11.IS_IRON_LTREAT	= cmd_inq1.GetString(4);
						//ttmsm11.IRON_LTREAT_NO	= cmd_inq1.GetString(5);
						//ttmsm11.MOLTIRON_WT	= cmd_inq1.GetDecimal(6);//传入值
					}
					cmd_inq1.Close();

					sqlstr1 = " SELECT * "
						" FROM   TTMSM99 "
						" WHERE  MODEL_ID = @ttmsm11.MODEL_ID ";
					;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set( "ttmsm11.MODEL_ID"	, ttmsm11.MODEL_ID );
					cmd_inq1.ExecuteReader();
					ttmsm99.Reset();
					if(cmd_inq1.Read())
					{
						cmd_inq1.Fetch(ttmsm99);
					}
					cmd_inq1.Close();

					// 此处是倒罐实绩 把上次脱硫的值、包龄都清掉
					//ttmsm11.IS_STANDARD_JAR = "0";	// 是否标准罐
					//ttmsm11.TPD_NO			= " ";	// 倒罐处理号
					ttmsm11.IS_IRON_LTREAT	= "0";	// 是否脱硫
					ttmsm11.IRON_LTREAT_NO	= " ";	// 铁水预处理号
					ttmsm11.IRON_LADLE_LIFE = 0;

					// 倒罐
					if(ttmsm11.IS_STANDARD_JAR.Trim() == "0" || ttmsm11.IS_STANDARD_JAR.Trim() == "")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.TPD_STD_PARA;
					}
					else if(ttmsm11.IS_STANDARD_JAR.Trim() == "1")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.TPD_NSTD_PARA;
					}
					// 来水
					if(ttmsm11.MOLTIRON_WT <= ttmsm99.MOLTIRON_WT_A)
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.MOLTIRON_PARA_X;
					}
					else if(ttmsm99.MOLTIRON_WT_A < ttmsm11.MOLTIRON_WT < ttmsm99.MOLTIRON_WT_B)
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.MOLTIRON_PARA_Y;
					}
					else if(ttmsm11.MOLTIRON_WT >= ttmsm99.MOLTIRON_WT_B)
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.MOLTIRON_PARA_Z;
					}
					// 脱硫
					if(ttmsm11.IS_IRON_LTREAT.Trim() == "0" || ttmsm11.IS_IRON_LTREAT.Trim() == "")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.NS_PARA;
					}
					else if(ttmsm11.IS_IRON_LTREAT.Trim() == "1")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.S_PARA;
					}

					ttmsm11.REC_REVISOR = s.userid;   //记录修改责任者
					ttmsm11.REC_REVISE_TIME =datetimeNow; //记录修改时刻
					ttmsm11.Update(" REC_REVISOR "
						" , REC_REVISE_TIME "
						" , MOLTIRON_WT "
						" , IS_STANDARD_JAR "
						" , TPD_NO "
						" , IS_IRON_LTREAT "
						" , IRON_LTREAT_NO "
						" , IRON_LADLE_LIFE ",
						" SM_UNIT_NO, IRON_LADLE_NO ");
				}
			}
		}
		
		// 脱硫：调用事件号、铁水包号、倒罐处理号、铁水预处理号、是否脱硫
		if(dev_code.Trim() == "TMTL")
		{
			Log::Trace(" ", "f_tmsm_mm", "*******脱硫*******");

			ttmsm11.Reset();
			ttmsm11.SM_UNIT_NO = sm_unit_no;
		 	// 脱硫：调用事件号、铁水包号、倒罐处理号、铁水预处理号、是否脱硫
		 	ttmsm11.IRON_LADLE_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["IRON_LADLE_NO"].ToString().Trim();
		 	ttmsm11.TPD_NO			= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["TPD_NO"].ToString().Trim();
		 	ttmsm11.IS_STANDARD_JAR	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["IS_STANDARD_JAR"].ToString().Trim();
		 	ttmsm11.IRON_LTREAT_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["IRON_LTREAT_NO"].ToString().Trim();
		 	ttmsm11.IS_IRON_LTREAT	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["IS_IRON_LTREAT"].ToString().Trim();
		 	ttmsm11.MOLTIRON_WT		= (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["MOLTIRON_WT"];
			
			Log::Trace(" ", "f_tmsm_mm", "ttmsm11.SM_UNIT_NO[{0}]"		,(const char*)ttmsm11.SM_UNIT_NO);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm11.IRON_LADLE_NO[{0}]"	,(const char*)ttmsm11.IRON_LADLE_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.TPD_NO[{0}]"			,(const char*)ttmsm11.TPD_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.IS_STANDARD_JAR[{0}]"	,(const char*)ttmsm11.IS_STANDARD_JAR);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.IRON_LTREAT_NO[{0}]"	,(const char*)ttmsm11.IRON_LTREAT_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.IS_IRON_LTREAT[{0}]"	,(const char*)ttmsm11.IS_IRON_LTREAT);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.MOLTIRON_WT[{0}]"		,ttmsm11.MOLTIRON_WT);
	
		 	if (ttmsm11.IRON_LADLE_NO.Trim() == "")
			{
				Log::Trace(" ", "f_tmsm_mm", "*******铁水包为空*******");
				//sprintf	(s.msg , "传入的铁水包号不能为空") ;
				//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
			}
			else
			{
				sqlstr = " SELECT COUNT(1) FROM TTMSM11 "
					" WHERE IRON_LADLE_NO	= @ttmsm11.IRON_LADLE_NO "
					" AND SM_UNIT_NO		= @ttmsm11.SM_UNIT_NO "
					;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
				cmd_inq.Parameters.Set( "ttmsm11.SM_UNIT_NO"	, ttmsm11.SM_UNIT_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if(cmd_inq.Read())
				{
					count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
				}
				cmd_inq.Close();
				Log::Trace("","f_tmsm_mm"," ttmsm11 count [{0}]",count );
				
				if	(count == 0)
				{
					Log::Trace(" ", "f_tmsm_mm", "*******铁水包不存在*******");
					//sprintf	(s.msg , "传入的铁水包号不存在") ;
					//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
				}
				else // 修改ttmsm11表
				{
					// 计算包龄先
					sqlstr1 = " SELECT MODEL_ID,IS_STANDARD_JAR,TPD_NO,IS_IRON_LTREAT,IRON_LTREAT_NO,MOLTIRON_WT "
						" FROM TTMSM11 "
						" WHERE IRON_LADLE_NO	= @ttmsm11.IRON_LADLE_NO "
						" AND SM_UNIT_NO		= @ttmsm11.SM_UNIT_NO "
					;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
					cmd_inq1.Parameters.Set( "ttmsm11.SM_UNIT_NO"		, ttmsm11.SM_UNIT_NO );
					cmd_inq1.ExecuteReader();
					if(cmd_inq1.Read())
					{
						ttmsm11.MODEL_ID		= cmd_inq1.GetString(1);
						//ttmsm11.IS_STANDARD_JAR = cmd_inq1.GetString(2);//传入值
						//ttmsm11.TPD_NO			= cmd_inq1.GetString(3);//传入值
						//ttmsm11.IS_IRON_LTREAT	= cmd_inq1.GetString(4);//传入值
						//ttmsm11.IRON_LTREAT_NO	= cmd_inq1.GetString(5);//传入值
						//ttmsm11.MOLTIRON_WT	= cmd_inq1.GetDecimal(6);//传入值
					}
					cmd_inq1.Close();

					sqlstr1 = " SELECT * "
						" FROM   TTMSM99 "
						" WHERE  MODEL_ID = @ttmsm11.MODEL_ID ";
					;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set( "ttmsm11.MODEL_ID"	, ttmsm11.MODEL_ID );
					cmd_inq1.ExecuteReader();
					ttmsm99.Reset();
					if(cmd_inq1.Read())
					{
						cmd_inq1.Fetch(ttmsm99);
					}
					cmd_inq1.Close();

					// 此处是脱硫实绩 把上次脱硫的值、包龄都清掉
					//ttmsm11.IS_STANDARD_JAR = "0";	// 是否标准罐
					//ttmsm11.TPD_NO			= " ";	// 倒罐处理号
					//ttmsm11.IS_IRON_LTREAT	= "0";	// 是否脱硫
					//ttmsm11.IRON_LTREAT_NO	= " ";	// 铁水预处理号
					ttmsm11.IRON_LADLE_LIFE = 0;

					// 倒罐
					if(ttmsm11.IS_STANDARD_JAR.Trim() == "0" || ttmsm11.IS_STANDARD_JAR.Trim() == "")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.TPD_STD_PARA;
					}
					else if(ttmsm11.IS_STANDARD_JAR.Trim() == "1")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.TPD_NSTD_PARA;
					}
					// 来水
					if(ttmsm11.MOLTIRON_WT <= ttmsm99.MOLTIRON_WT_A)
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.MOLTIRON_PARA_X;
					}
					else if(ttmsm99.MOLTIRON_WT_A < ttmsm11.MOLTIRON_WT < ttmsm99.MOLTIRON_WT_B)
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.MOLTIRON_PARA_Y;
					}
					else if(ttmsm11.MOLTIRON_WT >= ttmsm99.MOLTIRON_WT_B)
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.MOLTIRON_PARA_Z;
					}
					// 脱硫
					if(ttmsm11.IS_IRON_LTREAT.Trim() == "0" || ttmsm11.IS_IRON_LTREAT.Trim() == "")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.NS_PARA;
					}
					else if(ttmsm11.IS_IRON_LTREAT.Trim() == "1")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.S_PARA;
					}

					ttmsm11.REC_REVISOR = s.userid;   //记录修改责任者
					ttmsm11.REC_REVISE_TIME =datetimeNow; //记录修改时刻
					ttmsm11.Update(" REC_REVISOR "
						" ,REC_REVISE_TIME "
						" , MOLTIRON_WT "
						" , IS_STANDARD_JAR "
						" , TPD_NO "
						" , IRON_LTREAT_NO "
						" , IS_IRON_LTREAT "
						" , IRON_LADLE_LIFE ",
						" SM_UNIT_NO, IRON_LADLE_NO ");
				}
			}
		}
		
		// 转炉实绩 A~F
		if(dev_code.Trim().Compare("A") >= 0 && dev_code.Trim().Compare("F") <= 0)//if(event_id.Trim() == "TMZL")
		{
			Log::Trace(" ", "f_tmsm_mm", "*******转炉*******");
			Log::Trace(" ", "f_tmsm_mm", "*******转炉铁水包更新*******");
			// 铁水包使用实绩一次
			ttmsm11.Reset();
			ttmsm11.SM_UNIT_NO = sm_unit_no;
			// 转炉：调用事件号、铁水包号、倒罐处理号、铁水预处理号、是否脱硫
		 	ttmsm11.IRON_LADLE_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["IRON_LADLE_NO"].ToString().Trim();
		 	// 倒罐处理号和铁水预处理号收一下看看 下面操作还是用表里的吧
			ttmsm11.TPD_NO			= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["TPD_NO"].ToString().Trim();
		 	ttmsm11.IRON_LTREAT_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["IRON_LTREAT_NO"].ToString().Trim();

			Log::Trace(" ", "f_tmsm_mm", "ttmsm11.SM_UNIT_NO[{0}]"		,(const char*)ttmsm11.SM_UNIT_NO);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm11.IRON_LADLE_NO[{0}]"	,(const char*)ttmsm11.IRON_LADLE_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.TPD_NO[{0}]"			,(const char*)ttmsm11.TPD_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm11.IRON_LTREAT_NO[{0}]"	,(const char*)ttmsm11.IRON_LTREAT_NO);
	
		 	if (ttmsm11.IRON_LADLE_NO.Trim() == "")
			{
				Log::Trace(" ", "f_tmsm_mm", "*******铁水包为空*******");
				//sprintf	(s.msg , "传入的铁水包号不能为空") ;
				//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
			}
			else
			{
				sqlstr = " SELECT COUNT(1) FROM TTMSM11 "
					" WHERE IRON_LADLE_NO	= @ttmsm11.IRON_LADLE_NO "
					" AND SM_UNIT_NO		= @ttmsm11.SM_UNIT_NO "
					;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
				cmd_inq.Parameters.Set( "ttmsm11.SM_UNIT_NO"	, ttmsm11.SM_UNIT_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if(cmd_inq.Read())
				{
					count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
				}
				cmd_inq.Close();
				Log::Trace("","f_tmsm_mm"," ttmsm11 count [{0}]",count );
				
				if	(count == 0)
				{
					Log::Trace(" ", "f_tmsm_mm", "*******铁水包不存在*******");
					//sprintf	(s.msg , "传入的铁水包号不存在") ;
					//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
				}
				else // 修改ttmsm11表
				{
					// 计算包龄先
					sqlstr1 = " SELECT * "
						" FROM TTMSM11 "
						" WHERE IRON_LADLE_NO	= @ttmsm11.IRON_LADLE_NO "
						" AND SM_UNIT_NO		= @ttmsm11.SM_UNIT_NO "
					;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
					cmd_inq1.Parameters.Set( "ttmsm11.SM_UNIT_NO"		, ttmsm11.SM_UNIT_NO );
					cmd_inq1.ExecuteReader();
					if(cmd_inq1.Read())
					{
						cmd_inq1.Fetch(ttmsm11);
					}
					cmd_inq1.Close();

					sqlstr1 = " SELECT * "
						" FROM   TTMSM99 "
						" WHERE  MODEL_ID = @ttmsm11.MODEL_ID ";
					;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set( "ttmsm11.MODEL_ID"	, ttmsm11.MODEL_ID );
					cmd_inq1.ExecuteReader();
					ttmsm99.Reset();
					if(cmd_inq1.Read())
					{
						cmd_inq1.Fetch(ttmsm99);
					}
					cmd_inq1.Close();

					// 此处是脱硫实绩 把上次脱硫的值、包龄都清掉
					//ttmsm11.IS_STANDARD_JAR = "0";	// 是否标准罐
					//ttmsm11.TPD_NO			= " ";	// 倒罐处理号
					//ttmsm11.IS_IRON_LTREAT	= "0";	// 是否脱硫
					//ttmsm11.IRON_LTREAT_NO	= " ";	// 铁水预处理号
					ttmsm11.IRON_LADLE_LIFE = 0;

					// 倒罐
					if(ttmsm11.IS_STANDARD_JAR.Trim() == "0" || ttmsm11.IS_STANDARD_JAR.Trim() == "")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.TPD_STD_PARA;
					}
					else if(ttmsm11.IS_STANDARD_JAR.Trim() == "1")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.TPD_NSTD_PARA;
					}
					// 来水
					if(ttmsm11.MOLTIRON_WT <= ttmsm99.MOLTIRON_WT_A)
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.MOLTIRON_PARA_X;
					}
					else if(ttmsm99.MOLTIRON_WT_A < ttmsm11.MOLTIRON_WT < ttmsm99.MOLTIRON_WT_B)
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.MOLTIRON_PARA_Y;
					}
					else if(ttmsm11.MOLTIRON_WT >= ttmsm99.MOLTIRON_WT_B)
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.MOLTIRON_PARA_Z;
					}
					// 脱硫
					if(ttmsm11.IS_IRON_LTREAT.Trim() == "0" || ttmsm11.IS_IRON_LTREAT.Trim() == "")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.NS_PARA;
					}
					else if(ttmsm11.IS_IRON_LTREAT.Trim() == "1")
					{
						ttmsm11.IRON_LADLE_LIFE = ttmsm11.IRON_LADLE_LIFE + ttmsm99.S_PARA;
					}

					ttmsm11.REC_REVISOR = s.userid;   //记录修改责任者
					ttmsm11.REC_REVISE_TIME =datetimeNow; //记录修改时刻
					// 生成使用流水号REPAIR_SEQ_NO
					sqlstr1 =	" SELECT nvl(MAX(USE_SEQ_NO),0) "
								" FROM TTMSM19 "
								" WHERE SM_UNIT_NO	= @ttmsm19.SM_UNIT_NO "
								" AND IRON_LADLE_NO = @ttmsm19.IRON_LADLE_NO "
								;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set("ttmsm19.SM_UNIT_NO"	, ttmsm11.SM_UNIT_NO);
					cmd_inq1.Parameters.Set("ttmsm19.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO);
					cmd_inq1.ExecuteReader();
					if ( cmd_inq1.Read() )
					{
						ttmsm11.USE_SEQ_NO = cmd_inq1.GetInt32(1)+1;
					}
					cmd_inq1.Close();
					Log::Trace("","f_tmsm_mm","ttmsm11.USE_SEQ_NO [{0}]",ttmsm11.USE_SEQ_NO);

					ttmsm11.Update(" REC_REVISOR "
						" , REC_REVISE_TIME "
						" ,	USE_SEQ_NO "
						" , IRON_LADLE_LIFE ",
						" SM_UNIT_NO, IRON_LADLE_NO ");

					// 生成铁水包实绩
					ttmsm19.Reset();
					ttmsm19.CopyFrom(ttmsm11);					
					ttmsm19.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
					if(ttmsm19.REC_CREATOR.Trim() == "")
					{
						ttmsm19.REC_CREATOR = "XCOM";		//记录创建责任者
					}
					ttmsm19.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
					ttmsm19.REC_REVISOR = " ";
					ttmsm19.REC_REVISE_TIME = " ";
					ttmsm19.TrimOrBlank();
					ttmsm19.Print();
					ttmsm19.Insert();
				}
			}

			Log::Trace(" ", "f_tmsm_mm", "*******转炉钢包更新*******");
			// 钢包修改属性
			ttmsm01.Reset();
			ttmsm01.SM_UNIT_NO = sm_unit_no;
			ttmsm01.SM_PLAN_NO = (CString)bcls_rec->Tables["TMSM99"].Rows[0]["SM_PLAN_NO"].ToString().Trim();
		 	// 转炉实绩：调用事件号、钢包号、钢包状态（热包、温包、凉包）、钢水净重、吹氩时间累计、出钢开始时刻
		 	ttmsm01.LADLE_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_NO"].ToString().Trim();
		 	//电文里的钢包状态1:热包 2:温包 3:黑包 4:凉包
		 	//钢包状态00-新包 01-可用 02-凉包 03-黑包 11-烘烤 21-使用 31-维修 32-粘钢 99-报废
		 	ttmsm01.LADLE_STATUS= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_STATUS"].ToString().Trim();
		 	steel_net_weight	= (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["STEEL_NET_WEIGHT"];
			ttmsm01.LADLE_W_L	= steel_net_weight.Round(3);
			ttmsm01.AR_BLOW_TIME_TOTAL = (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["AR_DURATION"];
			ttmsm01.USAGE_ST	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["TAP_START_TIME"].ToString().Trim();
			ttmsm01.USAGE_ET = " ";
			ttmsm01.EMPTY_LADLE_WT = (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["STEEL_TARE_WT"]; //钢包皮重
			//ttmsm01.LADLE_W_L = (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["STEEL_NET_WEIGHT"];	//钢水重量

			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.SM_UNIT_NO[{0}]"		,(const char*)ttmsm01.SM_UNIT_NO);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.SM_PLAN_NO[{0}]"		,(const char*)ttmsm01.SM_PLAN_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LADLE_NO[{0}]"		,(const char*)ttmsm01.LADLE_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LADLE_STATUS[{0}]"	,(const char*)ttmsm01.LADLE_STATUS);
			Log::Trace(" ", "f_tmsm_mm", "steel_net_weight[{0}]"		,steel_net_weight);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LADLE_W_L[{0}]"		,ttmsm01.LADLE_W_L);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.AR_BLOW_TIME_TOTAL[{0}]",ttmsm01.AR_BLOW_TIME_TOTAL);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.USAGE_ST[{0}]"		,(const char*)ttmsm01.USAGE_ST);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.EMPTY_LADLE_WT[{0}]"	,ttmsm01.EMPTY_LADLE_WT);
			//Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LADLE_W_L[{0}]"		,ttmsm01.LADLE_W_L);

			if (ttmsm01.LADLE_NO.Trim() == "")
			{
				Log::Trace(" ", "f_tmsm_mm", "*******传入钢包号为空*******");
				//sprintf	(s.msg , "传入的钢包号不能为空") ;
				//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
				sqlstr = " SELECT LADLE_NO "
						" FROM TTMSM02 "
						" WHERE SM_PLAN_NO = @ttmsm01.SM_PLAN_NO "
						" AND SM_UNIT_NO = @ttmsm01.SM_UNIT_NO "
						;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm01.SM_PLAN_NO", ttmsm01.SM_PLAN_NO );
				cmd_inq.Parameters.Set( "ttmsm01.SM_UNIT_NO", ttmsm01.SM_UNIT_NO );
				cmd_inq.ExecuteReader();
				if(cmd_inq.Read())
				{
					ttmsm01.LADLE_NO = cmd_inq.GetString(1); //将数据获取到实体对象中				
				}
				cmd_inq.Close();
				Log::Trace("","f_tmsm_mm"," 查找ttmsm02 ttmsm01.LADLE_NO [{0}]",(const char*)ttmsm01.LADLE_NO );
			}
		 		
		 	if (ttmsm01.LADLE_NO.Trim() == "")
			{
				Log::Trace(" ", "f_tmsm_mm", "*******查找TTMSM02表 钢包号为空*******");
			}
			else
			{
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
				Log::Trace(" ", "f_tmsm_mm", "转换后的LADLE_STATUS[{0}]",(const char*)ttmsm01.LADLE_STATUS);

				// 转换装钢量
				ttmsm01.LOAD_STEEL_WT = ttmsm01.LADLE_W_L *1000;
				Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LOAD_STEEL_WT[{0}]"	,ttmsm01.LOAD_STEEL_WT);

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
				Log::Trace("","f_tmsm_mm"," ttmsm01 count [{0}]",count );
				
				if	(count == 0)
				{
					Log::Trace(" ", "f_tmsm_mm", "*******钢包不存在*******");
					//sprintf	(s.msg , "传入的钢包号不存在") ;
					//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
				}
				else // 修改TTMSM01表中该钢包的包状态与钢水净重信息
				{
					// 计算累计装钢量
					sqlstr1 = " SELECT LOAD_STEEL_WT_TOTAL FROM TTMSM01 "
						" WHERE LADLE_NO = @ttmsm01.LADLE_NO "
						" AND SM_UNIT_NO = @ttmsm01.SM_UNIT_NO "
						;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set( "ttmsm01.LADLE_NO"		, ttmsm01.LADLE_NO );
					cmd_inq1.Parameters.Set( "ttmsm01.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO );
					cmd_inq1.ExecuteReader();
					if(cmd_inq1.Read())
					{
						ttmsm01.LOAD_STEEL_WT_TOTAL = cmd_inq1.GetDecimal(1); //将数据获取到实体对象中				
					}
					cmd_inq1.Close();
					Log::Trace("","f_tmsm_mm"," ttmsm01.LOAD_STEEL_WT_TOTAL[{0}]",ttmsm01.LOAD_STEEL_WT_TOTAL );
					ttmsm01.LOAD_STEEL_WT_TOTAL = ttmsm01.LOAD_STEEL_WT_TOTAL + ttmsm01.LOAD_STEEL_WT;
					Log::Trace("","f_tmsm_mm"," new ttmsm01.LOAD_STEEL_WT_TOTAL[{0}]",ttmsm01.LOAD_STEEL_WT_TOTAL );
					
					ttmsm01.REC_REVISOR = s.userid;   //记录修改责任者
					ttmsm01.REC_REVISE_TIME =datetimeNow; //记录修改时刻
					ttmsm01.Update(" REC_REVISOR "
						" , REC_REVISE_TIME "
						" , USAGE_ST "
						" , USAGE_ET "
						" , LADLE_STATUS "
						" , LADLE_W_L "
						" , AR_BLOW_TIME_TOTAL "
						" , LOAD_STEEL_WT "
						" , LOAD_STEEL_WT_TOTAL ",							
						" SM_UNIT_NO, LADLE_NO ");
				}
			}
		}
		
		// LF实绩 K~N
		if(dev_code.Trim().Compare("K") >= 0 && dev_code.Trim().Compare("N") <= 0)//if(event_id.Trim() == "TMLF")
		{
			Log::Trace(" ", "f_tmsm_mm", "*******LF*******");
			ttmsm01.Reset();
			ttmsm01.SM_UNIT_NO = sm_unit_no;

		 	// LF实绩：调用事件号、钢包号、钢包状况、钢包使用次数、吹氩时间累计、钢包到达时刻
		 	ttmsm01.LADLE_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_NO"].ToString().Trim();
		 	ttmsm01.LD_STATUS	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_STATUS"].ToString().Trim();
		 	ttmsm01.LADLE_LIFE	= (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_USE_SUM"];
		 	ttmsm01.AR_BLOW_TIME_TOTAL	= (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["AR_DURATION"];
		 	ttmsm01.LADLE_ARRIVE_TIME	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_ARRIVE_TIME"].ToString().Trim();
			start_time			= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["START_TIME"].ToString().Trim();
		 	end_time			= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["END_TIME"].ToString().Trim();
			ttmsm01.GRP_NO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["PROD_SHIFT_GROUP"].ToString().Trim();
		 	ttmsm01.SHIFT_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["PROD_SHIFT_NO"].ToString().Trim();
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.SM_UNIT_NO[{0}]"		,(const char*)ttmsm01.SM_UNIT_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LADLE_NO[{0}]"		,(const char*)ttmsm01.LADLE_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LADLE_STATUS[{0}]"	,(const char*)ttmsm01.LADLE_STATUS);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LADLE_LIFE[{0}]"		,ttmsm01.LADLE_LIFE);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.AR_BLOW_TIME_TOTAL[{0}]",ttmsm01.AR_BLOW_TIME_TOTAL);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LADLE_ARRIVE_TIME[{0}]",(const char*)ttmsm01.LADLE_ARRIVE_TIME);			
		 	Log::Trace(" ", "f_tmsm_mm", "start_time[{0}]"				,(const char*)start_time);
			Log::Trace(" ", "f_tmsm_mm", "end_time[{0}]"				,(const char*)end_time);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.GRP_NO[{0}]"			,(const char*)ttmsm01.GRP_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.SHIFT_NO[{0}]"		,(const char*)ttmsm01.SHIFT_NO);
		 	
		 	if (ttmsm01.LADLE_NO.Trim() == "")
			{
				Log::Trace(" ", "f_tmsm_mm", "*******钢包号为空*******");
				//sprintf	(s.msg , "传入的钢包号不能为空") ;
				//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
			}	
			else
			{
				// 计算时间	精炼时间累计		
				CDateTime CTIME_START	= CDateTime::Parse(start_time); 
				CDateTime CTIME_END		= CDateTime::Parse(end_time);
				CTimeSpan ts			= CTIME_END - CTIME_START;
				ttmsm01.SRP_TIME_TOTAL	= ts.TotalMinutes();
				//int days = ts.Days();
				//int Minutes = ts.Minutes();//只对分钟计算
				//int Minutes_total = ts.TotalMinutes();//总的换算成分钟
				Log::Trace(" ", "f_tmsm_mm", "ttmsm01.SRP_TIME_TOTAL[{0}]"	,ttmsm01.SRP_TIME_TOTAL);
			 	
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
				Log::Trace("","f_tmsm_mm"," ttmsm01 count [{0}]",count );
				
				if	(count == 0)
				{
					Log::Trace(" ", "f_tmsm_mm", "*******钢包号不存在*******");
					//sprintf	(s.msg , "传入的钢包号不存在") ;
					//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
				}
				else // 修改TTMSM01表中
				{
					ttmsm01.REC_REVISOR = s.userid;   //记录修改责任者
					ttmsm01.REC_REVISE_TIME =datetimeNow; //记录修改时刻
					ttmsm01.Update(" REC_REVISOR "
						" ,REC_REVISE_TIME "
						" , LD_STATUS "
						//" , LADLE_LIFE "  // 20140309 不收了 老是清零
						" , AR_BLOW_TIME_TOTAL "
						" , LADLE_ARRIVE_TIME "
						" , SRP_TIME_TOTAL "
						" , GRP_NO "
						" , SHIFT_NO ",
						" SM_UNIT_NO, LADLE_NO ");
				}
			}
		}

		// 炉次连铸浇铸实绩 1~8
		if(dev_code.Trim().Compare("1") >= 0 && dev_code.Trim().Compare("8") <= 0)//if(event_id.Trim() == "TMLZ")
		{
			Log::Trace(" ", "f_tmsm_mm", "*******连铸*******");
		 	// 炉次连铸浇铸实绩：调用事件号、
			// 内部钢种ST_NO、HEAT_NO、PONO、SHIFT_NO、GRP_NO
			// 钢包号LADLE_NO、钢包到达时刻 LADLE_ARRIVE_TIME 钢包离开时刻 LADLE_LEAVE_TIME
			// ladle_weight_dep 钢包离开重量
			//用tmmsm31接中间包号和结晶器号
			tmmsm31.Reset();
			tmmsm31.MergeFrom(bcls_rec->Tables["TMSM99"].Rows[0]);

			Log::Trace(" ", "f_tmsm_mm", "*******连铸钢包更新*******");
			ttmsm01.Reset();
			ttmsm01.SM_UNIT_NO	= sm_unit_no;
		 	ttmsm01.LADLE_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_NO"].ToString().Trim();
			ttmsm01.ST_NO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["ST_NO"].ToString().Trim();
			ttmsm01.HEAT_NO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["HEAT_NO"].ToString().Trim();
			ttmsm01.PONO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["PONO"].ToString().Trim();
			ttmsm01.SHIFT_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["PROD_SHIFT_NO"].ToString().Trim();
			ttmsm01.GRP_NO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["PROD_SHIFT_GROUP"].ToString().Trim();
			ttmsm01.LADLE_ARRIVE_TIME = (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_ARRIVE_TIME"].ToString().Trim();
			ttmsm01.USAGE_ET	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_LEAVE_TIME"].ToString().Trim();
			ttmsm01.EMPTY_LADLE_WT = (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_WEIGHT_DEP"];	// ton
			ttmsm01.LADLE_W_L	= (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_W_L"];			// *1000 = ton
			ttmsm01.LOAD_STEEL_WT = ttmsm01.LADLE_W_L*1000;
			
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.SM_UNIT_NO[{0}]"	,(const char*)ttmsm01.SM_UNIT_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LADLE_NO[{0}]"	,(const char*)ttmsm01.LADLE_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.ST_NO[{0}]"		,(const char*)ttmsm01.ST_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.HEAT_NO[{0}]"		,(const char*)ttmsm01.HEAT_NO);
		 	Log::Trace(" ", "f_tmsm_mm", "ttmsm01.PONO[{0}]"		,(const char*)ttmsm01.PONO);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.SHIFT_NO[{0}]"	,(const char*)ttmsm01.SHIFT_NO);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.GRP_NO[{0}]"		,(const char*)ttmsm01.GRP_NO);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LADLE_ARRIVE_TIME[{0}]"	,(const char*)ttmsm01.LADLE_ARRIVE_TIME);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.USAGE_ET[{0}]"	,(const char*)ttmsm01.USAGE_ET);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.EMPTY_LADLE_WT[{0}]"	,ttmsm01.EMPTY_LADLE_WT);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LADLE_W_L[{0}]"		,ttmsm01.LADLE_W_L);
			Log::Trace(" ", "f_tmsm_mm", "ttmsm01.LOAD_STEEL_WT[{0}]"	,ttmsm01.LOAD_STEEL_WT);
		 	
			if (ttmsm01.LADLE_NO.Trim() == "")
			{
				Log::Trace(" ", "f_tmsm_mm", "*******钢包号为空*******");
				//sprintf	(s.msg , "传入的钢包号不能为空") ;
				//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
			}
			else
			{
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
				Log::Trace("","f_tmsm_mm"," ttmsm01 count [{0}]",count );
				
				if	(count == 0)
				{
					Log::Trace(" ", "f_tmsm_mm", "*******钢包号不存在*******");
					//sprintf	(s.msg , "传入的钢包号不存在") ;
					//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
				}
				else // 修改TTMSM01表中该钢包的包状态与钢水净重信息
				{
					ttmsm01.REC_REVISOR = s.userid;   //记录修改责任者
					ttmsm01.REC_REVISE_TIME =datetimeNow; //记录修改时刻

					// 计算累计装钢量
					sqlstr1 = " SELECT LOAD_STEEL_WT_TOTAL FROM TTMSM01 "
						" WHERE LADLE_NO = @ttmsm01.LADLE_NO "
						" AND SM_UNIT_NO = @ttmsm01.SM_UNIT_NO "
						;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set( "ttmsm01.LADLE_NO"		, ttmsm01.LADLE_NO );
					cmd_inq1.Parameters.Set( "ttmsm01.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO );
					cmd_inq1.ExecuteReader();
					ttmsm01.LOAD_STEEL_WT_TOTAL = 0;
					if(cmd_inq1.Read())
					{
						ttmsm01.LOAD_STEEL_WT_TOTAL = cmd_inq1.GetDecimal(1); //将数据获取到实体对象中				
					}
					cmd_inq1.Close();
					Log::Trace("","f_tmsm_mm"," ttmsm01.LOAD_STEEL_WT_TOTAL[{0}]",ttmsm01.LOAD_STEEL_WT_TOTAL );
					ttmsm01.LOAD_STEEL_WT_TOTAL = ttmsm01.LOAD_STEEL_WT_TOTAL + ttmsm01.LOAD_STEEL_WT;
					Log::Trace("","f_tmsm_mm"," new ttmsm01.LOAD_STEEL_WT_TOTAL[{0}]",ttmsm01.LOAD_STEEL_WT_TOTAL );

					ttmsm01.Update(" REC_REVISOR "
						" , REC_REVISE_TIME "
						//" , USE_SEQ_NO "
						" , ST_NO "
						" , HEAT_NO "
						" , PONO "
						" , SHIFT_NO "
						" , GRP_NO "
						//" , USAGE_ET "
						" , LADLE_ARRIVE_TIME "
						" , EMPTY_LADLE_WT "		// 20140309 空包重量
						" , LADLE_W_L "				// 20140309 钢水量
						" , LOAD_STEEL_WT "			// 20140309 装钢量
						" , LOAD_STEEL_WT_TOTAL ",	// 20140309 装钢量累计
						" SM_UNIT_NO, LADLE_NO ");

					// 查找最大使用流水号REPAIR_SEQ_NO 更新其中的装钢量和装钢量累计
					ttmsm09.CopyFrom(ttmsm01);
					sqlstr1 =	" SELECT nvl(MAX(USE_SEQ_NO),0) "
								" FROM TTMSM09 "
								" WHERE SM_UNIT_NO	= @ttmsm09.SM_UNIT_NO "
								" AND LADLE_NO 		= @ttmsm09.LADLE_NO "
								;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set("ttmsm09.SM_UNIT_NO", ttmsm01.SM_UNIT_NO);
					cmd_inq1.Parameters.Set("ttmsm09.LADLE_NO"	, ttmsm01.LADLE_NO);
					cmd_inq1.ExecuteReader();
					if ( cmd_inq1.Read() )
					{
						ttmsm09.USE_SEQ_NO = cmd_inq1.GetInt32(1);
					}
					cmd_inq1.Close();
					Log::Trace("","f_tmsm_mm","ttmsm09.USE_SEQ_NO [{0}]",ttmsm09.USE_SEQ_NO);
					Log::Trace("","f_tmsm_mm","ttmsm09.SM_UNIT_NO [{0}]",ttmsm09.SM_UNIT_NO);
					Log::Trace("","f_tmsm_mm","ttmsm09.LADLE_NO [{0}]"	,ttmsm09.LADLE_NO);
					ttmsm09.Update(" REC_REVISOR "
						" , REC_REVISE_TIME "
						" , LADLE_W_L "				// 20140309 钢水量
						" , LOAD_STEEL_WT "			// 20140309 装钢量
						" , LOAD_STEEL_WT_TOTAL ",	// 20140309 装钢量累计
						" SM_UNIT_NO, LADLE_NO,USE_SEQ_NO ");
				}
			}			
			
			Log::Trace(" ", "f_tmsm_mm", "*******连铸中间罐更新*******");
			// 中间包号1 TD_NO_1、中间包罐龄1 TD_LIFE_1、中间包号2 TD_NO_2、中间包罐龄2 TD_LIFE_2、
			// 大包开浇时刻LADLE_OPEN_TIME、大包浇完时刻LADLE_CLOSE_TIME、
			//搞个块放中间包
			tmbw_bcls_rec.Clear();
			if(!tmbw_bcls_rec.Tables.Contains("TMSM21")) tmbw_bcls_rec.Tables.Add("TMSM21");
			if(!tmbw_bcls_rec.Tables["TMSM21"].Columns.Contains("TD_NO")) tmbw_bcls_rec.Tables["TMSM21"].Columns.Add(DT_STRING,"TD_NO");
			if(!tmbw_bcls_rec.Tables["TMSM21"].Columns.Contains("LADLE_LIFE")) tmbw_bcls_rec.Tables["TMSM21"].Columns.Add(DT_INT32,"LADLE_LIFE");
			// tmmsm31上面接过参数了
			// tmmsm31.Reset();
			// tmmsm31.MergeFrom(bcls_rec->Tables["TMSM99"].Rows[0]);
			// 眼睛睁大 是MM的表结构 不是TM的表结构 别改错了~~~
			/*Log::Trace(" ", "f_tmsm_mm", "tmmsm31.TD_NO_1[{0}]" ,(const char*)tmmsm31.TD_NO_1);
			Log::Trace(" ", "f_tmsm_mm", "tmmsm31.TD_LIFE_1[{0}]" ,tmmsm31.TD_LIFE_1);
			Log::Trace(" ", "f_tmsm_mm", "tmmsm31.TD_NO_2[{0}]" ,(const char*)tmmsm31.TD_NO_2);
			Log::Trace(" ", "f_tmsm_mm", "tmmsm31.TD_LIFE_2[{0}]" ,tmmsm31.TD_LIFE_2);*/
			int td_cnt = 0;
			/*if (tmmsm31.TD_NO_1.Trim() != "")
			{
				tmbw_bcls_rec.Tables["TMSM21"].Rows.Add();
				tmbw_bcls_rec.Tables["TMSM21"].Rows[td_cnt]["TD_NO"] = tmmsm31.TD_NO_1;
				tmbw_bcls_rec.Tables["TMSM21"].Rows[td_cnt]["LADLE_LIFE"] = tmmsm31.TD_LIFE_1;
				td_cnt ++;
			}
			if (tmmsm31.TD_NO_2.Trim() != "")
			{
				tmbw_bcls_rec.Tables["TMSM21"].Rows.Add();
				tmbw_bcls_rec.Tables["TMSM21"].Rows[td_cnt]["TD_NO"] = tmmsm31.TD_NO_2;
				tmbw_bcls_rec.Tables["TMSM21"].Rows[td_cnt]["LADLE_LIFE"] = tmmsm31.TD_LIFE_2;
			}*/
			Log::Trace(" ", "f_tmsm_mm", "have a look at how many td rows :td_cnt[{0}]" ,td_cnt);

			int tmsm21_rows = tmbw_bcls_rec.Tables["TMSM21"].Rows.get_Count();
			Log::Trace(" ", "f_tmsm_mm", "tmsm21_rows[{0}]" ,tmsm21_rows);

			for(i = 0; i < tmsm21_rows ; i++)
			{
				ttmsm21.Reset();
				ttmsm21.SM_UNIT_NO	= sm_unit_no;
				// 下面几个就是rows[0] 别瞎改啊
				ttmsm21.CC_MACH_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["CC_NO"].ToString().Trim();			
				ttmsm21.ON_LINE_TIME= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_OPEN_TIME"].ToString().Trim();
				ttmsm21.OFF_LINE_TIME	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_CLOSE_TIME"].ToString().Trim();
				ttmsm21.ST_NO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["ST_NO"].ToString().Trim();
				ttmsm21.HEAT_NO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["HEAT_NO"].ToString().Trim();
				ttmsm21.PONO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["PONO"].ToString().Trim();
				ttmsm21.CAST_NO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["CAST_NO"].ToString().Trim();
				ttmsm21.CAST_DIV_NO	= (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["CAST_DIV_NO"];
				// 这个要写i的
				ttmsm21.TD_NO		= (CString)tmbw_bcls_rec.Tables["TMSM21"].Rows[i]["TD_NO"].ToString().Trim();
				ttmsm21.LADLE_LIFE	= (CDecimal)tmbw_bcls_rec.Tables["TMSM21"].Rows[i]["LADLE_LIFE"];
				
				Log::Trace(" ", "f_tmsm_mm", "ttmsm21.SM_UNIT_NO[{0}]"		,(const char*)ttmsm21.SM_UNIT_NO);
				Log::Trace(" ", "f_tmsm_mm", "ttmsm21.TD_NO[{0}]"			,(const char*)ttmsm21.TD_NO);
				Log::Trace(" ", "f_tmsm_mm", "ttmsm21.LADLE_LIFE[{0}]"		,ttmsm21.LADLE_LIFE);
		 		Log::Trace(" ", "f_tmsm_mm", "ttmsm21.CC_MACH_NO[{0}]"		,(const char*)ttmsm21.CC_MACH_NO);
		 		Log::Trace(" ", "f_tmsm_mm", "ttmsm21.ON_LINE_TIME[{0}]"	,(const char*)ttmsm21.ON_LINE_TIME);
		 		Log::Trace(" ", "f_tmsm_mm", "ttmsm21.OFF_LINE_TIME[{0}]"	,(const char*)ttmsm21.OFF_LINE_TIME);
				Log::Trace(" ", "f_tmsm_mm", "ttmsm21.ST_NO[{0}]"			,(const char*)ttmsm21.ST_NO);
				Log::Trace(" ", "f_tmsm_mm", "ttmsm21.HEAT_NO[{0}]"			,(const char*)ttmsm21.HEAT_NO);
				Log::Trace(" ", "f_tmsm_mm", "ttmsm21.PONO[{0}]"			,(const char*)ttmsm21.PONO);
				Log::Trace(" ", "f_tmsm_mm", "ttmsm21.CAST_NO[{0}]"			,(const char*)ttmsm21.CAST_NO);
		 		Log::Trace(" ", "f_tmsm_mm", "ttmsm21.CAST_DIV_NO[{0}]"		,ttmsm21.CAST_DIV_NO);

	 			sqlstr = " SELECT COUNT(1) FROM TTMSM21 "
					" WHERE TD_NO = @ttmsm21.LADLE_NO "
					" AND SM_UNIT_NO = @ttmsm21.SM_UNIT_NO "
					;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm21.TD_NO"		, ttmsm21.TD_NO );
				cmd_inq.Parameters.Set( "ttmsm21.SM_UNIT_NO", ttmsm21.SM_UNIT_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if(cmd_inq.Read())
				{
					count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
				}
				cmd_inq.Close();
				Log::Trace("","f_tmsm_mm"," ttmsm21 count [{0}]",count );
				
				if	(count == 0)
				{
					Log::Trace(" ", "f_tmsm_mm", "*******中间罐号不存在*******");
					//sprintf	(s.msg , "传入的钢包号不存在") ;
					//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
				}
				else // 修改TTMSM21表中该中间罐包的包状态与钢水净重信息
				{
					ttmsm21.REC_REVISOR = s.userid;   //记录修改责任者
					ttmsm21.REC_REVISE_TIME =datetimeNow; //记录修改时刻

					// 生成使用流水号REPAIR_SEQ_NO
					sqlstr1 =	" SELECT nvl(MAX(USE_SEQ_NO),0) "
								" FROM TTMSM29 "
								" WHERE SM_UNIT_NO	= @ttmsm29.SM_UNIT_NO "
								" AND TD_NO 		= @ttmsm29.TD_NO "
								;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set("ttmsm29.SM_UNIT_NO", ttmsm21.SM_UNIT_NO);
					cmd_inq1.Parameters.Set("ttmsm29.TD_NO"		, ttmsm21.TD_NO);
					cmd_inq1.ExecuteReader();
					if ( cmd_inq1.Read() )
					{
						ttmsm21.USE_SEQ_NO = cmd_inq1.GetInt32(1)+1;
					}
					cmd_inq1.Close();
					Log::Trace("","f_tmsm_mm","ttmsm21.USE_SEQ_NO [{0}]",ttmsm21.USE_SEQ_NO);

					ttmsm21.Update(" REC_REVISOR "
						" , REC_REVISE_TIME "
						" , USE_SEQ_NO "
						" , ST_NO "
						" , HEAT_NO "
						" , PONO "
						" , CAST_NO "
						" , CAST_DIV_NO "
						" , CC_MACH_NO "
						" , ON_LINE_TIME "
						" , OFF_LINE_TIME "
						" , LADLE_LIFE ",
						" SM_UNIT_NO, TD_NO ");

					/*新增中间罐使用实绩表*/
					ttmsm29.Reset();
					ttmsm29.CopyFrom(ttmsm01);
					ttmsm29.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
					if(ttmsm29.REC_CREATOR.Trim() == "")
					{
						ttmsm29.REC_CREATOR = "XCOM";		//记录创建责任者
					}
					ttmsm29.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
					ttmsm29.REC_REVISOR = " ";
					ttmsm29.REC_REVISE_TIME = " ";
					ttmsm29.TrimOrBlank();
					ttmsm29.Print();
					ttmsm29.Insert();
				}
			}

			

			
			
			


			





			

			Log::Trace(" ", "f_tmsm_mm", "*******连铸结晶器更新*******");
			// 1~8流结晶器 MOLD_NO1~8、1~8流结晶器使用炉数 MOLD1_NUM~MOLD8_NUM
			// 连铸机号 CC_NO
			//搞个块放结晶器
			tmbw_bcls_rec.Clear();
			if(!tmbw_bcls_rec.Tables.Contains("TMSM31")) tmbw_bcls_rec.Tables.Add("TMSM31");
			if(!tmbw_bcls_rec.Tables["TMSM31"].Columns.Contains("MOLD_NO")) tmbw_bcls_rec.Tables["TMSM31"].Columns.Add(DT_STRING,"MOLD_NO");
			if(!tmbw_bcls_rec.Tables["TMSM31"].Columns.Contains("MOLD_NUM")) tmbw_bcls_rec.Tables["TMSM31"].Columns.Add(DT_INT32,"MOLD_NUM");
			// tmmsm31上面接过参数了
			// tmmsm31.Reset();
			// tmmsm31.MergeFrom(bcls_rec->Tables["TMSM99"].Rows[0]);
			/*Log::Trace(" ", "f_tmsm_mm", "tmmsm31.MOLD_NO1[{0}]" ,(const char*)tmmsm31.MOLD_NO1);
			Log::Trace(" ", "f_tmsm_mm", "tmmsm31.MOLD1_NUM[{0}]" ,tmmsm31.MOLD1_NUM);
			Log::Trace(" ", "f_tmsm_mm", "tmmsm31.MOLD_NO2[{0}]" ,(const char*)tmmsm31.MOLD_NO2);
			Log::Trace(" ", "f_tmsm_mm", "tmmsm31.MOLD2_NUM[{0}]" ,tmmsm31.MOLD2_NUM);
			Log::Trace(" ", "f_tmsm_mm", "tmmsm31.MOLD_NO3[{0}]" ,(const char*)tmmsm31.MOLD_NO3);
			Log::Trace(" ", "f_tmsm_mm", "tmmsm31.MOLD3_NUM[{0}]" ,tmmsm31.MOLD3_NUM);
			Log::Trace(" ", "f_tmsm_mm", "tmmsm31.MOLD_NO4[{0}]" ,(const char*)tmmsm31.MOLD_NO4);
			Log::Trace(" ", "f_tmsm_mm", "tmmsm31.MOLD4_NUM[{0}]" ,tmmsm31.MOLD4_NUM);*/
		
			int md_cnt = 0;
			/*if (tmmsm31.MOLD_NO1.Trim() != "")
			{
				tmbw_bcls_rec.Tables["TMSM31"].Rows.Add();
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NO"] = tmmsm31.MOLD_NO1;
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NUM"] = tmmsm31.MOLD1_NUM;
				md_cnt++;
			}
			if (tmmsm31.MOLD_NO2.Trim() != "")
			{
				tmbw_bcls_rec.Tables["TMSM31"].Rows.Add();
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NO"] = tmmsm31.MOLD_NO2;
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NUM"] = tmmsm31.MOLD2_NUM;
				md_cnt++;
			}
			if (tmmsm31.MOLD_NO3.Trim() != "")
			{
				tmbw_bcls_rec.Tables["TMSM31"].Rows.Add();
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NO"] = tmmsm31.MOLD_NO3;
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NUM"] = tmmsm31.MOLD3_NUM;
				md_cnt++;
			}
			if (tmmsm31.MOLD_NO4.Trim() != "")
			{
				tmbw_bcls_rec.Tables["TMSM31"].Rows.Add();
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NO"] = tmmsm31.MOLD_NO4;
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NUM"] = tmmsm31.MOLD4_NUM;
				md_cnt++;
			}*/
		/*	if (tmmsm31.MOLD_NO5.Trim() != "")
			{
				tmbw_bcls_rec.Tables["TMSM31"].Rows.Add();
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NO"] = tmmsm31.MOLD_NO5;
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NUM"] = tmmsm31.MOLD5_NUM;
				md_cnt++;
			}
			if (tmmsm31.MOLD_NO6.Trim() != "")
			{
				tmbw_bcls_rec.Tables["TMSM31"].Rows.Add();
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NO"] = tmmsm31.MOLD_NO6;
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NUM"] = tmmsm31.MOLD6_NUM;
				md_cnt++;
			}
			if (tmmsm31.MOLD_NO7.Trim() != "")
			{
				tmbw_bcls_rec.Tables["TMSM31"].Rows.Add();
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NO"] = tmmsm31.MOLD_NO7;
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NUM"] = tmmsm31.MOLD7_NUM;
				md_cnt++;
			}
			if (tmmsm31.MOLD_NO8.Trim() != "")
			{
				tmbw_bcls_rec.Tables["TMSM31"].Rows.Add();
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NO"] = tmmsm31.MOLD_NO8;
				tmbw_bcls_rec.Tables["TMSM31"].Rows[md_cnt]["MOLD_NUM"] = tmmsm31.MOLD8_NUM;
				md_cnt++;
			}*/
			Log::Trace(" ", "f_tmsm_mm", "have a look at how many md rows :td_cnt[{0}]" ,md_cnt);

			int tmsm31_rows = tmbw_bcls_rec.Tables["TMSM31"].Rows.get_Count();
			Log::Trace(" ", "f_tmsm_mm", "tmsm31_rows[{0}]" ,tmsm31_rows);

			for(i = 0; i < tmsm31_rows ; i++)
			{
				ttmsm31.Reset();
				ttmsm31.SM_UNIT_NO	= sm_unit_no;
				// 下面几个就是rows[0] 别瞎改啊
				ttmsm31.CC_MACH_NO	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["CC_NO"].ToString().Trim();			
				ttmsm31.ON_LINE_TIME= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_OPEN_TIME"].ToString().Trim();
				ttmsm31.OFF_LINE_TIME	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_CLOSE_TIME"].ToString().Trim();
				ttmsm31.ST_NO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["ST_NO"].ToString().Trim();
				//ttmsm31.HEAT_NO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["HEAT_NO"].ToString().Trim();
				//ttmsm31.PONO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["PONO"].ToString().Trim();
				ttmsm31.CAST_NO		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["CAST_NO"].ToString().Trim();
				//ttmsm31.CAST_DIV_NO	= (CDecimal)bcls_rec->Tables["TMSM99"].Rows[0]["CAST_DIV_NO"];
				// 下面是i
				ttmsm31.MOLD_NO		= (CString)tmbw_bcls_rec.Tables["TMSM31"].Rows[i]["MOLD_NO"].ToString().Trim();
				ttmsm31.MOLD_NUM	= (CDecimal)tmbw_bcls_rec.Tables["TMSM31"].Rows[i]["MOLD_NUM"];
				
				Log::Trace(" ", "f_tmsm_mm", "ttmsm31.CC_MACH_NO[{0}]"	,(const char*)ttmsm31.CC_MACH_NO);
				Log::Trace(" ", "f_tmsm_mm", "ttmsm31.MOLD_NO[{0}]"		,(const char*)ttmsm31.MOLD_NO);
				Log::Trace(" ", "f_tmsm_mm", "ttmsm31.MOLD_NUM[{0}]"	,ttmsm31.MOLD_NUM);
		 		Log::Trace(" ", "f_tmsm_mm", "ttmsm31.CC_MACH_NO[{0}]"		,(const char*)ttmsm31.CC_MACH_NO);
		 		Log::Trace(" ", "f_tmsm_mm", "ttmsm31.ON_LINE_TIME[{0}]"	,(const char*)ttmsm31.ON_LINE_TIME);
		 		Log::Trace(" ", "f_tmsm_mm", "ttmsm31.OFF_LINE_TIME[{0}]"	,(const char*)ttmsm31.OFF_LINE_TIME);
				Log::Trace(" ", "f_tmsm_mm", "ttmsm31.ST_NO[{0}]"			,(const char*)ttmsm31.ST_NO);
				//Log::Trace(" ", "f_tmsm_mm", "ttmsm31.HEAT_NO[{0}]"			,(const char*)ttmsm31.HEAT_NO);
				//Log::Trace(" ", "f_tmsm_mm", "ttmsm31.PONO[{0}]"			,(const char*)ttmsm31.PONO);
				Log::Trace(" ", "f_tmsm_mm", "ttmsm31.CAST_NO[{0}]"			,(const char*)ttmsm31.CAST_NO);
		 		//Log::Trace(" ", "f_tmsm_mm", "ttmsm31.CAST_DIV_NO[{0}]"		,(const char*)ttmsm31.CAST_DIV_NO);

	 			sqlstr = " SELECT COUNT(1) FROM TTMSM31 "
					" WHERE MOLD_NO = @ttmsm31.MOLD_NO "
					" AND SM_UNIT_NO = @ttmsm31.SM_UNIT_NO "
					;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm31.MOLD_NO"	, ttmsm31.MOLD_NO );
				cmd_inq.Parameters.Set( "ttmsm31.SM_UNIT_NO", ttmsm31.SM_UNIT_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if(cmd_inq.Read())
				{
					count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
				}
				cmd_inq.Close();
				Log::Trace("","f_tmsm_mm"," ttmsm31 count [{0}]",count );
				
				if	(count == 0)
				{
					Log::Trace(" ", "f_tmsm_mm", "*******结晶器号不存在*******");
					//sprintf	(s.msg , "传入的钢包号不存在") ;
					//throw	CApplicationException(-1 , s.msg , "f_tmsm_mm");
				}
				else // 修改TTMSM31表中该结晶器
				{
					ttmsm31.REC_REVISOR = s.userid;   //记录修改责任者
					ttmsm31.REC_REVISE_TIME =datetimeNow; //记录修改时刻

					// 生成使用流水号REPAIR_SEQ_NO
					sqlstr1 =	" SELECT nvl(MAX(USE_SEQ_NO),0) "
								" FROM TTMSM39 "
								" WHERE SM_UNIT_NO	= @ttmsm39.SM_UNIT_NO "
								" AND MOLD_NO 		= @ttmsm39.MOLD_NO "
								;
					cmd_inq1.SetCommandText( sqlstr1 );
					cmd_inq1.Parameters.Set("ttmsm39.SM_UNIT_NO", ttmsm31.SM_UNIT_NO);
					cmd_inq1.Parameters.Set("ttmsm39.MOLD_NO"	, ttmsm31.MOLD_NO);
					cmd_inq1.ExecuteReader();
					if ( cmd_inq1.Read() )
					{
						ttmsm31.USE_SEQ_NO = cmd_inq1.GetInt32(1)+1;
					}
					cmd_inq1.Close();
					Log::Trace("","f_tmsm_mm","ttmsm31.USE_SEQ_NO [{0}]",ttmsm31.USE_SEQ_NO);

					ttmsm31.Update(" REC_REVISOR "
						" , REC_REVISE_TIME "
						" , USE_SEQ_NO "
						" , ST_NO "
						//" , HEAT_NO "
						//" , PONO "
						" , CAST_NO "
						//" , CAST_DIV_NO "
						" , CC_MACH_NO "
						" , ON_LINE_TIME "
						" , OFF_LINE_TIME "
						" , LADLE_LIFE ",
						" SM_UNIT_NO, MOLD_NO ");

					/*新增中间罐使用实绩表*/
					ttmsm39.Reset();
					ttmsm39.CopyFrom(ttmsm01);
					ttmsm39.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
					if(ttmsm39.REC_CREATOR.Trim() == "")
					{
						ttmsm39.REC_CREATOR = "XCOM";		//记录创建责任者
					}
					ttmsm39.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
					ttmsm39.REC_REVISOR = " ";
					ttmsm39.REC_REVISE_TIME = " ";
					ttmsm39.TrimOrBlank();
					ttmsm39.Print();
					ttmsm39.Insert();
				}
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