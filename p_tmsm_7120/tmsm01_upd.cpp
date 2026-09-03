/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sjh
Version:    1.0
Date:     2013-11-14
Description: 钢包基本信息管理_修改
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
#include "ttmsm01.h"
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
//   钢包基本信息__修改
/// <para>
/// 1.根据输入参数以炼钢单元号为基础修改《钢包基本信息表》中的相应信息。
/// <para>数据库表： ttmsm01	钢包基本信息表 </para>
/// <para>数据库表： htmsm01	钢包基本信息历史表 </para>
/// </summary>
/// <param name="SM_UNIT_NO">炼钢单元号               </param>
/// <returns>指定炼钢单元号下的修改命令信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm01_upd)

int f_tmsm01_upd(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义	
	
	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int i;
	int ret;
	char err_msg[200];
	int RowCount = 0;

	CString sqlstr = "";

	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);

	CTTMSM01 ttmsm01(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		/* 对输入信息循环处理 */
		RowCount = bcls_rec->Tables[0].Rows.get_Count();
		for (i = 0; i < RowCount; i++ ) 
		{
			/* ***** 初始化全局变量 ***** */
			ttmsm01.Reset();
			/* 取得单行传入信息 */
			ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm01.TrimOrBlank();

			Log::Trace(" ", "tmsm01_upd", "ttmsm01.SM_UNIT_NO[{0}]"		,(const char*)ttmsm01.SM_UNIT_NO);
			Log::Trace(" ", "tmsm01_upd", "ttmsm01.LADLE_NO[{0}]"	,(const char*)ttmsm01.LADLE_NO);

			if(ttmsm01.START_USE_DATE.Trim() != "")
			{
				ttmsm01.START_USE_DATE = ttmsm01.START_USE_DATE.Substring(0,8);
			}
			if(ttmsm01.BIG_REPAIR_DATE.Trim() != "")
			{
				ttmsm01.BIG_REPAIR_DATE = ttmsm01.BIG_REPAIR_DATE.Substring(0,8);
			}
			if(ttmsm01.MIDDLE_REPAIR_DATE.Trim() != "")
			{
				ttmsm01.MIDDLE_REPAIR_DATE = ttmsm01.MIDDLE_REPAIR_DATE.Substring(0,8);
			}
			if(ttmsm01.SMALL_REPAIR_DATE.Trim() != "")
			{
				ttmsm01.SMALL_REPAIR_DATE = ttmsm01.SMALL_REPAIR_DATE.Substring(0,8);
			}
			if(ttmsm01.FOREVER_REPAIR_DATE.Trim() != "")
			{
				ttmsm01.FOREVER_REPAIR_DATE = ttmsm01.FOREVER_REPAIR_DATE.Substring(0,8);
			}
			Log::Trace(" ", "tmsm01_upd", "ttmsm01.START_USE_DATE[{0}]"		,(const char*)ttmsm01.START_USE_DATE);
			Log::Trace(" ", "tmsm01_upd", "ttmsm01.BIG_REPAIR_DATE[{0}]"	,(const char*)ttmsm01.BIG_REPAIR_DATE);
			Log::Trace(" ", "tmsm01_upd", "ttmsm01.MIDDLE_REPAIR_DATE[{0}]"	,(const char*)ttmsm01.MIDDLE_REPAIR_DATE);
			Log::Trace(" ", "tmsm01_upd", "ttmsm01.SMALL_REPAIR_DATE[{0}]"	,(const char*)ttmsm01.SMALL_REPAIR_DATE);
			Log::Trace(" ", "tmsm01_upd", "ttmsm01.FOREVER_REPAIR_DATE[{0}]",(const char*)ttmsm01.FOREVER_REPAIR_DATE);

			ttmsm01.REC_REVISE_TIME = datetimeNow;
			ttmsm01.REC_REVISOR = s.userid;
			ttmsm01.Update(" REC_REVISOR "
						" ,REC_REVISE_TIME "
						" ,MANUFAC_NAME "   //厂家名称
						" ,LD_TYPE "				//钢包类型
						" ,LADLE_STATUS "			//钢包状态
						" ,LD_STATUS "				//钢包使用状况
						" ,REMARK "					//备注
						" ,START_USE_DATE "			//开始启用日期
						" ,EMPTY_LADLE_WT "			//空包重量
						" ,LD_POS "					//钢包位置
						" ,BAKE_SEQ_NO "			//烘烤流水号
						" ,BAKE_TYPE "				//烘烤类型
						" ,BAKE_POS "				//烘烤位置
						" ,DRYING_ST "				//烘烤开始时刻
						" ,DRYING_ET "				//烘烤结束时刻
						" ,BAKE_END_TEMP "			//烘烤结束温度
						" ,DRYING_REMARK "			//烘烤备注
						" ,REPAIR_SEQ_NO "			//维修序号
						" ,MAINT_TYPE "				//维修类型
						" ,REPAIR_START_TIME "		//维修开始时刻
						" ,REPAIR_END_TIME "		//维修结束时刻
						" ,BIG_REPAIR_NUM "			//大修次数
						" ,BIG_REPAIR_DATE "		//大修日期
						" ,BIG_REPAIR_USE_NUM "		//大修包龄
						" ,MIDDLE_REPAIR_NUM "		//中修次数
						" ,MIDDLE_REPAIR_DATE "		//中修日期
						" ,MIDDLE_REPAIR_USE_NUM "  //中修包龄
						" ,SMALL_REPAIR_DATE "		//小修日期
						" ,SMALL_REPAIR_NUM "		//小修次数
						" ,SMALL_REPAIR_USE_NUM "   //小修后使用次数
						" ,FOREVER_REPAIR_NUM "		//永久层修理次数
						" ,FOREVER_REPAIR_DATE "	//永久层修理日期
						" ,FOREVER_REPAIR_USE_NUM " //永久层修理后使用次数
						" ,FOREVER_FAY "			//永久层厂家
						" ,MAINTAIN_REMARK "		//维修备注
						" ,USE_SEQ_NO "				//使用流水号
						" ,USAGE_ST "				//使用开始时间
						" ,USAGE_ET "				//使用结束时间
						" ,SHELL_FAY "				//外壳厂家
						" ,SHELL_USE_TIMES "		//外壳使用次数
						" ,HEAT_NO "				//熔炼号
						" ,PROD_DATE "				//生产日期
						" ,SM_PLAN_NO "				//炼钢计划号
						" ,TPS_NO "					//出钢计划号
						" ,ST_NO "					//出钢记号
						" ,ACT_REFINE_ROUTE "		//实际精炼区分
						" ,PONO "					//制造命令号
						" ,LADLE_W_L "				//钢水重量
						" ,LOAD_STEEL_WT "			//装钢量
						" ,LOAD_STEEL_WT_TOTAL "	//装钢重量累计
						" ,SEN_TIMES_UPPER "		//上水口使用次数
						" ,NOZZLE_BRICK_MANU "		//水口座砖厂家
						" ,NOZZLE_BRICK_TIMES "		//水口座砖使用次数
						" ,LADLE_BRICK_TIMES "		//钢包透气砖使用次数
						" ,BREATH_FAY "				//透气砖厂家
						" ,BOF_NO "					//转炉号
						" ,NOZZLE_SWITCH_TIMES "	//水口开关使用次数
						" ,SRP_TIME_TOTAL "			//精炼时间累计
						" ,AR_BLOW_TIME_TOTAL "		//吹氩时间累计
						" ,RH_TIME_TOTAL "			//RH时间累计
						" ,SLIDE_MAKER "			//滑板厂家
						" ,SLIP_BOARD_USE_TIMES "   //滑板使用次数
						" ,SLAGLINE_TIMES "			//渣线次数
						" ,WORK_MAKER "				//工作层(耐材厂家)
						" ,LADLE_CURRENT_AREA "		//钢包当前位置
						" ,LADLE_ARRIVE_TIME "		//钢包到达时刻
						" ,GRP_NO "					//班别号
						" ,SHIFT_NO "				//班次号
						" ,USE_REMARK "				//使用备注
						" ,SCRAP_TIME "				//报废时刻
						" ,SCRAP_MAKER "			//报废责任者
						" ,SCRAP_CAUSE_CODE "		//报废原因代码
						" ,SCRAP_CAUSE_NAME "       //报废原因名称
						" ,SM_UNIT_NO,LADLE_NO ");
		}
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