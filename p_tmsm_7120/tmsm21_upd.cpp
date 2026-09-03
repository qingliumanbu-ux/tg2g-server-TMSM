/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sjh
Version:    1.0
Date:     2013-11-12
Description: 中间罐基本信息管理_修改
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
#include "ttmsm21.h"
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
//   中间罐基本信息__修改
/// <para>
/// 1.根据输入参数以炼钢单元号为基础修改《中间罐基本信息表》中的相应信息。
/// <para>数据库表： ttmsm21	中间罐基本信息表 </para>
/// <para>数据库表： htmsm21	中间罐基本信息历史表 </para>
/// </summary>
/// <param name="SM_UNIT_NO">炼钢单元号               </param>
/// <returns>指定炼钢单元号下的修改命令信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm21_upd)

int f_tmsm21_upd(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
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

	CTTMSM21 ttmsm21(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		/* 对输入信息循环处理 */
		RowCount = bcls_rec->Tables[0].Rows.get_Count();
		for (i = 0; i < RowCount; i++ ) 
		{
			/* ***** 初始化全局变量 ***** */
			ttmsm21.Reset();
			/* 取得单行传入信息 */
			ttmsm21.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm21.TrimOrBlank();

			Log::Trace(" ", "tmsm21_upd", "ttmsm21.SM_UNIT_NO[{0}]"	,(const char*)ttmsm21.SM_UNIT_NO);
			Log::Trace(" ", "tmsm21_upd", "ttmsm21.TD_NO[{0}]"		,(const char*)ttmsm21.TD_NO);

			ttmsm21.REC_REVISE_TIME = datetimeNow;
			ttmsm21.REC_REVISOR = s.userid;
			ttmsm21.Update(" REC_REVISOR "
						" ,REC_REVISE_TIME "
						" ,MANUFAC_NAME "   //厂家名称
						" ,TD_STATUS "		//中间包状态
						" ,USE_SEQ_NO "		//使用流水号
						" ,LADLE_LIFE "		//包龄
						" ,TD_COVER_NO "	//中间包包盖号
						" ,BAKE_POS "		//烘烤位置
						" ,DRYING_ST "		//烘烤开始时刻
						" ,DRYING_ET "		//烘烤结束时刻
						" ,DRYING_REMARK "  //烘烤备注
						" ,PROD_DATE "		//生产日期
						" ,ONLINE_GROUP "   //上包班别
						" ,ONLINE_TIME "	//上包时间
						" ,ON_LINE_TIME "   //上线时间
						" ,OFF_LINE_TIME "  //下线时间
						" ,PONO "			//制造命令号
						" ,HEAT_NO "		//熔炼号
						" ,SM_PLAN_NO "		//炼钢计划号
						" ,CAST_NO "		//连连浇号(CAST号)
						" ,CAST_DIV_NO "	//CAST分割号
						" ,ST_NO "			//出钢记号
						" ,CAST_START_GROUP "   //浇铸开始班别
						" ,CAST_START_TIME "	//浇铸开始时刻
						" ,CAST_END_TIME "		//浇铸结束时刻
						" ,CAST_STOP_REASON "   //停浇原因
						" ,INSPECT_MAKER "		//检验人员
						" ,SEN_UPPER_MANU "		//上水口厂家
						" ,QUICK_CHANGE_MANU "  //快换机构厂家
						" ,WORK_MAKER "		//工作层(耐材厂家)
						" ,STAB_MANU "		//稳流器厂家
						" ,STRIKE_MANU "	//冲击板厂家
						" ,STOPPER_MANU "   //塞棒厂家
						" ,NOZZLE_BRICK_MANU "	//水口座砖厂家
						" ,SUBM_NOZZLE_MANU "   //浸入式水口厂家
						" ,SUBM_NOZZLE_TIME "   //浸入式水口使用寿命
						" ,LADLE_SHROUD_MANU "  //钢包保护套管厂家
						" ,LADLE_SHROUD_TIME "  //钢包保护套管使用寿命
						" ,DEV_REMARK_1 "   //设备备注1
						" ,USE_REMARK "		//使用备注
						" ,REPAIR_SEQ_NO "  //维修序号
						" ,MAINT_TYPE "		//维修类型
						" ,REPAIR_POS "		//维修位置
						" ,REPAIR_START_TIME "	//维修开始时刻
						" ,REPAIR_END_TIME "	//维修结束时刻
						" ,MAINTAIN_REMARK "	//维修备注
						" ,SCRAP_TIME "		//报废时刻
						" ,SCRAP_MAKER "	//报废责任者
						" ,SCRAP_CAUSE_CODE "   //报废原因代码
						" ,SCRAP_CAUSE_NAME "   //报废原因名称
						" ,REMARK ",		//备注
						" SM_UNIT_NO,TD_NO ");
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