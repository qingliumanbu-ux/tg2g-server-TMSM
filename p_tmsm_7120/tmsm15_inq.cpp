/*****************************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2011
Author:      T01966
Version:     3.0
Date:        2013-09-10 13:15:20
Description: TMSM15 铁水罐维修实绩查询
**************************************************/

// 框架头文件
#include "stdafx.h"

// 业务头文件
#include "ttmsm15.h"

/*<remark>=========================================================
/// <summary>
/// 铁水罐维修实绩查询
/// <para>
/// 1.0 根据查询条件，查询铁水罐维修实绩信息；
/// 2.0 查询条件：；
/// 3.0 排序方式：IRON_LADLE_NO ASC, REPAIR_START_TIME DESC；
/// </para>
/// <para>数据库表：TTMSM15(铁水罐维修实绩表)；</para>
/// <para>主调用函数：前台TMSM12画面查询调用。</para>
/// </summary>
/// <param name=""></param>
/// <returns>铁水罐信息</returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(tmsm15_inq)

int f_tmsm15_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*打印程序起止LOG*/
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */	
	int doFlag = 0;		//返回值

	/* 数据库操作类定义 */
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);

    /* 实体类定义 */
	CTTMSM15 ttmsm15(conn);

	/* 业务变量 */
	CString v_sm_unit_no	= "";		//炼钢单元号
	CString v_iron_ladle_no		= "";	//铁水罐号
	CString v_repair_time_from	= "";	//维修时间起
	CString v_repair_time_to	= "";	//维修时间止

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";

	try 
	{
		//接收从前台传入的值
		v_sm_unit_no		= (CString)bcls_rec->Tables[0].Rows[0]["SM_UNIT_NO"].ToString().Trim();		
		v_iron_ladle_no		= (CString)bcls_rec->Tables[0].Rows[0]["IRON_LADLE_NO"].ToString().Trim();
		v_repair_time_from	= (CString)bcls_rec->Tables[0].Rows[0]["REPAIR_TIME_FROM"].ToString().Trim();
		v_repair_time_to	= (CString)bcls_rec->Tables[0].Rows[0]["REPAIR_TIME_TO"].ToString().Trim();		

		Log::Trace(" ", "tmsm15_inq", "v_sm_unit_no[{0}]"		,(const char*)v_sm_unit_no);
		Log::Trace(" ", "tmsm15_inq", "v_iron_ladle_no[{0}]"	,(const char*)v_iron_ladle_no);
		Log::Trace(" ", "tmsm15_inq", "v_repair_time_from[{0}]"	,(const char*)v_repair_time_from);
		Log::Trace(" ", "tmsm15_inq", "v_repair_time_to[{0}]"	,(const char*)v_repair_time_to);
		
		if(v_repair_time_from.Trim().GetLength()>= 1 && v_repair_time_from.Trim().GetLength() <= 14)
		{
			v_repair_time_from = (v_repair_time_from + "00000000").Trim().Substring(0,14);
		}
		if(v_repair_time_to.Trim().GetLength()>= 1 && v_repair_time_to.Trim().GetLength() <= 14)
		{
			v_repair_time_to = (v_repair_time_to + "999999999").Trim().Substring(0,14);
		}
		Log::Trace(" ", "tmsm15_inq", "v_repair_time_from**[{0}]"	,(const char*)v_repair_time_from);
		Log::Trace(" ", "tmsm15_inq", "v_repair_time_to**[{0}]"	,(const char*)v_repair_time_to);
		
		//建立查询数据SQL语句查询铁水罐维修实绩表--分页
		int nStart  	=  (int)bcls_rec->Tables[1].Rows[0]["START"];
		int nPageSize	=  (int)bcls_rec->Tables[1].Rows[0]["PAGE_SIZE"];
		
		//建立查询数据SQL语句查询铁水罐维修实绩表
		sqlstr =	" SELECT COUNT(1) "
					" FROM   TTMSM15 "
					" WHERE  1=1 ";
		if(!v_sm_unit_no.Trim().IsEmpty())
		{
			sqlstr += " AND SM_UNIT_NO = @v_sm_unit_no ";
		}
		if(!v_iron_ladle_no.Trim().IsEmpty())
		{
			sqlstr += " AND IRON_LADLE_NO LIKE @v_iron_ladle_no ";
		}
		if(!v_repair_time_from.Trim().IsEmpty())
		{
			sqlstr += " AND REPAIR_START_TIME >= @v_repair_time_from ";
		}
		if(!v_repair_time_to.Trim().IsEmpty())
		{
			sqlstr += " AND REPAIR_END_TIME <= @v_repair_time_to ";
		}

		//连接数据库
		cmd1.SetCommandText( sqlstr );

		//传递参数进SQL语句
		cmd1.Parameters.Set("v_sm_unit_no"		, v_sm_unit_no);
		cmd1.Parameters.Set("v_iron_ladle_no"	, v_iron_ladle_no + "%");
		cmd1.Parameters.Set("v_repair_time_from", v_repair_time_from);
		cmd1.Parameters.Set("v_repair_time_to"	, v_repair_time_to);
		
		CDecimal rc = cmd1.ExecuteScalar();
		bcls_ret -> ExtendedProperties.Add("RC",rc.ToString());
		//分页over
		
		//建立查询数据SQL语句查询铁水罐维修实绩表
		sqlstr =	" SELECT * "
					" FROM   TTMSM15 "
					" WHERE  1=1 ";
		if(!v_sm_unit_no.Trim().IsEmpty())
		{
			sqlstr += " AND SM_UNIT_NO = @v_sm_unit_no ";
		}
		if(!v_iron_ladle_no.Trim().IsEmpty())
		{
			sqlstr += " AND IRON_LADLE_NO LIKE @v_iron_ladle_no ";
		}
		if(!v_repair_time_from.Trim().IsEmpty())
		{
			sqlstr += " AND REPAIR_START_TIME >= @v_repair_time_from ";
		}
		if(!v_repair_time_to.Trim().IsEmpty())
		{
			sqlstr += " AND REPAIR_END_TIME <= @v_repair_time_to ";
		}
		sqlstr += " ORDER BY REPAIR_START_TIME DESC, IRON_LADLE_NO ASC ";

		//连接数据库
		//CDbCommand cmd(sqlstr, conn);
		cmd.SetCommandText( sqlstr );

		//传递参数进SQL语句
		cmd.Parameters.Set("v_sm_unit_no"		, v_sm_unit_no);
		cmd.Parameters.Set("v_iron_ladle_no"	, v_iron_ladle_no + "%");
		cmd.Parameters.Set("v_repair_time_from"	, v_repair_time_from);
		cmd.Parameters.Set("v_repair_time_to"	, v_repair_time_to);
		
		//执行SQL语句
		int count=cmd.ExecuteQuery(bcls_ret->Tables[0],nStart,nPageSize);

		bcls_ret->Tables[0].set_TableName("TTMSM15");
		cmd.Close();

		//strcpy(s.msg,_RES("处理成功。")/*处理成功。*/);
		strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);	
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