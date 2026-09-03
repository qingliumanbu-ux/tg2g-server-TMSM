/*****************************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2011
Author:      T01966
Version:     3.0
Date:        2014-01-20 13:15:20
Description: TMSM99 铁包包龄计算公式查询
**************************************************/

// 框架头文件
#include "stdafx.h"

// 业务头文件
//#include "ttmsm99.h"

/*<remark>=========================================================
/// <summary>
/// 铁包维修实绩查询
/// <para>
/// 1.0 查询铁包包龄计算公式；
/// </para>
/// <para>数据库表：TTMSM99(铁包包龄计算公式表)；</para>
/// <para>主调用函数：前台TMSM11画面查询调用。</para>
/// </summary>
/// <param name=""></param>
/// <returns>铁包包龄计算公式信息</returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(tmsm99_inq)

int f_tmsm99_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*打印程序起止LOG*/
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */	
	int doFlag = 0;		//返回值

	/* 数据库操作类定义 */
	CDbCommand cmd(conn);

    /* 实体类定义 */
	//CTTMSM99 ttmsm99(conn);

	/* 业务变量 */
	//CString v_sm_unit_no	= "";		//炼钢单元号

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";

	try 
	{
	//	//接收从前台传入的值
	//	//建立查询数据SQL语句查询铁包包龄计算公式表
	//	sqlstr =	" SELECT * "
	//				" FROM   TTMSM99 "
	//				" WHERE  1=1 ";
	//	sqlstr += " ORDER BY MODEL_ID ASC ";

	//	//连接数据库
	//	//CDbCommand cmd(sqlstr, conn);
	//	cmd.SetCommandText( sqlstr );

	//	//传递参数进SQL语句
	//	//cmd.Parameters.Set("v_sm_unit_no"	, v_sm_unit_no);
	//
	//	//执行SQL语句
	//	int count=cmd.ExecuteQuery(bcls_ret->Tables[0]);

	//	bcls_ret->Tables[0].set_TableName("TTMSM99");
	//	cmd.Close();

	//	//strcpy(s.msg,_RES("处理成功。")/*处理成功。*/);
	//	strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);	
	//}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		//CMessageFormat::Format(s.msg,  _RES("读取数据失败,表[{0}],sqlcode=[{1}]。请联系系统维护人员."), arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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