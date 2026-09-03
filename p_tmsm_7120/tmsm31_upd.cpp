/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sjh
Version:    1.0
Date:     2013-11-14
Description: 结晶器基本信息管理_修改
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
#include "ttmsm31.h"
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
//   结晶器基本信息__修改
/// <para>
/// 1.根据输入参数以炼钢单元号为基础修改《结晶器基本信息表》中的相应信息。
/// <para>数据库表： ttmsm31	结晶器基本信息表 </para>
/// <para>数据库表： htmsm31	结晶器基本信息历史表 </para>
/// </summary>
/// <param name="SM_UNIT_NO">炼钢单元号               </param>
/// <returns>指定炼钢单元号下的修改命令信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm31_upd)

int f_tmsm31_upd(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义	
	
	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int i;
	int ret;
	char err_msg[200];
	int RowCount = 0;

	CString  sqlstr = "";

	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);

	CTTMSM31 ttmsm31(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		/* 对输入信息循环处理 */
		RowCount = bcls_rec->Tables[0].Rows.get_Count();
		for (i = 0; i < RowCount; i++ ) 
		{
			/* ***** 初始化全局变量 ***** */
			ttmsm31.Reset();
			/* 取得单行传入信息 */
			ttmsm31.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm31.TrimOrBlank();

			Log::Trace(" ", "tmsm31_upd", "ttmsm31.SM_UNIT_NO[{0}]"		,(const char*)ttmsm31.SM_UNIT_NO);
			Log::Trace(" ", "tmsm31_upd", "ttmsm31.MOLD_NO[{0}]"		,(const char*)ttmsm31.MOLD_NO);
			Log::Trace(" ", "tmsm31_upd", "ttmsm31.MOLD_SECTION[{0}]"	,(const char*)ttmsm31.MOLD_SECTION);
			Log::Trace(" ", "tmsm31_upd", "ttmsm31.CC_MACH_NO[{0}]"		,(const char*)ttmsm31.CC_MACH_NO);
		
			ttmsm31.REC_REVISE_TIME = datetimeNow;
			ttmsm31.REC_REVISOR = s.userid;
			ttmsm31.Update(" REC_REVISOR "
						" ,REC_REVISE_TIME "
						//" ,SM_UNIT_NO "		//炼钢单元号
						//" ,MOLD_NO "			//结晶器号
						//" ,MOLD_SECTION "		//结晶器断面
						" ,CURRENT_STATUS "		//当前状态
						//" ,CC_MACH_NO "		//连铸机号
						" ,STRAND_NO "			//流号
						" ,PROD_DATE "			//生产日期
						" ,START_USE_DATE "		//开始启用日期
						" ,USE_SEQ_NO "			//使用流水号
						" ,COPPER_CHANGE_DIV "  //更换铜管板标记
						" ,MD_COPPER_PLATE_NO " //铜管板号
						" ,COPPER_MANU "		//铜板/管厂家
						" ,COPPER_TIMES "		//铜板/管使用次数
						" ,ON_LINE_TIME "		//上线时间
						" ,OFF_LINE_TIME "		//下线时间
						" ,UNLADE_REASON "		//下线原因
						" ,CAST_NO "			//连连浇号(CAST号)
						" ,ST_NO "				//出钢记号
						" ,MOLD_NUM "			//结晶器使用炉数
						" ,TOTAL_CHARGE_NUM "   //累计总炉数
						" ,STD_CHARGE_NUM "		//标准使用炉数
						" ,CUR_CAST_WT "		//当前浇铸重量
						" ,CUR_CAST_LEN "		//当前浇铸长度
						" ,TOTAL_CAST_WT "		//总浇铸重量
						" ,TOTAL_CAST_LEN "		//总浇铸长度
						" ,USE_REMARK "			//使用备注
						" ,REPAIR_SEQ_NO "		//维修序号
						" ,MAINT_TYPE "			//维修类型
						" ,REPAIR_POS "			//维修位置
						" ,REPAIR_START_TIME "  //维修开始时刻
						" ,REPAIR_END_TIME "    //维修结束时刻
						" ,MAINTAIN_REMARK "    //维修备注
						" ,SCRAP_TIME "			//报废时刻
						" ,SCRAP_MAKER "		//报废责任者
						" ,SCRAP_CAUSE_CODE "   //报废原因代码
						" ,SCRAP_CAUSE_NAME "   //报废原因名称
						" ,REMARK ",			//备注
						" SM_UNIT_NO,MOLD_NO,MOLD_SECTION,CC_MACH_NO ");
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