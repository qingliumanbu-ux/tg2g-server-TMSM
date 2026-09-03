/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sjh
Version:    1.0
Date:     2013-11-13
Description: 中间罐基本信息_删除
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
#include "ttmsm21.h"
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
//   中间罐基本信息_删除
/// <para>
/// 1.根据输入参数以炼钢单元号、中间罐号为基础删除《中间罐基本信息表》中的相应信息。
/// <para>数据库表： ttmsm21	中间罐基本信息表 </para>
/// </summary>
/// <param name="td_no">中间罐号  </param>
/// <param name="SM_UNIT_NO">炼钢单元号               </param>
/// <returns>指定炼钢单元号下的删除命令信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm21_del)

int f_tmsm21_del(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
	
	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;
	int i;
	int RowCount = 0;

	CString  sqlstr = "";

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
			/* 取得单行传入信息 */
			ttmsm21.Reset();
			ttmsm21.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm21.TrimOrBlank();
			Log::Trace(" ", "tmsm21_del", "ttmsm21.SM_UNIT_NO[{0}]"		,(const char*)ttmsm21.SM_UNIT_NO);
			Log::Trace(" ", "tmsm21_del", "ttmsm21.TD_NO[{0}]"	,(const char*)ttmsm21.TD_NO);
			Log::Trace(" ", "tmsm21_del", "s.userid[{0}]"		,(const char*)s.userid);
			Log::Trace(" ", "tmsm21_del", "datetimeNow[{0}]"	,(const char*)datetimeNow);

			ttmsm21.Delete("SM_UNIT_NO,TD_NO");
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