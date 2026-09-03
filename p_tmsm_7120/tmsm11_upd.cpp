/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sjh
Version:    1.0
Date:     2013-11-12
Description: 铁水罐基本信息管理_修改
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
#include "ttmsm11.h"
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
//   铁水罐基本信息__修改
/// <para>
/// 1.根据输入参数以炼钢单元号为基础修改《铁水罐基本信息表》中的相应信息。
/// <para>数据库表： ttmsm11	铁水罐基本信息表 </para>
/// <para>数据库表： htmsm11	铁水罐基本信息历史表 </para>
/// </summary>
/// <param name="SM_UNIT_NO">炼钢单元号               </param>
/// <returns>指定炼钢单元号下的修改命令信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm11_upd)

int f_tmsm11_upd(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
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

	CTTMSM11 ttmsm11(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		/* 对输入信息循环处理 */
		RowCount = bcls_rec->Tables[0].Rows.get_Count();
		for (i = 0; i < RowCount; i++ ) 
		{
			/* ***** 初始化全局变量 ***** */
			ttmsm11.Reset();
			/* 取得单行传入信息 */
			ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm11.TrimOrBlank();

			Log::Trace(" ", "tmsm11_upd", "ttmsm11.SM_UNIT_NO[{0}]"		,(const char*)ttmsm11.SM_UNIT_NO);
			Log::Trace(" ", "tmsm11_upd", "ttmsm11.IRON_LADLE_NO[{0}]"	,(const char*)ttmsm11.IRON_LADLE_NO);

			ttmsm11.REC_REVISE_TIME = datetimeNow;
			ttmsm11.REC_REVISOR = s.userid;
			ttmsm11.Update(" REC_REVISOR "
						" ,REC_REVISE_TIME "
						" ,EMPTY_LADLE_WT " //空包重量
						" ,MIT_STATUS "		//铁水罐状态
						" ,IRON_LADLE_POS " //铁水罐位置
						" ,LADLE_LIFE "		//包龄
						" ,LADLE_ERA "		//包役
						" ,WORK_MAKER "		//工作层(耐材厂家)
						" ,PROD_DATE "		//生产日期
						" ,START_USE_DATE "  //开始启用日期
						" ,REPAIR_SEQ_NO "   //维修序号
						" ,REPAIR_START_TIME "	//维修开始时刻
						" ,REPAIR_END_TIME "//维修结束时刻
						" ,MAINT_TYPE "		//维修类型
						" ,USE_SEQ_NO "		//使用流水号
						" ,USAGE_ST "		//使用开始时间
						" ,USAGE_ET "		//使用结束时间
						" ,REMARK "			//备注
						" ,MODEL_ID "		//模板号
						" ,IRON_LADLE_LIFE ", //铁水包包龄
						" SM_UNIT_NO,IRON_LADLE_NO ");
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