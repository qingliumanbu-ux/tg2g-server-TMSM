/*****************************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2011
Author:      
Version:     3.0
Date:        2014-05-07
Description: TMSM04 钢包热修实绩查询
**************************************************/

// 框架头文件
#include "stdafx.h"

// 业务头文件
#include "ttmsm04.h"

/*<remark>=========================================================
/// <summary>
/// 钢包热修实绩查询
/// <para>
/// 1.0 根据查询条件，查询钢包热修实绩信息；
/// 2.0 查询条件：；
/// 3.0 排序方式：LADLE_NO ASC, REPAIR_START_TIME DESC；
/// </para>
/// <para>数据库表：TTMSM04(钢包热修实绩表)；</para>
/// <para>主调用函数：前台TMSM06画面查询调用。</para>
/// </summary>
/// <param name=""></param>
/// <returns>钢包信息</returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(tmsm06_inq)

int f_tmsm06_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*打印程序起止LOG*/
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */	
	int doFlag = 0;		//返回值

	/* 数据库操作类定义 */
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);

    /* 实体类定义 */
	CTTMSM04 ttmsm04(conn);

	/* 业务变量 */
	CString v_sm_unit_no	= "";	//炼钢单元号
	CString v_ladle_no		= "";	//钢包号
	CString v_repair_start_time	= "";	//使用时间起
	CString v_repair_end_time	= "";	//使用时间止

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";

	try 
	{
		//接收从前台传入的值
		v_sm_unit_no	= (CString)bcls_rec->Tables[0].Rows[0]["SM_UNIT_NO"].ToString().Trim();		
		v_ladle_no		= (CString)bcls_rec->Tables[0].Rows[0]["LADLE_NO"].ToString().Trim();
		v_repair_start_time	= (CString)bcls_rec->Tables[0].Rows[0]["REPAIR_START_TIME"].ToString().Trim();
		v_repair_end_time	= (CString)bcls_rec->Tables[0].Rows[0]["REPAIR_END_TIME"].ToString().Trim();		

		Log::Trace(" ", "tmsm06_inq", "v_sm_unit_no[{0}]"	,(const char*)v_sm_unit_no);
		Log::Trace(" ", "tmsm06_inq", "v_ladle_no[{0}]"		,(const char*)v_ladle_no);
		Log::Trace(" ", "tmsm06_inq", "v_repair_start_time[{0}]",(const char*)v_repair_start_time);
		Log::Trace(" ", "tmsm06_inq", "v_repair_end_time[{0}]"	,(const char*)v_repair_end_time);

		if(v_repair_start_time.Trim().GetLength()>= 1 && v_repair_start_time.Trim().GetLength() <= 14)
		{
			v_repair_start_time = (v_repair_start_time + "00000000").Trim().Substring(0,14);
		}
		if(v_repair_end_time.Trim().GetLength()>= 1 && v_repair_end_time.Trim().GetLength() <= 14)
		{
			v_repair_end_time = (v_repair_end_time + "999999999").Trim().Substring(0,14);
		}
		Log::Trace(" ", "tmsm06_inq", "v_repair_start_time**[{0}]",(const char*)v_repair_start_time);
		Log::Trace(" ", "tmsm06_inq", "v_repair_end_time**[{0}]"	,(const char*)v_repair_end_time);
		
		//建立查询数据SQL语句查询钢包使用实绩表--分页
		int nStart  	=  (int)bcls_rec->Tables[1].Rows[0]["START"];
		int nPageSize	=  (int)bcls_rec->Tables[1].Rows[0]["PAGE_SIZE"];
		
		//建立查询数据SQL语句查询钢包使用实绩表
		sqlstr =	" SELECT COUNT(1) "
					" FROM   TTMSM04 "
					" WHERE  1=1 ";
		if(!v_sm_unit_no.Trim().IsEmpty())
		{
			sqlstr += " AND SM_UNIT_NO = @v_sm_unit_no ";
		}
		if(!v_ladle_no.Trim().IsEmpty())
		{
			sqlstr += " AND LADLE_NO LIKE @v_ladle_no ";
		}
		if(!v_repair_start_time.Trim().IsEmpty())
		{
			sqlstr += " AND REPAIR_START_TIME >= @v_repair_start_time ";
		}
		if(!v_repair_end_time.Trim().IsEmpty())
		{
			sqlstr += " AND REPAIR_END_TIME <= @v_repair_end_time ";
		}

		//连接数据库
		cmd1.SetCommandText( sqlstr );

		//传递参数进SQL语句
		cmd1.Parameters.Set("v_sm_unit_no"	, v_sm_unit_no);
		cmd1.Parameters.Set("v_ladle_no"	, v_ladle_no + "%");
		cmd1.Parameters.Set("v_repair_start_time", v_repair_start_time);
		cmd1.Parameters.Set("v_repair_end_time"	, v_repair_end_time);
		
		CDecimal rc = cmd1.ExecuteScalar();
		bcls_ret -> ExtendedProperties.Add("RC",rc.ToString());
		//分页over
		
		//建立查询数据SQL语句查询钢包使用实绩表
		sqlstr =	" SELECT * "
					" FROM   TTMSM04 "
					" WHERE  1=1 ";
		if(!v_sm_unit_no.Trim().IsEmpty())
		{
			sqlstr += " AND SM_UNIT_NO = @v_sm_unit_no ";
		}
		if(!v_ladle_no.Trim().IsEmpty())
		{
			sqlstr += " AND LADLE_NO LIKE @v_ladle_no ";
		}
		if(!v_repair_start_time.Trim().IsEmpty())
		{
			sqlstr += " AND REPAIR_START_TIME >= @v_repair_start_time ";
		}
		if(!v_repair_end_time.Trim().IsEmpty())
		{
			sqlstr += " AND REPAIR_END_TIME <= @v_repair_end_time ";
		}
		sqlstr += " ORDER BY REPAIR_START_TIME DESC , LADLE_NO ASC ";

		//连接数据库
		//CDbCommand cmd(sqlstr, conn);
		cmd.SetCommandText( sqlstr );

		//传递参数进SQL语句
		cmd.Parameters.Set("v_sm_unit_no"	, v_sm_unit_no);
		cmd.Parameters.Set("v_ladle_no"		, v_ladle_no + "%");
		cmd.Parameters.Set("v_repair_start_time", v_repair_start_time);
		cmd.Parameters.Set("v_repair_end_time"	, v_repair_end_time);
		
		//执行SQL语句
		int count=cmd.ExecuteQuery(bcls_ret->Tables[0],nStart,nPageSize);

		bcls_ret->Tables[0].set_TableName("TTMSM04");
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