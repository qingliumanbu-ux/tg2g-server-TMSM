/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2013-11-27
Description: 钢包配包计划获取
**************************************************/

/*<remark>=========================================================
/// <summary>
/// 钢包配包计划获取
/// <para>
/// 0.根据传入的
熔炼号
制造命令号
设备号(详细请见代码手册)
运转信号
事件发生时刻
钢包号
设备代码
/// 具体定义可以参看PSSMS1 PSSMD1两个画面
/// 根据运转信号，判断是否要插表TTMSM02
/// 根据运转信号，判断是否要更新TTMSM01
/// 根据运转信号，判断是否要新增或更新TTMSM09
/// </para>
/// <para>数据库表：ttmsm01(钢包基本信息表)		</para>
/// <para>数据库表：ttmsm02(钢包配包计划表)		</para>
/// <para>数据库表：ttmsm09(钢包使用信息表)		</para>
/// <para>主调用函数：电文 运转信号 调用。  </para>
/// </summary>
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
#include "ttmsm02.h"
#include "ttmsm09.h"

//名称空间引用
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm_plan(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);

	// 程序内部变量
	int doFlag = 0;
	int count = 0;

	// 实体类定义
	CTTMSM01 ttmsm01(conn);
	CTTMSM02 ttmsm02(conn);
	CTTMSM09 ttmsm09(conn);

	// 数据库SQL操作字符串
	CString sqlstr = "";

	// 数据库操作类定义
	CDbCommand cmd_inq(conn);

	// 业务变量
	CString heat_no = "";
	CString pono = "";
	//CString equ_no = "";
	CString run_signal = "";
	CString start_time_event = "";
	CString ladle_no = "";
	CString sm_plan_no = "";
	CString sm_unit_no = "";
	CString v_sys = "";
	CString dev_code = "";
	CString rs_1 = ""; // run_signal第一位
	CString rs_2 = ""; // run_signal第二位
	CString rs_3 = ""; // run_signal第三位

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString delTime = CDateTime::Now().AddDays(-90).ToString("yyyyMMddHHmmss");

	try
	{
		#if defined(_P1)
			sm_unit_no = "L1";
			v_sys = "P1";
		#endif

		#if defined(_P2)
			sm_unit_no = "L2";
			v_sys = "P2";
		#endif

		#if defined(_P3)
			sm_unit_no = "L1";
			v_sys = "P3";
		#endif

		if(sm_unit_no.Trim() == "")
			sm_unit_no = "L1";

		if(v_sys.Trim() == "")
			v_sys = "P1";
		Log::Trace(" ", "f_tmsm_plan", "*****v_sys[{0}]"	,(const char*)v_sys);

		//// 转炉装料开始
		//if( rs_1 == "3" &&  rs_3 == "1")
		//// 转炉装料结束
		//else if( rs_1 == "3" &&  rs_3 == "2")
		//// 转炉吹炼开始
		//else if( rs_1 == "3" &&  rs_3 == "3")
		//// 转炉吹炼结束
		//else if( rs_1 == "3" &&  rs_3 == "4")
		//// 转炉出钢开始
		//else if( rs_1 == "3" &&  rs_3 == "5")
		//// 转炉出钢结束
		//else if( rs_1 == "3" &&  rs_3 == "6")
		//// LF包到
		//else if( rs_1 == "4" &&  rs_3 == "1")
		//// LF精炼开始
		//else if( rs_1 == "4" &&  rs_3 == "2")
		//// LF精炼结束
		//else if( rs_1 == "4" &&  rs_3 == "3")
		//// LF精炼包离开
		//else if( rs_1 == "4" &&  rs_3 == "4")
		//// 连铸机包到
		//else if( rs_1 == "5" &&  rs_3 == "1")
		//// 连铸机开浇
		//else if( rs_1 == "5" &&  rs_3 == "2")
		//// 连铸机浇铸完毕
		//else if( rs_1 == "5" &&  rs_3 == "3")
		//// 连铸机切断完毕
		//else if( rs_1 == "5" &&  rs_3 == "4")

		// 获取输入参数		
		heat_no		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["HEAT_NO"].ToString().Trim();
		pono		= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["PONO"].ToString().Trim();
		//equ_no = (CString)bcls_rec->Tables["TMSM99"].Rows[0]["EQU_NO"].ToString().Trim();
		run_signal	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["RUN_SIGNAL"].ToString().Trim();
		start_time_event = (CString)bcls_rec->Tables["TMSM99"].Rows[0]["START_TIME_EVENT"].ToString().Trim();
		ladle_no	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["LADLE_NO"].ToString().Trim();
		sm_plan_no	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["SM_PLAN_NO"].ToString().Trim();
		dev_code	= (CString)bcls_rec->Tables["TMSM99"].Rows[0]["DEV_CODE"].ToString().Trim();

		Log::Trace(" ", "f_tmsm_plan", "*****sm_unit_no[{0}]"	,(const char*)sm_unit_no);
		Log::Trace(" ", "f_tmsm_plan", "run_signal[{0}]"	,(const char*)run_signal);
		Log::Trace(" ", "f_tmsm_plan", "heat_no[{0}]"		,(const char*)heat_no);
		Log::Trace(" ", "f_tmsm_plan", "pono[{0}]"			,(const char*)pono);		
		Log::Trace(" ", "f_tmsm_plan", "start_time_event[{0}]"	,(const char*)start_time_event);
		Log::Trace(" ", "f_tmsm_plan", "ladle_no[{0}]"		,(const char*)ladle_no);
		Log::Trace(" ", "f_tmsm_plan", "sm_plan_no[{0}]"	,(const char*)sm_plan_no);
		Log::Trace(" ", "f_tmsm_plan", "dev_code[{0}]"		,(const char*)dev_code);

		// 收到运转信号或配包信号
		// 1. 根据炼钢计划号查找TTMSM02表 无增 有改
		// 2. 根据钢包号 修改TTMSM01表的 炼钢计划号、熔炼号、pono号、设备号、设备代码（钢包位置）
		// 3. 清理一下TTMSM02之前的记录，保留90天
		// 4. 根据不同的运转信号 对钢包属性做操作
		if (sm_plan_no.Trim() == "")
		{
			sprintf	(s.msg , "传入的炼钢计划号不能为空") ;
			throw	CApplicationException(-1 , s.msg , "f_tmsm_plan");
		}
		if (ladle_no.Trim() == "")
		{
			sprintf	(s.msg , "传入的钢包号不能为空") ;
			throw	CApplicationException(-1 , s.msg , "f_tmsm_plan");
		}

		Log::Trace(" ", "f_tmsm_plan", " 1.根据炼钢计划号查找TTMSM02表 无增 有改 begin");
		sqlstr = " SELECT COUNT(1) FROM TTMSM02 "
				" WHERE LADLE_NO = @ladle_no "
				" AND SM_PLAN_NO = @sm_plan_no "
				;
		cmd_inq.SetCommandText( sqlstr );
		cmd_inq.Parameters.Set( "ladle_no"	, ladle_no );
		cmd_inq.Parameters.Set( "sm_plan_no", sm_plan_no );
		cmd_inq.ExecuteReader();
		count = 0 ;
		if(cmd_inq.Read())
		{
			count = cmd_inq.GetInt16(1); //将数据获取到实体对象中				
		}
		cmd_inq.Close();
		Log::Trace("","f_tmsm_plan"," TTMSM02 count [{0}]",count );
		
		ttmsm02.Reset();
		if	(count == 0)
		{
			Log::Trace("","f_tmsm_plan"," 没有就新增一条计划 " );
			
			ttmsm02.REC_CREATOR = s.userid;		//记录创建责任者
			if(ttmsm02.REC_CREATOR.Trim() == "")
			{
				ttmsm02.REC_CREATOR = "XCOM";		//记录创建责任者
			}
			ttmsm02.REC_CREATE_TIME =datetimeNow;	//记录创建时刻
			ttmsm02.REC_REVISOR = " ";
			ttmsm02.REC_REVISE_TIME = " ";
			ttmsm02.SM_UNIT_NO = sm_unit_no;	//炼钢单元号
			ttmsm02.LADLE_NO = ladle_no;		//钢包号
			ttmsm02.SM_PLAN_NO = sm_plan_no;	//炼钢计划号
			ttmsm02.HEAT_NO = heat_no;			//熔炼号
			ttmsm02.PONO = pono;				//制造命令号
			ttmsm02.DEV_CODE = dev_code;		//设备代码
			ttmsm02.REMARK = start_time_event;   //备注
			//ttmsm02.Print();
			ttmsm02.TrimOrBlank();
			ttmsm02.Insert();
			//Log::Trace("","f_tmsm_plan"," after ttmsm02.Insert() " );
		}
		else
		{
			Log::Trace("","f_tmsm_plan"," 有就修改计划 " );
			ttmsm02.REC_REVISOR = s.userid;		//记录修改责任者
			if(ttmsm02.REC_REVISOR.Trim() == "")
			{
				ttmsm02.REC_REVISOR = "XCOM";		//记录创建责任者
			}
			ttmsm02.REC_REVISE_TIME =datetimeNow; //记录修改时刻
			ttmsm02.SM_UNIT_NO = sm_unit_no;   //炼钢单元号
			ttmsm02.LADLE_NO = ladle_no;		//钢包号
			ttmsm02.SM_PLAN_NO = sm_plan_no;	//炼钢计划号
			ttmsm02.HEAT_NO = heat_no;			//熔炼号
			ttmsm02.PONO = pono;				//制造命令号			
			ttmsm02.DEV_CODE = dev_code;		//设备代码
			ttmsm02.REMARK = start_time_event;			
			ttmsm02.Update(" REC_REVISOR "
						" , REC_REVISE_TIME "
						" , HEAT_NO "
						" , PONO "
						" , DEV_CODE "
						" , REMARK " ,							
						" SM_UNIT_NO, LADLE_NO, SM_PLAN_NO ");
		}
		Log::Trace(" ", "f_tmsm_plan", " 1. 根据炼钢计划号查找TTMSM02表 无增 有改 over");
		
		Log::Trace(" ", "f_tmsm_plan", " 2. 根据钢包号 修改TTMSM01表的 炼钢计划号、熔炼号、pono号、设备号、设备代码（钢包位置）begin");
		ttmsm01.Reset();
		ttmsm01.CopyFrom(ttmsm02);		
		ttmsm01.SM_UNIT_NO	= sm_unit_no;//炼钢单元号
		ttmsm01.LADLE_NO	= ladle_no;	//钢包号
		ttmsm01.PONO		= pono;
		ttmsm01.HEAT_NO		= heat_no;
		ttmsm01.SM_PLAN_NO	= sm_plan_no;
		ttmsm01.LD_POS		= dev_code;
		if(dev_code.Trim() == "A")
		{
			ttmsm01.BOF_NO = "1";
		}
		else if(dev_code.Trim() == "B")
		{
			ttmsm01.BOF_NO = "2";
		}
		else if(dev_code.Trim() == "C")
		{
			ttmsm01.BOF_NO = "3";
		}
		else if(dev_code.Trim() == "D")
		{
			ttmsm01.BOF_NO = "4";
		}
		else if(dev_code.Trim() == "E")
		{
			ttmsm01.BOF_NO = "5";
		}
		else if(dev_code.Trim() == "F")
		{
			ttmsm01.BOF_NO = "6";
		}
		else
		{
			ttmsm01.BOF_NO = " ";
		}
		Log::Trace(" ", "f_tmsm_plan", "ttmsm01.SM_UNIT_NO[{0}]",(const char*)ttmsm01.SM_UNIT_NO);
		Log::Trace(" ", "f_tmsm_plan", "ttmsm01.LADLE_NO[{0}]"	,(const char*)ttmsm01.LADLE_NO);
		Log::Trace(" ", "f_tmsm_plan", "ttmsm01.LD_POS[{0}]"	,(const char*)ttmsm01.LD_POS);
		Log::Trace(" ", "f_tmsm_plan", "ttmsm01.BOF_NO[{0}]"	,(const char*)ttmsm01.BOF_NO);
		ttmsm01.Update(" SM_PLAN_NO,HEAT_NO,PONO,LD_POS, BOF_NO "," SM_UNIT_NO, LADLE_NO ");
		Log::Trace(" ", "f_tmsm_plan", " 2. 根据钢包号 修改TTMSM01表的 炼钢计划号、熔炼号、pono号、设备号、设备代码（钢包位置）over");

		Log::Trace(" ", "f_tmsm_plan", " 3. 清理一下TTMSM02之前的记录，保留90天 begin");
		// 20140121 清理一下TTMSM02之前的记录，保留90天的吧 
		Log::Trace(" ", "f_tmsm_plan", "delTime[{0}]",(const char*)delTime);		
		sqlstr = " DELETE TTMSM02 "
				" WHERE REC_CREATE_TIME <= @delTime "
				;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("delTime",delTime);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		Log::Trace(" ", "f_tmsm_plan", " 3. 清理一下TTMSM02之前的记录，保留90天 over");

		rs_1 = "";
		rs_2 = "";
		rs_3 = "";
		if(run_signal.Trim() != "000")
		{
			rs_1 = run_signal.Trim().Substring(0,1);
			rs_2 = run_signal.Trim().Substring(1,1);
			rs_3 = run_signal.Trim().Substring(2,1);
			Log::Trace(" ", "f_tmsm_plan", "*****run_signal[{0}]"	,(const char*)run_signal);
			Log::Trace(" ", "f_tmsm_plan", "*****rs_1[{0}]"	,(const char*)rs_1);
			Log::Trace(" ", "f_tmsm_plan", "*****rs_2[{0}]"	,(const char*)rs_2);
			Log::Trace(" ", "f_tmsm_plan", "*****rs_3[{0}]"	,(const char*)rs_3);
		}
		
		Log::Trace(" ", "f_tmsm_plan", " 4. 根据不同的运转信号 对钢包属性做操作 begin");
		// P1和P3的配包计划 用信号000表示
		if(run_signal.Trim() == "000" ) // || sm_unit_no.Trim() == "L2"
		{
			Log::Trace(" ", "f_tmsm_plan", "配包 以上 over");
		}
		// 转炉开始
		else if( rs_1 == "3" &&  rs_3 == "1")
		{
			Log::Trace(" ", "f_tmsm_plan", "转炉开始  暂略");
		}
		// 转炉装料结束
		else if( rs_1 == "3" &&  rs_3 == "2")
		{
			Log::Trace(" ", "f_tmsm_plan", "转炉装料结束 暂略");
		}
		// 转炉吹炼开始
		else if( rs_1 == "3" &&  rs_3 == "3")
		{
			Log::Trace(" ", "f_tmsm_plan", "转炉吹炼开始 暂略");
		}
		// 转炉吹炼结束
		else if( rs_1 == "3" &&  rs_3 == "4")
		{
			Log::Trace(" ", "f_tmsm_plan", "转炉吹炼结束 暂略");
		}
		// 转炉出钢开始
		else if( rs_1 == "3" &&  rs_3 == "5")
		{
			Log::Trace(" ", "f_tmsm_plan", "转炉出钢开始");
			// 钢包状态变为使用 使用开始时间
			ttmsm01.LADLE_STATUS = "21";
			if(start_time_event.Trim() != "")
			{
				ttmsm01.USAGE_ST = start_time_event;
			}
			else
			{
				ttmsm01.USAGE_ST = datetimeNow;
			}
			ttmsm01.USAGE_ET = " ";
			Log::Trace(" ", "f_tmsm_plan", "ttmsm01.SM_UNIT_NO[{0}]",(const char*)ttmsm01.SM_UNIT_NO);
			Log::Trace(" ", "f_tmsm_plan", "ttmsm01.LADLE_NO[{0}]"	,(const char*)ttmsm01.LADLE_NO);
			Log::Trace(" ", "f_tmsm_plan", "ttmsm01.LADLE_STATUS[{0}]"	,(const char*)ttmsm01.LADLE_STATUS);
			Log::Trace(" ", "f_tmsm_plan", "ttmsm01.USAGE_ST[{0}]"	,(const char*)ttmsm01.USAGE_ST);
			ttmsm01.Update(" LADLE_STATUS,USAGE_ST,USAGE_ET "," SM_UNIT_NO, LADLE_NO ");
		}
		// 转炉出钢结束
		else if( rs_1 == "3" &&  rs_3 == "6")
		{
			Log::Trace(" ", "f_tmsm_plan", "转炉出钢结束");
			// 钢包包龄+1
			ttmsm01.LADLE_LIFE = 0;
			sqlstr = " SELECT LADLE_LIFE FROM TTMSM01 "
					" WHERE LADLE_NO	= @ttmsm01.LADLE_NO "
					" AND SM_UNIT_NO	= @ttmsm01.SM_UNIT_NO "
					;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm01.LADLE_NO"		, ttmsm01.LADLE_NO );
			cmd_inq.Parameters.Set( "ttmsm01.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO );
			cmd_inq.ExecuteReader();
			
			if(cmd_inq.Read())
			{
				ttmsm01.LADLE_LIFE = cmd_inq.GetInt16(1) + 1; //将数据获取到实体对象中				
			}
			cmd_inq.Close();
			Log::Trace(" ", "f_tmsm_plan", "ttmsm01.LADLE_LIFE [{0}]",ttmsm01.LADLE_LIFE );
			Log::Trace(" ", "f_tmsm_plan", "ttmsm01.SM_UNIT_NO[{0}]",(const char*)ttmsm01.SM_UNIT_NO);
			Log::Trace(" ", "f_tmsm_plan", "ttmsm01.LADLE_NO[{0}]"	,(const char*)ttmsm01.LADLE_NO);
			ttmsm01.Update(" LADLE_LIFE "," SM_UNIT_NO, LADLE_NO ");
		}
		// LF包到
		else if( rs_1 == "4" &&  rs_3 == "1")
		{
			Log::Trace(" ", "f_tmsm_plan", "LF包到 暂略");
		}
		// LF精炼开始
		else if( rs_1 == "4" &&  rs_3 == "2")
		{
			Log::Trace(" ", "f_tmsm_plan", "LF精炼开始 暂略");
		}
		// LF精炼结束
		else if( rs_1 == "4" &&  rs_3 == "3")
		{
			Log::Trace(" ", "f_tmsm_plan", "LF精炼结束 暂略");
		}
		// LF精炼包离开
		else if( rs_1 == "4" &&  rs_3 == "4")
		{
			Log::Trace(" ", "f_tmsm_plan", "LF精炼包离开 暂略");
		}
		// 连铸机包到
		else if( rs_1 == "5" &&  rs_3 == "1")
		{
			Log::Trace(" ", "f_tmsm_plan", "连铸机包到 暂略");
		}
		// 连铸机开浇
		else if( rs_1 == "5" &&  rs_3 == "2")
		{
			Log::Trace(" ", "f_tmsm_plan", "连铸机开浇 暂略");			
		}
		// 连铸机钢包浇铸完毕
		else if( rs_1 == "5" &&  rs_3 == "3")
		{
			Log::Trace(" ", "f_tmsm_plan", "连铸机浇铸完毕");
			// 20140322 原来写的是else的部分 造成如果信号多次上就新增好多使用实绩
			// （比方因为二级问题信号发错了 L3取消了重新做什么的
			// 先取了ttmsm01表 取包号、开始时间，看看ttmsm09表中是否已经有履历了
			// 如果有了就修改ttmsm01和ttmsm09原有的那条实绩
			// 如果没有就新增，按照原来写的就行了
			sqlstr = " SELECT USAGE_ST FROM TTMSM01 "
					" WHERE LADLE_NO	= @ttmsm01.LADLE_NO "
					" AND SM_UNIT_NO	= @ttmsm01.SM_UNIT_NO "
					;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm01.LADLE_NO"		, ttmsm01.LADLE_NO );
			cmd_inq.Parameters.Set( "ttmsm01.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO );
			cmd_inq.ExecuteReader();			
			if(cmd_inq.Read())
			{
				ttmsm01.USAGE_ST = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			Log::Trace(" ", "f_tmsm_plan", "ttmsm01.USAGE_ST[{0}]"	,(const char*)ttmsm01.USAGE_ST);

			sqlstr = " SELECT COUNT(1) FROM TTMSM09 "
					" WHERE LADLE_NO	= @ttmsm01.LADLE_NO "
					" AND SM_UNIT_NO	= @ttmsm01.SM_UNIT_NO "
					" AND USAGE_ST		= @ttmsm01.USAGE_ST "
					;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm01.LADLE_NO"		, ttmsm01.LADLE_NO );
			cmd_inq.Parameters.Set( "ttmsm01.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO );
			cmd_inq.Parameters.Set( "ttmsm01.USAGE_ST"		, ttmsm01.USAGE_ST );
			cmd_inq.ExecuteReader();
			count = 0;
			if(cmd_inq.Read())
			{
				count = cmd_inq.GetInt16(1);
			}
			cmd_inq.Close();
			Log::Trace(" ", "f_tmsm_plan", "09表中是否已有实绩 count[{0}]"	,count);
			
			if(count > 0)
			{
				Log::Trace(" ", "f_tmsm_plan", "09表中已有实绩 修改实绩");
				// 修改TTMSM09就好了
				if(start_time_event.Trim() != "")
				{
					ttmsm09.USAGE_ET = start_time_event;
				}
				else
				{
					ttmsm09.USAGE_ET = datetimeNow;
				}
				ttmsm09.SM_UNIT_NO	= sm_unit_no;//炼钢单元号
				ttmsm09.LADLE_NO	= ladle_no;	//钢包号
				ttmsm09.PONO		= pono;
				ttmsm09.HEAT_NO		= heat_no;
				ttmsm09.SM_PLAN_NO	= sm_plan_no;
				ttmsm09.USAGE_ST	= ttmsm01.USAGE_ST;
				ttmsm09.Update(" PONO,HEAT_NO,SM_PLAN_NO,USAGE_ET "," SM_UNIT_NO,LADLE_NO,USAGE_ST ");
			}
			// 20140322 over
			else
			{
				Log::Trace(" ", "f_tmsm_plan", "09表中没有实绩 新增实绩");
				// 钢包状态改为可用
				ttmsm01.LADLE_STATUS = "01";
				if(start_time_event.Trim() != "")
				{
					ttmsm01.USAGE_ET = start_time_event;
				}
				else
				{
					ttmsm01.USAGE_ET = datetimeNow;
				}
				// 连铸结束的时候 滑板次数按1→2→3→1来计算 
				// 上水口按1→2→……→20(C0)23(A0)来计算 
				// 渣线同包龄
				// 钢包包龄已经在出钢的时候+1过了
				ttmsm01.SLIP_BOARD_USE_TIMES = 0;	//滑板次数
				ttmsm01.SEN_TIMES_UPPER = 0;		//上水口使用次数
				ttmsm01.NOZZLE_BRICK_TIMES = 0;		//水口座砖使用次数
				ttmsm01.SLAGLINE_TIMES = 0;			//渣线次数
				ttmsm01.NOZZLE_SWITCH_TIMES = 0;	//水口开关使用次数
				ttmsm01.SHELL_USE_TIMES= 0;			//外壳使用次数（机构使用次数）
				ttmsm01.LADLE_BRICK_TIMES = 0;		//透气砖使用次数

				sqlstr = " SELECT SLIP_BOARD_USE_TIMES, SEN_TIMES_UPPER,NOZZLE_BRICK_TIMES, "
						" SLAGLINE_TIMES, NOZZLE_SWITCH_TIMES,LADLE_LIFE, "
						" SHELL_USE_TIMES, LADLE_BRICK_TIMES"
						" FROM TTMSM01 "
						" WHERE LADLE_NO	= @ttmsm01.LADLE_NO "
						" AND SM_UNIT_NO	= @ttmsm01.SM_UNIT_NO "
						;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm01.LADLE_NO"		, ttmsm01.LADLE_NO );
				cmd_inq.Parameters.Set( "ttmsm01.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO );
				cmd_inq.ExecuteReader();
				
				if(cmd_inq.Read())
				{
					ttmsm01.SLIP_BOARD_USE_TIMES = cmd_inq.GetInt16(1) + 1; //滑板次数
					ttmsm01.SEN_TIMES_UPPER = cmd_inq.GetInt16(2) + 1;		//上水口使用次数
					ttmsm01.NOZZLE_BRICK_TIMES = cmd_inq.GetInt16(3) + 1;	//水口座砖使用次数
					ttmsm01.SLAGLINE_TIMES = cmd_inq.GetInt16(4) + 1;		//渣线次数
					ttmsm01.NOZZLE_SWITCH_TIMES = cmd_inq.GetInt16(5) + 1;	//水口开关使用次数
					ttmsm01.LADLE_LIFE = cmd_inq.GetInt16(6);				//包龄
					ttmsm01.SHELL_USE_TIMES= cmd_inq.GetInt16(7) + 1;		//外壳使用次数（机构使用次数）
					ttmsm01.LADLE_BRICK_TIMES = cmd_inq.GetInt16(8) + 1;	//透气砖使用次数
				}
				cmd_inq.Close();
				Log::Trace(" ", "f_tmsm_plan", "ttmsm01.SLIP_BOARD_USE_TIMES[{0}]"	,ttmsm01.SLIP_BOARD_USE_TIMES);
				Log::Trace(" ", "f_tmsm_plan", "ttmsm01.SEN_TIMES_UPPER[{0}]"		,ttmsm01.SEN_TIMES_UPPER);
				Log::Trace(" ", "f_tmsm_plan", "ttmsm01.NOZZLE_BRICK_TIMES[{0}]"	,ttmsm01.NOZZLE_BRICK_TIMES);
				Log::Trace(" ", "f_tmsm_plan", "ttmsm01.SLAGLINE_TIMES[{0}]"		,ttmsm01.SLAGLINE_TIMES);
				Log::Trace(" ", "f_tmsm_plan", "ttmsm01.NOZZLE_SWITCH_TIMES[{0}]"	,ttmsm01.NOZZLE_SWITCH_TIMES);
				Log::Trace(" ", "f_tmsm_plan", "ttmsm01.LADLE_LIFE[{0}]"			,ttmsm01.LADLE_LIFE);
				Log::Trace(" ", "f_tmsm_plan", "ttmsm01.SHELL_USE_TIMES[{0}]"		,ttmsm01.SHELL_USE_TIMES);
				Log::Trace(" ", "f_tmsm_plan", "ttmsm01.LADLE_BRICK_TIMES[{0}]"		,ttmsm01.LADLE_BRICK_TIMES);

				// 滑板次数到3
				if( ttmsm01.SLIP_BOARD_USE_TIMES > 3)
				{
					ttmsm01.SLIP_BOARD_USE_TIMES = 1;
				}
				//上水口次数 按1→2→……→20(C0)23(A0)来计算
				if(v_sys.Trim() == "P1" && ttmsm01.SEN_TIMES_UPPER >20)
				{
					ttmsm01.SEN_TIMES_UPPER = 1;
				}
				else if(v_sys.Trim() == "P3" && ttmsm01.SEN_TIMES_UPPER >23)
				{
					ttmsm01.SEN_TIMES_UPPER = 1;
				}
				//else if()
				//{
				//}
				//渣线次数同包龄
				ttmsm01.SLAGLINE_TIMES = ttmsm01.LADLE_LIFE.ToInt16();
				Log::Trace(" ", "f_tmsm_plan", "*ttmsm01.SLIP_BOARD_USE_TIMES[{0}]",ttmsm01.SLIP_BOARD_USE_TIMES);
				Log::Trace(" ", "f_tmsm_plan", "*ttmsm01.SLAGLINE_TIMES[{0}]",ttmsm01.SLAGLINE_TIMES);
				Log::Trace(" ", "f_tmsm_plan", "*ttmsm01.SEN_TIMES_UPPER[{0}]",ttmsm01.SEN_TIMES_UPPER);
				Log::Trace(" ", "f_tmsm_plan", "ttmsm01.SM_UNIT_NO[{0}]",(const char*)ttmsm01.SM_UNIT_NO);
				Log::Trace(" ", "f_tmsm_plan", "ttmsm01.LADLE_NO[{0}]"	,(const char*)ttmsm01.LADLE_NO);
				Log::Trace(" ", "f_tmsm_plan", "ttmsm01.LADLE_STATUS[{0}]"	,(const char*)ttmsm01.LADLE_STATUS);
				Log::Trace(" ", "f_tmsm_plan", "ttmsm01.USAGE_ET[{0}]"	,(const char*)ttmsm01.USAGE_ET);

				// 20140322 查一下是否有使用开始时间的履历 如果有就修改 不要新增了
				// 原来写的就是else里的部分
				// 生成使用流水号REPAIR_SEQ_NO
				sqlstr =	" SELECT nvl(MAX(USE_SEQ_NO),0) "
							" FROM TTMSM09 "
							" WHERE SM_UNIT_NO	= @ttmsm09.SM_UNIT_NO "
							" AND LADLE_NO 		= @ttmsm09.LADLE_NO "
							;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set("ttmsm09.SM_UNIT_NO", ttmsm01.SM_UNIT_NO);
				cmd_inq.Parameters.Set("ttmsm09.LADLE_NO"	, ttmsm01.LADLE_NO);
				cmd_inq.ExecuteReader();
				if ( cmd_inq.Read() )
				{
					ttmsm01.USE_SEQ_NO = cmd_inq.GetInt32(1)+1;
				}
				cmd_inq.Close();
				Log::Trace("","f_tmsm_mm","ttmsm01.USE_SEQ_NO [{0}]",ttmsm01.USE_SEQ_NO);

				ttmsm01.REC_REVISOR = s.userid;   //记录修改责任者
				if(ttmsm01.REC_REVISOR.Trim() == "")
				{
					ttmsm01.REC_REVISOR = "XCOM";		//记录创建责任者
				}
				ttmsm01.REC_REVISE_TIME =datetimeNow; //记录修改时刻

				ttmsm01.Update(" REC_REVISOR "
								" , REC_REVISE_TIME "
								" , USE_SEQ_NO "
								" ,SLIP_BOARD_USE_TIMES " // 滑板次数
								" ,SEN_TIMES_UPPER "	// 上水口次数
								" ,SLAGLINE_TIMES "		// 渣线次数
								" ,NOZZLE_BRICK_TIMES " // 水口座砖使用次数--用户没提
								" ,NOZZLE_SWITCH_TIMES "// 水口开关使用次数--用户没提
								" ,SHELL_USE_TIMES "	// 外壳使用次数（机构使用次数）--用户没提
								" ,LADLE_BRICK_TIMES "	// 透气砖使用次数 --用户没提
								" ,LADLE_STATUS "		// 钢包状态
								" ,USAGE_ET ",		
								" SM_UNIT_NO, LADLE_NO ");
				/*新增钢包使用实绩表*/
				ttmsm09.Reset();
				sqlstr =	" SELECT * "
							" FROM TTMSM01 "
							" WHERE SM_UNIT_NO	= @ttmsm01.SM_UNIT_NO "
							" AND LADLE_NO 		= @ttmsm01.LADLE_NO "
							;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set("ttmsm01.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO);
				cmd_inq.Parameters.Set("ttmsm01.LADLE_NO"	, ttmsm01.LADLE_NO);
				cmd_inq.ExecuteReader();
				if ( cmd_inq.Read() )
				{
					cmd_inq.Fetch(ttmsm09);;
				}
				cmd_inq.Close();

				//班次班组 shift_no shift_group
				f_epep_get_shift_group("SM",ttmsm09.USAGE_ST,ttmsm09.SHIFT_NO,ttmsm09.GRP_NO,conn );
				ttmsm09.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
				if(ttmsm09.REC_CREATOR.Trim() == "")
				{
					ttmsm09.REC_CREATOR = "XCOM";		//记录创建责任者
				}
				ttmsm09.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
				ttmsm09.REC_REVISOR = " ";
				ttmsm09.REC_REVISE_TIME = " ";
				ttmsm09.TrimOrBlank();
				//ttmsm09.Print();
				ttmsm09.Insert();
			}
		}
		// 连铸机切断完毕
		else if( rs_1 == "5" &&  rs_3 == "4")
		{
			Log::Trace(" ", "f_tmsm_plan", "连铸机切断完毕 暂略");
		}
		Log::Trace(" ", "f_tmsm_plan", " 4. 根据不同的运转信号 对钢包属性做操作 over");

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