/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-11
Description: 铁包信息详细信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 钢包信息详细信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明

BM2F_ENTERACE(ttmsm11a1a1_inq)

int f_ttmsm11a1a1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	

	/* 实体类定义 */ 
	CModel ttmsm11("TTMSM11");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		//datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//
		///* 获取输入参数 */
		//ttmsm11["IRON_LADLE_NO"] = bcls_rec->Tables[0].Rows[0]["IRON_LADLE_NO"].ToString().Trim();
	
		//Log::Trace("",__FUNCTION__,"ttmsm11.IRON_LADLE_NO		= [{0}]",(const char*)ttmsm11["IRON_LADLE_NO"].ToString());
		//
		///* 检查输入参数合法性 */
		//if(ttmsm11["IRON_LADLE_NO"].ToString().Trim() == "")
		//{
		//	strcpy(s.msg,"铁包号不能为空!");
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}	  

		///* 查询钢包信息表 */
		//ttmsm11["OP_DIV"] = "0";
		//ttmsm11.Query("OP_DIV,IRON_LADLE_NO");
	
		///* 设置返回块的值 */
		//ttmsm11.MergeTo(bcls_ret->Tables[0],false);

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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

